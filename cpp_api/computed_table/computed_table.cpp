#include "computed_table.h"


void ComputedTable::initialize()
{
    std::string error_msg = "Cannot compute table with the generalized sample '" + gsample + "'";

    /* ---- see c_calc.c ----
     *      1. call COL_cc(smpl) to compile the GSample in a COLS struct, say cls.
     *      2. call COL_resize() to extend COLS according to the number of columns in the TBL 
     *      3. for each TBL line, call: 
     *          COL_clear(cls) to reset the COLS values 
     *          COL_exec(tbl, i, cls) to store in cls the computed values of the cells in line i
     *          COL_text() to generate the value of the TABLE_CELL_STRING cells
     */

    // Compiles a GSample into a COLS struct and resizes COLS according to the nb of cols in the passed table.
    columns = COL_cc(to_char_array(gsample));
    if(columns == NULL) 
        throw std::invalid_argument(error_msg);

    // Compute 
    // - minimum sample containing all periods present in the columns.
    // - the list of unique file_1 (op) file_2 combinations
    // Note: - equivalent to T_prep_smpl()
    COL column = columns->cl_cols[0];
    Period start_per(column.cl_per[0]);
    Period end_per(column.cl_per[0]);
    int pos;
    for(int col=0; col < columns->cl_nb; col++)
    {
        column = columns->cl_cols[col];
        pos = find_file_op(column);
        if(pos < 0)
            files_ops.push_back(column);

        Period per(columns->cl_cols[col].cl_per[0]);
        if(per.difference(start_per) < 0)
        {
            start_per.year = per.year;
            start_per.step = per.step;
        }
        if(per.difference(end_per) > 0)
        {
            end_per.year = per.year;
            end_per.step = per.step;
        }
    }
    sample = new Sample(start_per, end_per);

    // Returns the number of columns for the computed table + 1.
    dim = COL_resize(ref_table, columns);
    if(dim == 0) 
        throw std::runtime_error(error_msg);

    // Get filepath of each reference file
    // Note: - equivalent to T_find_files()
    std::bitset<K_MAX_FREF + 1> files_usage;
    for(int col=0; col < columns->cl_nb; col++) 
    {
        column = columns->cl_cols[col];
        files_usage[column.cl_fnb[0]] = 1;
        files_usage[column.cl_fnb[1]] = 1;
    }

    KDB* kdb;
    for(int ref=1; ref < K_MAX_FREF + 1; ref++) 
    {
        if(files_usage.test(ref))
        {
            kdb = K_RWS[VARIABLES][ref - 1];
            if(kdb == NULL) 
                throw std::invalid_argument("file[" + std::to_string(ref) + "] is not present");
            files.push_back(std::string(kdb->k_nameptr));
        }
    }

    // Get column names
    // QUESTION FOR JMP: Would it be possible to have a table with several cells containing the '#' character ?
    //                   How the GSample is build in that case ? -> What would be the column names ?
    // TODO JMP: please check the code to get column names below carefully.
    //           For instance, when there is only 1 file involved (current workspace), the returned column name 
    //           does not ends with '[1]' while it is the case in the old GUI. 
    for(int row=0; row < (int) ref_table->nb_lines(); row++)
    {
        TableLine* line = ref_table->get_line(row);

        if(line->get_line_type() != TableLineType::TABLE_LINE_CELL) 
            continue;
        
        // QUESTION FOR JMP: Can we assume that the cell containing the '#' character will always be the second ?
        TableCell* cell = line->get_cell(1, ref_table->nb_columns());
        if(cell->get_type() == TableCellType::TABLE_CELL_STRING)
        {
            std::string content = cell->get_content(false);
            if(content.find('#') != std::string::npos)
            {
                std::string column_name;
                char* c_content = to_char_array(content);
                int nb_files = (int) files.size();
                int step = ref_table->nb_columns();          // to skip first column of the reference table containing text 
                for(int col=1; col < columns->cl_nb; col+=step)
                {
                    column_name = std::string(COL_text(&columns->cl_cols[col], c_content, nb_files));
                    column_names.push_back(column_name); 
                    v_pos_in_columns_struct.push_back(col);
                }
                break;
            }
        }
    }

    // For each table line, get line name + add a NaN value for each column
    std::string name;
    for(int row=0; row < (int) ref_table->nb_lines(); row++)
    {
        TableLine* line = ref_table->get_line(row);

        // QUESTION FOR JMP: Can we always assume that 
        //                   - the first cell will contain the name of the line ?
        //                   - the second cell will contain either the '#' character or a LEC expression ?
        // In k_graph.c, in function T_graph_tbl_1() lines 181 to 185, the code is:
        //    case TABLE_LINE_CELL  :
        //        if(cell[1].tc_type != TABLE_CELL_LEC) break;
        //        begin = 0;
        //        if(T_GraphLine(tbl, i, cls, &smpl, x, y, /*c, t,*/ fcls)) w = -1;
        //        break;
        // from which I understand that you assume that the LEC expression WILL be in the second cell
        if(line->get_line_type() == TableLineType::TABLE_LINE_CELL && 
           line->get_cell(1, ref_table->nb_columns())->get_type() == TableCellType::TABLE_CELL_LEC)
        {
            name = line->get_cell(0, ref_table->nb_columns())->get_content(false);
            line_names.push_back(name);
            v_line_pos_in_ref_table.push_back(row);

            std::vector<double> row_values(column_names.size(), IODE_NAN);
            values.push_back(row_values);
        }
    }

    compute_values();
}

