/**
 *  @header4iode
 *
 *  Functions to manage Table objects.*  
 */
#include "scr4/s_prost.h"

#include "api/constants.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/k_lang.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/tables.h"
#include "api/print/print.h"
#include "api/utils/buf.h"
#include "api/report/undoc/undoc.h"
#include "cpp_api/computed_table/computed_table.h"

bool debug_unpack = false;
bool debug_pack = false;

// ========================= TableCell methods ========================= //

// If quotes is true, the content is returned between double quotes.
std::string TableCell::get_content(const bool quotes, const bool to_utf8) const
{
    if(type == TABLE_CELL_LEC)
    {
        if(!idt) 
            return "";
        else
            return idt->get_lec();
    }
    else
    {
        if(content.empty())
            return "";
        std::string _content = content;
        if(quotes) 
            _content = '\"' + _content + '\"';
        if(to_utf8)
            _content = oem_to_utf8(_content);
        return _content;
    }
}

// to_oem set to false means that the text is already in OEM encoding
// i.e. when reading from old binary files
void TableCell::set_text(const std::string& text, const bool to_oem)
{   
    type = TABLE_CELL_STRING;
    if(idt)
        delete idt;
    idt = nullptr;

    // alignment rules:
    // - TABLE_CELL_LEFT if previously TABLE_CELL_DECIMAL
    // - TABLE_CELL_CENTER if the text contains the char '#' indicating
    //   a time period (col title).
    if(get_align() == TABLE_CELL_DECIMAL)
        set_align(TABLE_CELL_LEFT);
    if(text.find('#') != std::string::npos) 
        set_align(TABLE_CELL_CENTER);

    if(text.empty())
    {
        content = "";
        return;
    }

    std::string text_oem = text;
    if(to_oem)
        text_oem = utf8_to_oem(text_oem);

    // remove the leading and trailing double quotes
    if (text_oem.front() == '\"') 
        text_oem.erase(0, 1);
    if (text_oem.back() == '\"')
        text_oem.pop_back();

    content = text_oem;
}

void TableCell::set_lec(const std::string& lec)
{
    type = TABLE_CELL_LEC;
    content = "";
    
    if(idt)
        delete idt;
    idt = nullptr;

    if(lec.empty()) 
        return;

    try
    {
        this->idt = new Identity(lec);
    }
    catch(std::exception& e)
    {
        kwarning(e.what());
        this->idt = nullptr;
    }
}

// Rule: If the content starts with a double quotes, we assume it is a string cell. 
//       Otherwise, it is a LEC cell.
// to_oem set to false means that the text is already in OEM encoding
// i.e. when reading from old binary files
void TableCell::set_content(const std::string& content, const bool to_oem)
{
    if(content.starts_with('\"'))
        set_text(content, to_oem);
    else
        set_lec(content);
}

// -------- PRINT DEF --------

/**
 *  Print a TABLE cell optionally on several columns.
 *  
 *  @param [in] nb_columns int   number of columns occupied by the cell
 *  @return                bool  true if successful, false otherwise
 */
bool TableCell::print_definition(int nb_columns) const
{
    if(!check_print_def()) 
    {
        W_printfRepl((char*) "&%dL ", nb_columns);
        return true;
    }

    int attribute = (int) get_attribute();
    std::string content = get_content(false, false);
    switch(get_type()) 
    {
        case TABLE_CELL_STRING :
            T_open_cell(attribute, nb_columns, TABLE_CELL_STRING);
            T_open_attr(attribute);
            W_printf((char*) "\"%s\"", (char*) content.c_str());
            break;

        case TABLE_CELL_LEC :
            W_printfRepl((char*) "&%dL", nb_columns);
            T_open_attr(attribute);
            W_printf((char*) "%s", (char*) content.c_str());
            break;
    }
    
    T_close_attr(attribute);
    return true;
}

TableCell32 TableCell::convert_64_to_32bits()
{
    TableCell32 tc32;

    TableCellType type64 = this->get_type();
    if(type64 == TABLE_CELL_STRING)
    {
        // NOTE: always set content to 1, even if the string is empty.
        tc32.content = 1; 
        tc32.type = (char) type64;
        tc32.attribute = this->get_attribute();
    }
    else
    {
        tc32.content = this->is_null() ? 0 : 1;
        tc32.type = (char) type64;
        tc32.attribute = this->get_attribute();
    }
    memset(tc32.pad, '\0', 2);

    return tc32;
}

bool TableCell::convert_32_to_64bits(const TableCell32& cell32)
{
    if(cell32.type == TABLE_CELL_STRING)
        this->set_text("");
    // NOTE: if cell32.type == 0, assume it is a LEC cell with content = ""
    //       This can happen when reading binary formatted files
    else if(cell32.content != 0)
        this->set_lec("1");
    else
        this->set_lec("");

    return true;
}

