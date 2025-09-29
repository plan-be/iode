/**
 *  @header4iode
 *
 *  Functions to manage TBL objects.
 *  
 *  List of functions 
 *  -----------------
 *      TBL *T_create(int dim)                                                                     | Creates a new TBL object.
 *      void T_free(TBL* tbl)                                                                      | Frees a TBL object
 *      int T_default(TBL* tbl, char*titg, char**titls, char**lecs, int mode, int files, int date) | Fills a TBL with some basic data: a title, line titles and LEC expressions.
 *      void T_auto(TBL* tbl, char* def, char** vars, int mode, int files, int date)               | Fills a TBL with a list of variables and their CMT. 
 *  
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

/**
 *  Creates a new TBL object.
 *  
 *  @param [in]  dim    int     Number of expression columns
 *  @return             TBL*    pointer to a new allocated TBL
 */
TBL *T_create(int dim)
{
    if(dim < 1) 
    {
        kwarning("Table: could not create new table -> number of columns < 1");
        return NULL;
    }

    TBL* tbl = new TBL(dim);
    return tbl;
}


/**
 *  Frees a TBL object.
 *  
 *  @param [in] tbl     TBL*    pointer to the TBL
 *  
 */
void T_free(TBL* tbl)
{
    if(tbl == NULL) 
        return;
    
    tbl->divider_line.cells.clear();
    if(tbl->lines.size() > 0)
    {
        for(int i = 0; i < tbl->lines.size(); i++)
            tbl->lines[i].cells.clear();
        tbl->lines.clear();
    }

    delete tbl;
}


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

// -------- LINES --------

TableLine* TBL::append_line(const TableLineType line_type)
{
    TableLine line((TableLineType) line_type);
    bool success = T_initialize_line(line, nb_columns);
	if(!success) 
	    throw std::runtime_error("Cannot append a new line to the table");

    lines.push_back(line);
	return &this->lines.back();
}

TableLine* TBL::insert_line(const int pos, const TableLineType line_type, const bool after)
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

void TBL::remove_line(const int row)
{
    if(row < 0 || row >= lines.size())
        throw std::out_of_range("Table line index " + std::to_string(row) + " is out of range [0, " + 
            std::to_string(lines.size()) + ").");
    lines.erase(lines.begin() + row);
}


/**
 *  Fills a TBL with some basic data: a title, line titles and LEC expressions.
 *  The TBL must exist and is normally empty.
 *  
 *  @param [in, out] tbl   TBL*     Table to modify
 *  @param [in]      titg  char*    Title of the table (in UTF-8)
 *  @param [in]      titls char**   Titles of the lines (in UTF-8)
 *  @param [in]      lecs  char**   LEC formulas of the lines (// titls)
 *  @param [in]      mode  int      if 1, includes a special TABLE_LINE_MODE line
 *  @param [in]      files int      if 1, includes a special TABLE_LINE_FILES line
 *  @param [in]      date  int      if 1, includes a special TABLE_LINE_DATE line
 *  @return                int      0
 *  
 */
int T_default(TBL* tbl, char* titg, char** titls, char** lecs, int mode, int files, int date)
{
    TableCell* cell;

    if(titg) 
    {
        tbl->append_line(TABLE_LINE_TITLE);
        std::string title = std::string(titg);
        title = trim(title);
        if(!title.empty())
        {
            cell = &(tbl->lines.back().cells[0]);
            cell->set_text(title);
        }
        tbl->append_line(TABLE_LINE_SEP);
    }

    tbl->append_line(TABLE_LINE_CELL);
    cell = &(tbl->lines.back().cells[0]);
    cell->set_lec("");
    for(int j = 1 ; j < T_NC(tbl) ; j++) 
    {
        cell = &(tbl->lines.back().cells[j]);
        cell->set_text("\"#S");
        cell->set_align(TABLE_CELL_CENTER);
    }

    tbl->append_line(TABLE_LINE_SEP);

    if(lecs && titls) 
    {
        for(int i = 0 ; lecs[i] && titls[i]; i++) 
        {
            tbl->append_line(TABLE_LINE_CELL);
            TableLine& line = tbl->lines.back();
            // left column: line title
            std::string label = std::string(titls[i]);
            line.cells[0].set_text(label);
            // other columns: LEC expression
            std::string lec = std::string(lecs[i]);
            for(int j = 1 ; j < T_NC(tbl) ; j++)
                line.cells[j].set_lec(lec);
        }
    }
    else 
        tbl->append_line(TABLE_LINE_CELL);
    
    if(mode || files || date)
        tbl->append_line(TABLE_LINE_SEP);
    
    if(mode)  
        tbl->append_line(TABLE_LINE_MODE);
    
    if(files) 
        tbl->append_line(TABLE_LINE_FILES);
    
    if(date)  
        tbl->append_line(TABLE_LINE_DATE);

    return(0);
}


/**
 *  Fills a TBL with a list of variables and their CMT. 
 *  The TBL must exist and is normally empty.
 *  
 *    - def can be either a CMT name or a free text. If it's a CMT name, the contents of the CMT becomes the table title.
 *    - vars is a list of VAR names. If a CMT with the same name exists, the CMT become the line title and the variable the LEC expression.
 *  
 *  @param [in, out] tbl   TBL*     Table to modify
 *  @param [in]      def   char*    Table title or name of the CMT that must become the table title
 *  @param [in]      vars  char**   NULL terminated list of variable names
 *  @param [in]      mode  int      if 1, includes a special TABLE_LINE_MODE line
 *  @param [in]      files int      if 1, includes a special TABLE_LINE_FILES line
 *  @param [in]      date  int      if 1, includes a special TABLE_LINE_DATE line
 *  @return                int      0
 *  
 */
void T_auto(TBL* tbl, char* def, char** vars, int mode, int files, int date)
{
    int     i, pos,
            nb = SCR_tbl_size((unsigned char**) vars),
            nbt = 0, nbl = 0;
    KDB     *kdb = K_WS[COMMENTS];
    char    *titg, **titls = NULL, **lecs = NULL;
    std::string comment;

    pos = K_find(kdb, def);
    
    if(pos < 0) 
        titg = (char*) SCR_stracpy((unsigned char*) def);
    else 
        titg = (char*) SCR_stracpy((unsigned char*) KCVAL(kdb, pos));
    
    for(i = 0; i < nb; i++) 
    {
        pos = K_find(kdb, vars[i]);
        if(pos < 0)
            SCR_add_ptr((unsigned char***) &titls, &nbt, (unsigned char*) vars[i]);
        else
        {
            comment = std::string((char*) KCVAL(kdb, pos));
            comment = oem_to_utf8(comment);
            SCR_add_ptr((unsigned char***) &titls, &nbt, (unsigned char*) comment.c_str());
        }

        SCR_add_ptr((unsigned char***) &lecs, &nbl, (unsigned char*) vars[i]);
    }

    SCR_add_ptr((unsigned char***) &titls, &nbt, NULL);
    SCR_add_ptr((unsigned char***) &lecs, &nbl, NULL);

    T_default(tbl, titg, titls, lecs, mode, files, date);

    SCR_free(titg);
    SCR_free_tbl((unsigned char**) titls);
    SCR_free_tbl((unsigned char**) lecs);
}

std::size_t hash_value(const TBL& table)
{
    std::hash<TBL> tbl_hash;
    return tbl_hash(table);
}