ComputedTable::ComputedTable(Table* ref_table, const std::string& gsample, const int nb_decimals) 
    : gsample(gsample)
{
    if(!ref_table)
        throw std::runtime_error("Cannot compute table. Table is null.");
    this->ref_table = new Table(*ref_table);
    set_nb_decimals(nb_decimals);
    initialize();
}

ComputedTable::~ComputedTable()
{
    COL_free_cols(columns);
    delete sample;
    delete ref_table;
}

int ComputedTable::find_file_op(const COL& col)
{
    if(files_ops.size() == 0)
        return -1;

    COL fileop;
    for(int i = 0; i < files_ops.size(); i++)
    {
        fileop = files_ops[i];
        if(col.cl_opf    == fileop.cl_opf &&
            col.cl_fnb[0] == fileop.cl_fnb[0] &&
            col.cl_fnb[1] == fileop.cl_fnb[1])
            return i;
    }

    return -1;
}

void ComputedTable::compute_values()
{
    // For each ref_table line containing a LEC cell, compute and store all values using 
    // the generalized sample
    int res;
    int line;
    int pos;
    for(int row = 0; row < v_line_pos_in_ref_table.size(); row++)
    {
        // resets the values in the COLS
        COL_clear(columns);
        
        // Calculates the values of all LEC formulas in ONE table line for all columns 
        // of a GSample (precompiled into a COLS structure). 
        // Stores each column calculated values in cls[i]->cl_res.
        line = v_line_pos_in_ref_table[row];
        res = COL_exec(ref_table, line, columns);
        if(res < 0) 
            throw std::runtime_error("Cannot compute values corresponding to row '" + get_line_name(row) + "'");
        
        // store all values
        for(int col = 0; col < v_pos_in_columns_struct.size(); col++)
        {
            pos = v_pos_in_columns_struct[col];
            values[row][col] = columns->cl_cols[pos].cl_res;
        }
    }
}

bool ComputedTable::is_editable(const int line, const int col)
{
    // RULE 1: A cell cannot be updated if the corresponding column (COL object)
    //         - contains on operation on periods or files
    //         - does not refer to the current workspace
    int col_pos = v_pos_in_columns_struct[col];
    COL column = columns->cl_cols[col_pos];
    if(column.cl_opy != COL_NOP || column.cl_opf != COL_NOP) 
        return false;
    if(column.cl_fnb[0] != 1) 
        return false;

    // RULE 2: A cell cannot be updated if the corresponding LEC expression from the 
    //         reference table starts with 0+
    int line_ref_pos = v_line_pos_in_ref_table.at(line);
    TableLine* line_ref = ref_table->get_line(line_ref_pos);
    TableCell* cell_ref = line_ref->get_cell(1, ref_table->nb_columns());
    std::string lec = cell_ref->get_content(false);
    if(lec.substr(0, 2) == "0+")
        return false;

    // RULE 3: A cell cannot be updated if the corresponding LEC expression from the 
    //         reference table does not refer to at least one variable
    std::vector<std::string> variables = cell_ref->get_variables_from_lec();
    if(variables.size() == 0)
        return false;

    return true;
}