/**
 *  Serializes a Table cell
 *  
 *  @param [in, out]    pointer to the pack before adding the cell
 *  @return             pointer to the new pack
 */
char* TableCell::to_binary(char *pack, int& p, int i, int j)
{
    int len;
    if(this->get_type() == TABLE_CELL_LEC)
    {
        std::string lec = this->get_content();
        if(lec.empty())
            return pack;

        char* pack_idt = NULL;
        Identity idt(lec);
        idt.to_binary(&pack_idt);
        len = P_len(pack_idt);
        pack = (char*) P_add(pack, pack_idt, len);
        p++;

        debug_packing("cell", "LEC  ", p, i, j, lec, len);
    }
    else
    {
        std::string text = this->get_content(false, false);
        char* c_text = (char*) text.c_str();
        len = (int) strlen(c_text) + 1;
        pack = (char*) P_add(pack, c_text, len);
        p++;

        debug_packing("cell", "STR  ", p, i, j, text, len);
    }

    return pack;
}

bool TableCell::from_binary(char *pack, int& p, int i, int j)
{
    char* value = (char*) P_get_ptr(pack, p);

    if(this->get_type() == TABLE_CELL_STRING)
    {
        std::string content = (value == NULL) ? "" : std::string(value);
        debug_unpacking("cell", "STR  ", p, i, j, content);
        this->set_text(content, false);
        p++;
        return true;
    }
    else
    {   
        // NOTE: 'value' is a serialized Identity
        char* c_lec = (char*) P_get_ptr(value, 0);
        std::string lec = (c_lec == NULL) ? "" : std::string(c_lec);
        debug_unpacking("cell", "LEC  ", p, i, j, lec);
        this->set_lec(lec);
        p++;
        return true;
    }
}

// see the constructor Table(nb_columns) for the initialization of 
// the special 'div' line
void TableCell::sanitize_div(int j)
{
    // recompile the LEC expression
    if(this->get_type() == TABLE_CELL_STRING)
    {
        std::string lec = this->get_content();
        this->set_lec(lec);
    }

    if(j == 0 && this->is_null())
        this->set_lec("1");
}

std::size_t hash_value(const TableCell& cell)
{
    std::hash<TableCell> cell_hash;
    return cell_hash(cell);
}

// ========================= TableLine methods ========================= //

TableLine32 TableLine::convert_64_to_32bits()
{
    TableLine32 tl32;

    tl32.type = (char) this->get_type();
    tl32.graph_type = (char) this->get_graph_type();
    tl32.right_axis = this->right_axis ? 1 : 0;
    tl32.pad[0] = '\0';

    return tl32;
}

bool TableLine::convert_32_to_64bits(const TableLine32& line32)
{
    this->set_graph_type((TableGraphType) line32.graph_type);
    this->right_axis = (bool) line32.right_axis;
    return true;
}

std::size_t hash_value(const TableLine& line)
{
    std::hash<TableLine> line_hash;
    return line_hash(line);
}


// ========================= Table methods ========================= //

static bool T_initialize_line(TableLine& line, const int nb_columns)
{
    bool success = true;
    switch(line.get_type()) 
    {
        case TABLE_LINE_CELL:
            line.set_graph_type(T_GRAPHDEFAULT);
            for(int i = 0; i < nb_columns; i++)
                line.cells.push_back(TableCell(TABLE_CELL_LEC, "", i));
            break;
        case TABLE_LINE_TITLE:
            line.cells.push_back(TableCell(TABLE_CELL_STRING));
            line.cells.back().set_align(TABLE_CELL_CENTER);
            line.cells.back().set_bold(true);   // default attribute for title line is bold
            break;
        case TABLE_LINE_SEP:
        case TABLE_LINE_FILES:
        case TABLE_LINE_MODE:
        case TABLE_LINE_DATE:
            break;
        default:
            kwarning("Table: could not initialize new line -> unknown line type %d", line.get_type());
            success = false;
    }

    return success;
}

static void T_initialize_divider(TableLine& divider_line, const int nb_columns)
{
    divider_line.cells.push_back(TableCell(TABLE_CELL_LEC, "1", 0));
    for(int j = 1; j < nb_columns; j++) 
        divider_line.cells.push_back(TableCell(TABLE_CELL_LEC, "", j));
}

static void T_initialize_title(TableLine& title_line, const std::string& def)
{
    std::string title;
    if(global_ws_cmt->contains(def))
        title = global_ws_cmt->get(def);
    else
        title = def;
    title = trim(title);
    title_line.cells[0].set_text(title);
}

static void T_initialize_col_names(TableLine& line, const int nb_columns)
{
    TableCell* cell = &(line.cells[0]);
    cell->set_lec("");
    for(int j = 1; j < nb_columns; j++) 
    {
        cell = &(line.cells[j]);
        cell->set_text("\"#S");
        cell->set_align(TABLE_CELL_CENTER);
    }
}

