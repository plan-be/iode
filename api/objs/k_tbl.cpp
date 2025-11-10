/**
 *  @header4iode
 *
 *  Functions to manage Table objects.*  
 */
#include "scr4/s_prost.h"

#include "api/constants.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/tables.h"
#include "api/utils/buf.h"


// ========================= TableCell methods ========================= //

// If quotes is true, the content is returned between double quotes.
std::string TableCell::get_content(const bool quotes, const bool to_utf8) const
{
    if(type == TABLE_CELL_LEC)
    {
        if(!idt) 
            return "";
        else
            return idt->lec;
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

std::size_t hash_value(const TableCell& cell)
{
    std::hash<TableCell> cell_hash;
    return cell_hash(cell);
}

// ========================= TableLine methods ========================= //

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
    SWHDL handle = K_WS[COMMENTS]->get_handle(def);
    std::string title = (handle > 0) ? std::string(KCVAL(K_WS[COMMENTS], handle)) : def;
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
		// Retrieves all variable names matching one or more patterns in KV_WS (similar to grep)
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
    std::string comment;

    T_initialize_divider(this->divider_line, nb_columns);

    append_line(TABLE_LINE_TITLE);
    T_initialize_title(this->lines.back(), def);
    append_line(TABLE_LINE_SEP);

    append_line(TABLE_LINE_CELL);
    T_initialize_col_names(this->lines.back(), nb_columns);
    append_line(TABLE_LINE_SEP);

    SWHDL handle;
    std::string lec;
    std::string line_name;
    std::vector<std::string> v_vars = expand_lecs(vars);
    for(const std::string& var: v_vars) 
    {
        append_line(TABLE_LINE_CELL);
        TableLine& line = lines.back();

        // ---- line name (left column) ----
        handle = K_WS[COMMENTS]->get_handle(var);
        if(handle == 0)
            line_name = var;
        else
        {
            comment = std::string((char*) KCVAL(K_WS[COMMENTS], handle));
            comment = oem_to_utf8(comment);
            line_name = trim(comment);
        }

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
    std::string comment;

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

    SWHDL handle;
    for(int i = 0; i < (int) titles.size(); i++)
    {
        append_line(TABLE_LINE_CELL);
        TableLine& line = lines.back();

        // ---- line name (left column) ----
        line_name = titles[i];
        handle = K_WS[COMMENTS]->get_handle(line_name);
        if(handle > 0)
        {
            comment = std::string((char*) KCVAL(K_WS[COMMENTS], handle));
            comment = oem_to_utf8(comment);
            line_name = trim(comment);
        }

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
    std::string comment;

    T_initialize_divider(this->divider_line, nb_columns);

    append_line(TABLE_LINE_TITLE);
    T_initialize_title(this->lines.back(), def);
    append_line(TABLE_LINE_SEP);

    append_line(TABLE_LINE_CELL);
    T_initialize_col_names(this->lines.back(), nb_columns);
    append_line(TABLE_LINE_SEP);

    SWHDL handle;
    std::string line_name;
    std::vector<std::string> v_lecs = expand_lecs(lecs);
    for(const std::string& lec: v_lecs) 
    {
        append_line(TABLE_LINE_CELL);
        TableLine& line = lines.back();

        // ---- line name (left column) ----
        handle = K_WS[COMMENTS]->get_handle(lec);
        if(handle == 0)
            line_name = lec;
        else
        {
            comment = std::string((char*) KCVAL(K_WS[COMMENTS], handle));
            comment = oem_to_utf8(comment);
            line_name = trim(comment);
        }

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


std::size_t hash_value(const Table& table)
{
    std::hash<Table> tbl_hash;
    return tbl_hash(table);
}