// TODO : use a KDBVariable object instead of K_find and KV_set.
//        For the moment, there is a memory problem when the function ends and
//        thus when the KDBVariable object is destroyed. 
//        -> problem linked to the compiler option /Zp1
bool ComputedTable::propagate_new_value(const std::string& lec, const std::string& div_lec, 
        const std::string& var_name, const double value, const int period_pos)
{
    double res;
    std::ostringstream oss;

    // Set the width to 20 and precision to 8
    oss << std::setw(20) << std::fixed << std::setprecision(8) << value;

    std::string formula = lec + " := " + oss.str() + " * " + div_lec;
    CLEC* clec = L_solve(to_char_array(formula), to_char_array(var_name));

    int var_pos = K_find(KV_WS, to_char_array(var_name));
    
    // if the formula is not inversible regarding to the variable var_name, 
    // the Newton-Raphson method is used
    if(clec == NULL || clec->dupendo)
    {
        if(clec != NULL && clec->dupendo) 
            SW_nfree(clec);
        
        oss.clear();
        oss << std::fixed << std::setprecision(15) << value;

        formula = lec + " := " + oss.str() + " * " + div_lec;
        clec = L_cc(to_char_array(formula));
        if(clec == NULL)
            return false;

        L_link(KV_WS, KS_WS, clec);
        // Newton-Raphson method
        res = L_zero(KV_WS, KS_WS, clec, period_pos, var_pos, var_pos);
    }
    else
    {
        // inverse formula
        L_link(KV_WS, KS_WS, clec);
        res = L_exec(KV_WS, KS_WS, clec, period_pos);
    }

    SW_nfree(clec);

    if(!IODE_IS_A_NUMBER(res))
        return false;

    // update the variable var_name in the database
    KV_set(KV_WS, var_pos, period_pos, 0, res);

    return true;
}

// TODO : use a KDBVariable object instead of KSMPL(KV_WS).
//        For the moment, there is a memory problem when the function ends and
//        thus when the KDBVariable object is destroyed. 
//        -> problem linked to the compiler option /Zp1
void ComputedTable::set_value(const int line, const int col, const double value, bool check_if_editable)
{
    // Reject NaN value
    if(!IODE_IS_A_NUMBER(value))
        throw std::invalid_argument("An IODE NA (NaN) value is not accepted to edit a cell of a computed table");

    if(check_if_editable)
        if(!is_editable(line, col))
            throw std::runtime_error("The cell corresponding to the line\n" + line_names[line] + "\n" +
                                     "and column\n" + column_names[col] + "\ncannot be edited");

    int line_ref_pos = v_line_pos_in_ref_table.at(line);
    int col_pos = v_pos_in_columns_struct[col];

    TableLine* line_ref = ref_table->get_line(line_ref_pos);
    TableCell* cell_ref = line_ref->get_cell(1, ref_table->nb_columns());

    // RULE 4: Only the first variable found in the LEC expression is updated
    // see https://iode.plan.be/doku.php?id=edit_tables for the rules
    std::string var_to_update = cell_ref->get_variables_from_lec().at(0);

    // get period position 
    COL column = columns->cl_cols[col_pos];
    Sample var_sample(*KSMPL(KV_WS));
    int period_pos = Period(column.cl_per[0]).difference(var_sample.start_period);

    // get lec
    std::string lec = cell_ref->get_content(false);

    // get divider
    TableLine* line_divider = ref_table->get_divider_line();
    TableCell* cell_divider = line_divider->get_cell(1, ref_table->nb_columns());
    std::string div_lec = cell_divider->get_content(false);
    if(div_lec.empty())
        div_lec = "1";

    // same as VT_calc() from o_vt.c from the old GUI
    bool success = propagate_new_value(lec, div_lec, var_to_update, value, period_pos);
    if(!success)
        throw std::runtime_error("The cell corresponding to the line\n" + line_names[line] + "\n" +
            "and column\n" + column_names[col] + "\ncannot be edited.\n\n" + 
            "Cannot calculate the new value for the variable " + var_to_update + "\n" + 
            "LEC expression: " + lec);

   // recompute all values of the ComputedTable table
   compute_values();
}