static std::vector<std::string> expand_lecs(const std::string& lecs)
{
    std::vector<std::string> v_lecs;
	if(!lecs.empty())
	{
		char* pattern = to_char_array(lecs);
		// Retrieves all variable names matching one or more patterns in global_ws_var (similar to grep)
		char* lst = K_expand(VARIABLES, NULL, pattern, '*');
		// Parses a string and replaces @filename and $listname by their contents
        char* OLD_SEPS = A_SEPS;
        A_SEPS = (char*) ";\t\n";
		char** c_lecs = B_ainit_chk(lst, NULL, 0);
        A_SEPS = OLD_SEPS;
        for(int i = 0; i < SCR_tbl_size((unsigned char**) c_lecs); i++)
            v_lecs.push_back(std::string(c_lecs[i]));
		SCR_free(lst);
        SCR_free_tbl((unsigned char**) c_lecs);
	}
    return v_lecs;
}

static std::vector<std::string> expand_lecs(const std::vector<std::string>& lecs)
{
    std::string lecs_str = join(lecs, ";");
    return expand_lecs(lecs_str);
}


Table::Table(const int nb_columns): nb_columns(nb_columns)
{        
    T_initialize_divider(this->divider_line, nb_columns);
}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
	bool mode, bool files, bool date): nb_columns(nb_columns)
{
    T_initialize_divider(this->divider_line, nb_columns);

    append_line(TABLE_LINE_TITLE);
    T_initialize_title(this->lines.back(), def);
    append_line(TABLE_LINE_SEP);

    append_line(TABLE_LINE_CELL);
    T_initialize_col_names(this->lines.back(), nb_columns);
    append_line(TABLE_LINE_SEP);

    std::string lec;
    Comment comment;
    std::string line_name;
    std::vector<std::string> v_vars = expand_lecs(vars);
    for(const std::string& var: v_vars) 
    {
        append_line(TABLE_LINE_CELL);
        TableLine& line = lines.back();

        // ---- line name (left column) ----
        if(global_ws_cmt->contains(var))
        {
            comment = global_ws_cmt->get(var);
            line_name = trim(comment);
        }
        else
            line_name = var;

        line.cells[0].set_text(line_name);

        // ---- LEC expression (right column) ----
        lec = var;
        for(int j = 1; j < nb_columns; j++)
            line.cells[j].set_lec(lec);
    }
    
    if(mode || files || date)
        append_line(TABLE_LINE_SEP);
    
    if(mode)  
        append_line(TABLE_LINE_MODE);
    
    if(files) 
        append_line(TABLE_LINE_FILES);
    
    if(date)  
        append_line(TABLE_LINE_DATE);
}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
	const std::vector<std::string>& lecs, bool mode, bool files, bool date)
    : nb_columns(nb_columns)
{
    T_initialize_divider(this->divider_line, nb_columns);

    append_line(TABLE_LINE_TITLE);
    T_initialize_title(this->lines.back(), def);
    append_line(TABLE_LINE_SEP);

    append_line(TABLE_LINE_CELL);
    T_initialize_col_names(this->lines.back(), nb_columns);
    append_line(TABLE_LINE_SEP);

    std::string lec;
    std::string line_name;
    std::vector<std::string> v_lecs = expand_lecs(lecs);
    if(v_lecs.size() != titles.size())
    {
        std::string error_msg = "Table constructor: titles and lecs must have the same size ";
        error_msg += "after expansion of possible lists";
        throw std::invalid_argument(error_msg);
    }

    std::string comment;
    for(int i = 0; i < (int) titles.size(); i++)
    {
        append_line(TABLE_LINE_CELL);
        TableLine& line = lines.back();

        // ---- line name (left column) ----
        if(global_ws_cmt->contains(line_name))
        {
            comment = global_ws_cmt->get(line_name);
            line_name = trim(comment);
        }
        else
            line_name = titles[i];

        line.cells[0].set_text(line_name);

        // ---- LEC expression (right column) ----
        lec = v_lecs[i];
        for(int j = 1; j < nb_columns; j++)
            line.cells[j].set_lec(lec);
    }
    
    if(mode || files || date)
        append_line(TABLE_LINE_SEP);
    
    if(mode)  
        append_line(TABLE_LINE_MODE);
    
    if(files) 
        append_line(TABLE_LINE_FILES);
    
    if(date)  
        append_line(TABLE_LINE_DATE);
}

