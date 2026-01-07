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
    std::string title;
    SWHDL handle = global_ws_cmt->get_handle(def);
    if(handle == 0)
        title = def;
    else
        title = std::string(global_ws_cmt->get_obj(handle));
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
        handle = global_ws_cmt->get_handle(var);
        if(handle == 0)
            line_name = var;
        else
        {
            comment = std::string(global_ws_cmt->get_obj(handle));
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
        handle = global_ws_cmt->get_handle(line_name);
        if(handle > 0)
        {
            comment = std::string(global_ws_cmt->get_obj(handle));
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
        handle = global_ws_cmt->get_handle(lec);
        if(handle == 0)
            line_name = lec;
        else
        {
            comment = std::string(global_ws_cmt->get_obj(handle));
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


Table* CKDBTables::get_obj(const SWHDL handle) const
{    
    std::string name;
    for(const auto& [_name_, _handle_] : this->k_objs) 
    {
        if(_handle_ == handle) 
        {
            name = _name_;
            break;
        }
    }

    return get_obj(name);
}

Table* CKDBTables::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("Table with name '" + name + "' not found.");
    
    char* ptr = SW_getptr(handle);
    if(ptr == nullptr)  
        return nullptr;
    return (Table*) K_tunpack(ptr, (char*) name.c_str());
}

bool CKDBTables::set_obj(const std::string& name, const Table* value)
{
    char* pack = NULL;
    K_tpack(&pack, (char*) value, (char*) name.c_str());
    bool success = set_packed_object(name, pack);
    if(!success)
    {
        std::string error_msg = "Failed to set table object '" + name + "'";
        kwarning(error_msg.c_str());
    }
    return success;
}

bool CKDBTables::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
{
    bool found = false;
    std::string text;

    Table* tbl = KTVAL(const_cast<CKDBTables*>(this), name);
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
    
    delete tbl;
    return found;
}

/**
 *  Compute a table on the GSample ismpl and return a string containing the result.
 */
char* CKDBTables::dde_create_table(const std::string& name, char *ismpl, int *nc, int *nl, int nbdec)
{
    int     dim, i, j, d, rc = 0, nli = 0,
                          nf = 0, nm = 0;
    char    gsmpl[128], **l = NULL, *buf, *res = NULL;
    COLS    *cls;

    Table* tbl = KTVAL(global_ws_tbl.get(), name);
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

    return(res);
}

char* CKDBTables::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
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
bool CKDBTables::print_obj_def(const std::string& name)
{
    Table* tbl = KTVAL(this, name);
    if(!tbl) 
        return false;
    
    if(B_TABLE_TITLE) 
    {
        if(B_TABLE_TITLE == 1) 
            W_printfReplEsc((char*) "\n~b%s~B : %s\n", name.c_str(), T_get_title(tbl, false));
        else 
            W_printf((char*) "\n%s\n", T_get_title(tbl, false));
        
        delete tbl;
        tbl = nullptr;
        return true;
    }

    B_PrintRtfTopic((char*) T_get_title(tbl, false));
    W_printf((char*) ".tb %d\n", T_NC(tbl));
    W_printfRepl((char*) ".sep &\n");
    W_printfRepl((char*) "&%dC%cb%s : definition%cB\n", T_NC(tbl), A2M_ESCCH, name.c_str(), A2M_ESCCH);
    bool success = tbl->print_definition();
    W_printf((char*) ".te\n");

    delete tbl;
    tbl = nullptr;
    return success;
}

void CKDBTables::update_reference_db()
{
    K_RWS[this->k_type][0] = new CKDBTables(this, "*");      
}