void ComputedTable::initialize_printing(const std::string& destination_file, const char format)
{
    int res;

    // set output file
    // output_file = "<filepath> <format>"
    // if windows printer -> output_file = ""
    // see C_FilePrintFile() from sb_file.c from the old GUI
    if(destination_file.empty())
        throw std::invalid_argument("Cannot initialize printing. Destination file is empty.");

    if(format == '\0')
        res = B_PrintDest(to_char_array(destination_file));
    else
    {
        char upper_format = toupper(format);
        if (ALLOWED_FORMATS.find(upper_format) == std::string::npos)
            throw std::invalid_argument(std::string("Cannot initialize printing. Invalid format.\n") + 
                "Possible formats are: " + ALLOWED_FORMATS);
        std::string arg = destination_file + " " + upper_format;
        res = B_PrintDest(arg.data());
    }   

    if(res < 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty()) 
            throw std::invalid_argument(std::string("Cannot initialize printing.\n") + 
                    "Invalid value for 'destination_file' or 'format' argument.\n" + last_error);
    }
}

void ComputedTable::print_to_file()
{
    int res;

    // set number of decimals to print
    std::string str_nb_decimals = std::to_string(nb_decimals);
    res = B_PrintNbDec(str_nb_decimals.data());
    if(res < 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            throw std::invalid_argument(std::string("Cannot initialize printing.\n") + 
                "Invalid value for 'nb_decimals' argument.\n" + last_error);
    }

    // set language
    std::string language = ref_table->get_language();
    if(language.empty())
        throw std::invalid_argument("Cannot initialize printing. Language is empty.");

    char tlang[2];
    tlang[0] = language[0];
    tlang[1] = 0;
    
    res = B_PrintLang(tlang);
    if(res < 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            throw std::invalid_argument(std::string("Cannot initialize printing.\n") + 
                    "Invalid value for 'language' argument.\n" + last_error);
    }

    // Anciennement
    // B_PrintRtfTopic(T_get_title(tbl));
    // Nouveau JMP 18/04/2022
    W_printf( ".topic %d %d %s\n", KT_CUR_TOPIC++, KT_CUR_LEVEL, T_get_title(ref_table));
    //if(W_type == A2M_DESTRTF && W_rtfhelp) W_printf(".par1 tit_%d\n%s\n\n", KT_CUR_LEVEL, T_get_title(tbl));
    
    res = T_begin_tbl(dim, columns);
    if(res != 0) 
        throw std::runtime_error("Couldn't print table. Couldn't print the table header.");

    W_printf(".ttitle %s\n", T_get_title(ref_table));  /* JMP 27-02-98 */

    TableLine* line;
    bool first_title = true;
    for(int i = 0; i < ref_table->nb_lines(); i++) 
    {
        line = ref_table->get_line(i);

        switch(line->get_line_type()) {
            case TABLE_LINE_SEP:
                W_printf(".tl\n");
                break;
            case TABLE_LINE_TITLE:
                // 1st title has already been printed by W_printf(".ttitle %s\n", ...) above
                if(first_title)
                {
                    first_title = false;
                    break;
                }
                T_print_cell((TCELL *) line->tl_val, NULL, dim);
                W_printf("\n");
                break;
            case TABLE_LINE_DATE  :
                T_print_date(dim);
                break;
            case TABLE_LINE_MODE  :
                T_print_mode(columns, dim);
                break;
            case TABLE_LINE_FILES :
                T_print_files(columns, dim);
                break;
            case TABLE_LINE_CELL  :
                res = T_print_line(ref_table, i, columns);
                if(res != 0)
                    throw std::runtime_error("Couldn't print table. Couldn't print line " + std::to_string(i));
        }
    }
    T_end_tbl();
}

void ComputedTable::print_to_file(const std::string& destination_file, const char format)
{
    initialize_printing(destination_file, format);
    print_to_file();
    finalize_printing();
}