Table::Table(const int nb_columns, const std::string& def, const std::string& lecs, 
	bool mode, bool files, bool date): nb_columns(nb_columns)
{
    T_initialize_divider(this->divider_line, nb_columns);

    append_line(TABLE_LINE_TITLE);
    T_initialize_title(this->lines.back(), def);
    append_line(TABLE_LINE_SEP);

    append_line(TABLE_LINE_CELL);
    T_initialize_col_names(this->lines.back(), nb_columns);
    append_line(TABLE_LINE_SEP);

    std::string comment;
    std::string line_name;
    std::vector<std::string> v_lecs = expand_lecs(lecs);
    for(const std::string& lec: v_lecs) 
    {
        append_line(TABLE_LINE_CELL);
        TableLine& line = lines.back();

        // ---- line name (left column) ----
        if(global_ws_cmt->contains(lec))
        {
            comment = global_ws_cmt->get(lec);
            line_name = trim(comment);
        }
        else
            line_name = lec;

        line.cells[0].set_text(line_name);

        // ---- LEC expression (right column) ----
        for(int j = 1; j < nb_columns; j++)
            line.cells[j].set_lec(lec);
    }
    
    if(mode || files || date)
        append_line(TABLE_LINE_SEP);
    
    if(mode)  
        append_line(TABLE_LINE_MODE);
    
    if(files) 
        append_line(TABLE_LINE_FILES);
    
    if(date)  
        append_line(TABLE_LINE_DATE);
}

Table::Table(const Table& table): nb_columns(table.nb_columns)
{
    this->set_language(table.get_language());
    this->set_gridx(table.get_gridx());
    this->set_gridy(table.get_gridy());
    this->set_graph_axis(table.get_graph_axis());
    this->set_text_alignment(table.get_text_alignment());
    repeat_columns = table.repeat_columns;
    
    divider_line.set_graph_type(table.divider_line.get_graph_type());
    divider_line.right_axis = table.divider_line.right_axis;
    divider_line.cells.clear();
    for(const TableCell& cell : table.divider_line.cells)
        divider_line.cells.push_back(cell);

    lines.clear();
    for(const TableLine& line : table.lines)
        lines.push_back(line);
    
    z_min = table.z_min;
    z_max = table.z_max;
    y_min = table.y_min;
    y_max = table.y_max;
    attribute = table.attribute;
    chart_box = table.chart_box;
    chart_shadow = table.chart_shadow;
}

Table::~Table()
{
    divider_line.cells.clear();
    
    if(lines.size() > 0)
    {
        for(int i = 0; i < lines.size(); i++)
            lines[i].cells.clear();
        lines.clear();
    }
}

// -------- LINES --------

TableLine* Table::append_line(const TableLineType line_type)
{
    TableLine line((TableLineType) line_type);
    bool success = T_initialize_line(line, nb_columns);
	if(!success) 
	    throw std::runtime_error("Cannot append a new line to the table");

    lines.push_back(line);
	return &this->lines.back();
}

TableLine* Table::insert_line(const int pos, const TableLineType line_type, const bool after)
{
    if(pos < 0 || pos >= this->lines.size())
    {
        std::string error_msg = "Table: failed to insert a line in the table at position " 
                                + std::to_string(pos) + ": position out of range [0," 
                                + std::to_string(lines.size() - 1) + "]";
        kerror(0, (char*) error_msg.c_str());
    }

    TableLine line((TableLineType) line_type);
    bool success = T_initialize_line(line, nb_columns);
    if(!success)
    {
        std::string error_msg = "Table: cannot insert a new line in the table at position " 
                                + std::to_string(pos);
        kerror(0, (char*) error_msg.c_str());
    }

    int insert_pos = after ? pos + 1 : pos;
    lines.insert(lines.begin() + insert_pos, line);

	return &this->lines[insert_pos];
}

// -------- REMOVE --------

void Table::remove_line(const int row)
{
    if(row < 0 || row >= lines.size())
        throw std::out_of_range("Table line index " + std::to_string(row) + " is out of range [0, " + 
            std::to_string(lines.size()) + ").");
    lines.erase(lines.begin() + row);
}

// -------- OTHER METHODS --------

Table32 Table::convert_64_to_32bits()
{
    Table32 tbl32;

    tbl32.language = (short) this->get_language();
    tbl32.repeat_columns = this->repeat_columns;
    tbl32.nb_columns = this->nb_columns;
    tbl32.nb_lines = (int) this->lines.size();

    tbl32.divider_line = this->divider_line.convert_64_to_32bits();

    tbl32.z_min = this->z_min;
    tbl32.z_max = this->z_max;
    tbl32.y_min = this->y_min;
    tbl32.y_max = this->y_max;
    tbl32.attribute = this->attribute;
    tbl32.chart_box = this->chart_box;
    tbl32.chart_shadow = this->chart_shadow;
    tbl32.chart_gridx = (char) this->get_gridx();
    tbl32.chart_gridy = (char) this->get_gridy();
    tbl32.chart_axis_type = (char) this->get_graph_axis();
    tbl32.text_alignment = (char) this->get_text_alignment();
    memset(tbl32.pad, '\0', sizeof(tbl32.pad));

    return tbl32;
}

bool Table::convert_32_to_64bits(const Table32& table32)
{
    this->set_language((TableLang) table32.language);
    this->repeat_columns = table32.repeat_columns;
    this->nb_columns = table32.nb_columns;

    this->divider_line.convert_32_to_64bits(table32.divider_line);

    this->z_min = table32.z_min;
    this->z_max = table32.z_max;
    this->y_min = table32.y_min;
    this->y_max = table32.y_max;
    this->attribute = table32.attribute;
    this->chart_box = table32.chart_box;
    this->chart_shadow = table32.chart_shadow;
    this->set_gridx((TableGraphGrid) table32.chart_gridx);
    this->set_gridy((TableGraphGrid) table32.chart_gridy);
    this->set_graph_axis((TableGraphAxis) table32.chart_axis_type);
    this->set_text_alignment((TableTextAlign) table32.text_alignment);

    return true;
}

Table* binary_to_tbl(char* pack)
{
    int p = 0;

    /* table header */
    Table32* tbl32 = (Table32*) P_get_ptr(pack, 0);
    Table* tbl = new Table(tbl32->nb_columns);
    tbl->convert_32_to_64bits(*tbl32);
    debug_unpacking("Table    ", "", 0);

    /* divider line */
    TableCell32* cells32 = (TableCell32*) P_get_ptr(pack, 1);
    debug_unpacking("line", "DIV  ", 1);

    // NOTE: new Table(tbl32->nb_columns) above creates a table with a 'divider' line 
    //       with nb_columns cells of type TABLE_CELL_LEC and content = ""
    p = 2;
    int k = 0;
    for(TableCell& cell : tbl->divider_line.cells) 
    {
        TableCell32* cell32 = cells32 + k;
        cell.convert_32_to_64bits(*cell32);
        debug_unpacking("cell", "DIV  ", p, 0, k, (cell32->content == 0) ? " NO" : " YES");
        if(cell32->content != 0)
            cell.from_binary(pack, p, 0, k);
        cell.set_attribute(cell32->attribute);
        cell.sanitize_div(k);
        k++;
    }
    
    /* lines */
    TableLine32* lines32 = (TableLine32*) P_get_ptr(pack, p);
    debug_unpacking("LINES    ", "", p);
    p++;

    TableCell* cell = nullptr;
    TableLine32* line32 = nullptr;
    TableCell32* cell32 = nullptr;
    for(int i = 0; i < tbl32->nb_lines; i++) 
    {
        line32 = lines32 + i;
        switch(line32->type) 
        {
            case TABLE_LINE_CELL:
            {
                TableLine line(TableLineType::TABLE_LINE_CELL);
                line.convert_32_to_64bits(*line32);

                cells32 = (TableCell32*) P_get_ptr(pack, p);
                debug_unpacking("line", "CELL ", p, i);
                p++;

                for(int j = 0; j < tbl->nb_columns; j++) 
                {
                    cell32 = cells32 + j;
                    // if cell type not properly set, assume it is a LEC cell
                    // -> may happen when reading binary formatted files <-
                    if(!(cell32->type == TABLE_CELL_STRING || cell32->type == TABLE_CELL_LEC))
                        cell32->type = TABLE_CELL_LEC;
                    if(cell32->type == TABLE_CELL_STRING)
                        cell = new TableCell(TABLE_CELL_STRING);
                    else
                        cell = new TableCell(TABLE_CELL_LEC, "", j);
                    cell->convert_32_to_64bits(*cell32);
                    if(cell32->content != 0) 
                        cell->from_binary(pack, p, i, j);
                    cell->set_attribute(cell32->attribute);
                    line.cells.push_back(*cell);
                }

                tbl->lines.push_back(line);
                break;
            }
            case TABLE_LINE_TITLE:
            {
                TableLine line(TableLineType::TABLE_LINE_TITLE);
                line.convert_32_to_64bits(*line32);

                cell32 = (TableCell32*) P_get_ptr(pack, p);
                debug_unpacking("line", "TITLE", p, i);
                p++;

                cell = new TableCell(TABLE_CELL_STRING);
                cell->convert_32_to_64bits(*cell32);
                if(cell32->content != 0) 
                    cell->from_binary(pack, p, i, 0);
                cell->set_attribute(cell32->attribute);
                line.cells.push_back(*cell);

                tbl->lines.push_back(line);
                break;
            }
            case TABLE_LINE_SEP:
            {
                TableLine line(TableLineType::TABLE_LINE_SEP);
                debug_unpacking("line", "SEP  ", p, i);
                tbl->lines.push_back(line);
                break;
            }
            case TABLE_LINE_MODE:
            {
                TableLine line(TableLineType::TABLE_LINE_MODE);
                debug_unpacking("line", "MODE ", p, i);
                tbl->lines.push_back(line);
                break;
            }
            case TABLE_LINE_FILES:
            {
                TableLine line(TableLineType::TABLE_LINE_FILES);
                debug_unpacking("line", "FILES", p, i);
                tbl->lines.push_back(line);
                break;
            }
            case TABLE_LINE_DATE:
            {
                TableLine line(TableLineType::TABLE_LINE_DATE);
                debug_unpacking("line", "DATE ", p, i);
                tbl->lines.push_back(line);
                break;
            }
            default:
                std::string msg = "Unpacking table line: invalid line type " + std::to_string(line32->type); 
                msg += " at line " + std::to_string(i);
                kwarning(msg.c_str());
                break;
        }
    }

    return tbl;
}

std::size_t hash_value(const Table& table)
{
    std::hash<Table> tbl_hash;
    return tbl_hash(table);
}

// ========================= KDBTables methods ========================= //

std::string KDBTables::get_title(const std::string& name) const
{
	// throw exception if table with passed position is not valid
	if(!this->contains(name))
		throw std::out_of_range("Cannot get title of table with name '" + name + "'.\n" +
			                    "The table with name '" + name + "' does not exist in the database.");
    Table* table = this->get_obj_ptr(name);
    std::string title = std::string((char*) T_get_title(table));
    return title;
}

bool KDBTables::add(const std::string& name, const int nbColumns)
{
	Table table(nbColumns);
	return KDBTemplate::add(name, table);
}

bool KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, 
	const std::vector<std::string>& vars, bool mode, bool files, bool date)
{
	Table table(nbColumns, def, vars, mode, files, date);
	return KDBTemplate::add(name, table);
}

bool KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, 
	const std::vector<std::string>& titles, const std::vector<std::string>& lecs, 
	bool mode, bool files, bool date)
{
	Table table(nbColumns, def, titles, lecs, mode, files, date);
	return KDBTemplate::add(name, table);
}

bool KDBTables::add(const std::string& name, const int nbColumns, const std::string& def, 
	const std::string& lecs, bool mode, bool files, bool date)
{
	Table table(nbColumns, def, lecs, mode, files, date);
	return KDBTemplate::add(name, table);
}

void KDBTables::print_to_file(const std::string& destination_file, const std::string& gsample, 
	const std::string& names, const int nb_decimals, const char format)
{
	ComputedTable::initialize_printing(destination_file, format);

	Table* table;
	ComputedTable* computed_table;
	std::set<std::string> v_names = filter_names(names);
	for(const std::string& name : v_names)
	{
		table = this->get_obj_ptr(name);
		computed_table = new ComputedTable(table, gsample, nb_decimals);
		computed_table->print_to_file();
		delete computed_table;
	}

	ComputedTable::finalize_printing();
}

bool KDBTables::binary_to_obj(const std::string& name, char* pack)
{
    debug_unpacking("table " + name, "--------------------------------", -1);
    Table* tbl = binary_to_tbl(pack);
    if(!tbl)
        return false;

    this->k_objs[name] = tbl;
    return true;
}

/**
 * Serializes a Table object. 
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the packed Table
 * @param [in]  name    string      table name
 * @return                          true if the serialization succeeded, false otherwise 
*/
bool KDBTables::obj_to_binary(char** pack, const std::string& name)
{
    debug_packing("table " + std::string(name), "--------------------------------", -1);
    
    *pack = NULL;
    Table* tbl = this->get_obj_ptr(name);
    if(!tbl)
        return false;

    int p = -1;
    *pack = (char*) P_create();

    /* table header */

    Table32 tbl32 = tbl->convert_64_to_32bits();
    *pack = (char*) P_add(*pack, (char*) &tbl32, sizeof(Table32));
    p++;
    debug_packing("Table      ", "", p);

    /* divider line */

    // a) serialize cells except their content
    std::vector<TableCell32> cells32; 
    cells32.reserve(tbl->nb_columns);
    for(TableCell& cell : tbl->divider_line.cells)
    {
        TableCell32 cell32 = cell.convert_64_to_32bits();
        cells32.push_back(cell32);
    }
    int nb_cells = (int) cells32.size();
    *pack = (char*) P_add(*pack, (char*) cells32.data(), sizeof(TableCell32) * nb_cells);
    p++;

    debug_packing("line", "DIV  ", p);

    int k = 0;
    for(TableCell32& cell32 : cells32)
    {
        debug_packing("cell", "DIV  ", p+k+1, 0, k, (cell32.content == 1) ? "YES" : "NO");
        k++;
    }

    cells32.clear();

    // b) serialize cells content
    k = 0;
    for(TableCell& cell : tbl->divider_line.cells)
    {
        *pack = cell.to_binary(*pack,  p, 0, k);
        k++;
    }

    /* lines */

    // a) serialize lines except their cells
    std::vector<TableLine32> lines32;
    int nb_lines = (int) tbl->lines.size();
    lines32.reserve(nb_lines);
    for(TableLine& line : tbl->lines)
    {
        TableLine32 line32 = line.convert_64_to_32bits();
        lines32.push_back(line32);
    }
    *pack = (char*) P_add(*pack, (char*) lines32.data(), sizeof(TableLine32) * nb_lines);
    p++;

    debug_packing("LINES    ", "", p);

    lines32.clear();

    // b) serialize cells of lines
    int i = 0;
    for(TableLine& line: tbl->lines) 
    {
        switch(line.get_type()) 
        {
            case TABLE_LINE_CELL:
            {
                // a) serialize cells except their content
                cells32.reserve(tbl->nb_columns);
                for(TableCell& cell : line.cells) 
                {
                    TableCell32 cell32 = cell.convert_64_to_32bits();
                    cells32.push_back(cell32);
                }
                *pack = (char*) P_add(*pack, (char*) cells32.data(), sizeof(TableCell32) * tbl->nb_columns);
                p++;

                debug_packing("line", "CELL ", p, i);

                int j = 0;
                for(TableCell32& cell32 : cells32)
                {
                    debug_packing("content? ", "", p+j+1, i, j, (cell32.content == 1) ? "YES" : "NO");
                    j++;
                }
                
                cells32.clear();

                // b) serialize cells content
                j = 0;
                for(TableCell& cell : line.cells)
                {
                    *pack = cell.to_binary(*pack, p, i, j);
                    j++;
                } 
                break;
            }
            case TABLE_LINE_TITLE:
            {
                TableCell cell = line.cells[0];
                // a) serialize cell except its content
                TableCell32 cell32 = cell.convert_64_to_32bits();
                *pack = (char*) P_add(*pack, (char*) &cell32, sizeof(TableCell32));
                p++;

                debug_packing("line", "TITLE", p, i);
                debug_packing("content? ", "", p+1, i, 0, (cell32.content == 1) ? "YES" : "NO");

                // b) serialize cell content
                *pack = cell.to_binary(*pack, p, i, 0);
                break;
            }
            case TABLE_LINE_SEP:
                debug_packing("line", "SEP  ", p, i);
                break;
            case TABLE_ASCII_LINE_MODE:
                debug_packing("line", "MODE ", p, i);
                break;
            case TABLE_LINE_FILES:
                debug_packing("line", "FILES", p, i);
                break;
            case TABLE_LINE_DATE:
                debug_packing("line", "DATE ", p, i);
                break;
            default:
                std::string msg = "Packing table line: invalid line type " + std::to_string(line.get_type()); 
                msg += " at line " + std::to_string(i);
                kwarning(msg.c_str());
                break;
        }
        i++;
    }

    return true;
}

bool KDBTables::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    std::string text;

    Table* tbl = const_cast<KDBTables*>(this)->get_obj_ptr(name);
    for(const TableLine& tline : tbl->lines) 
    {
        if(found) 
            break;
        
        switch(tline.get_type()) 
        {
            case TABLE_LINE_SEP   :
            case TABLE_LINE_MODE  :
            case TABLE_LINE_DATE  :
            case TABLE_LINE_FILES :
                break;
            case TABLE_LINE_TITLE :
            {
                TableCell cell = tline.cells[0];
                if(texts)
                {
                    text = cell.get_content(true);
                    found = wrap_grep_gnl(pattern, text, ecase, all);
                } 
                break;
            }
            case TABLE_LINE_CELL :
            {
                found = false;
                for(const TableCell& cell : tline.cells)
                {
                    if(found) 
                        break;
                    
                    if((texts && cell.get_type() == TABLE_CELL_STRING) || 
                       (forms && cell.get_type() == TABLE_CELL_LEC))
                       {
                            text = cell.get_content(true);
                            found = wrap_grep_gnl(pattern, text, ecase, all);
                       }
                }
                break;
            }
        }
    }
    
    return found;
}

/**
 *  Compute a table on the GSample ismpl and return a string containing the result.
 */
char* KDBTables::dde_create_table(const std::string& name, char *ismpl, int *nc, int *nl, int nbdec)
{
    int     dim, i, j, d, rc = 0, nli = 0,
                          nf = 0, nm = 0;
    char    gsmpl[128], **l = NULL, *buf, *res = NULL;
    COLS    *cls;

    Table* tbl = global_ws_tbl->get_obj_ptr(name);
    Sample* smpl = global_ws_var->sample;

    /* date */
    char date[11];

    if(smpl == nullptr || smpl->nb_periods == 0) 
        return (char*) "";

    /* mode */
    if(!ismpl)
        sprintf(gsmpl, "%s:%d", (char*) smpl->start_period.to_string().c_str(), smpl->nb_periods);
    else
        sprintf(gsmpl, "%s", ismpl);

    dim = T_prep_cls(tbl, gsmpl, &cls);
    if(dim < 0) 
        return((char*) SCR_stracpy((unsigned char*) "Error in Tbl or Smpl"));

    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size((unsigned char**) KT_names);
    if(KT_nbnames == 0) 
        return((char*) SCR_stracpy((unsigned char*) "Error in Tbl or Smpl"));
    COL_find_mode(cls, KT_mode, 2);

    *nc = dim + 1;
    *nl = 1;

    TableLine* line;
    buf = SCR_malloc(256 + dim * 128);
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) 
    {
        buf[0] = 0;
        line = &tbl->lines[i];

        switch(line->get_type()) 
        {
            case TABLE_LINE_SEP   :
                break;
            case TABLE_LINE_DATE  :
                strcat(buf, SCR_long_to_fdate(SCR_current_date(), date, (char*) "dd/mm/yy"));
                break;
            case TABLE_LINE_MODE  :
                for(j = 0; j < MAX_MODE; j++) 
                {
                    if(KT_mode[j] == 0) 
                        continue;
                    sprintf(date, "(%s) ", COL_OPERS[j + 1]);
                    strcat(buf, date);
                    strcat(buf, KLG_OPERS_TEXTS[j + 1][K_LANG]);
                    strcat(buf, "\n");
                    nm ++;
                }
                break;
            case TABLE_LINE_FILES :
                for(j = 0; KT_names[j]; j++) 
                {
                    strcat(buf, KT_names[j]);
                    strcat(buf, "\n");
                    nf ++;
                }
                break;
            case TABLE_LINE_TITLE :
                strcat(buf, IodeTblCell(&(line->cells[0]), NULL, nbdec));
                //strcat(buf,"\x01\x02\03"); // JMP 13/7/2022
                break;
            case TABLE_LINE_CELL  :
                COL_clear(cls);
                if(COL_exec(tbl, i, cls) < 0)
                    strcat(buf, "Error in calc");
                else
                    for(j = 0; j < cls->cl_nb; j++) 
                    {
                        d = j % T_NC(tbl);
                        if(tbl->repeat_columns == 0 && d == 0 && j != 0) 
                            continue;
                        strcat(buf, IodeTblCell(&(line->cells[d]), cls->cl_cols + j, nbdec));
                        strcat(buf, "\t");
                    }
                break;
        }

        if(buf[0]) 
        {
            SCR_add_ptr((unsigned char***) &l, &nli, (unsigned char*) buf);
            (*nl)++;
        }
    }
    
    SCR_add_ptr((unsigned char***) &l, &nli, NULL);
    *nl += nf + nm;
    res = (char*) SCR_mtov((unsigned char**) l, '\n');

    COL_free_cols(cls);
    SCR_free_tbl((unsigned char**) l);
    SCR_free(buf);

    SCR_free_tbl((unsigned char**) KT_names);
    KT_names = NULL;
    KT_nbnames = 0;

    return res;
}

char* KDBTables::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    return  NULL;
}

/**
 *  Print the table in position pos in kdb.  
 *  
 *  If B_TABLE_TITLE != 0, print the table title.
 *  If B_TABLE_TITLE == 1, print the table name and its title.
 *  If B_TABLE_TITLE == 0, print the table definition.
 *  
 *  @param [in] kdb KDB*        KDB of tables
 *  @param [in] pos int         position of the table in kdb
 *  @return         int         -1 if the table cannot be found in kdb
 */
bool KDBTables::print_obj_def(const std::string& name)
{
    Table* tbl = this->get_obj_ptr(name);
    if(!tbl) 
        return false;
    
    if(B_TABLE_TITLE) 
    {
        if(B_TABLE_TITLE == 1) 
            W_printfReplEsc((char*) "\n~b%s~B : %s\n", name.c_str(), T_get_title(tbl, false));
        else 
            W_printf((char*) "\n%s\n", T_get_title(tbl, false));
        return true;
    }

    B_PrintRtfTopic((char*) T_get_title(tbl, false));
    W_printf((char*) ".tb %d\n", T_NC(tbl));
    W_printfRepl((char*) ".sep &\n");
    W_printfRepl((char*) "&%dC%cb%s : definition%cB\n", T_NC(tbl), A2M_ESCCH, name.c_str(), A2M_ESCCH);
    bool success = tbl->print_definition();
    W_printf((char*) ".te\n");
    return success;
}

void KDBTables::update_reference_db()
{
    if(global_ref_tbl[0]) 
        delete global_ref_tbl[0];
    global_ref_tbl[0] = new KDBTables(this, "*", false);      
}
