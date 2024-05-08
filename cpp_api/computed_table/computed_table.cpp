#include "computed_table.h"


void ComputedTable::initialize()
{
    std::string error_msg = "Cannot compute table with the generalized sample '" + gsample + "'";

    /* ---- see c_calc.c ----
     *      1. call COL_cc(smpl) to compile the GSAMPLE in a COLS struct, say cls.
     *      2. call COL_resize() to extend COLS according to the number of columns in the TBL 
     *      3. for each TBL line, call: 
     *          COL_clear(cls) to reset the COLS values 
     *          COL_exec(tbl, i, cls) to store in cls the computed values of the cells in line i
     *          COL_text() to generate the value of the KT_STRING cells
     */

    // Compiles a GSAMPLE into a COLS struct and resizes COLS according to the nb of cols in the passed table.
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
            start_per.p_y = per.p_y;
            start_per.p_s = per.p_s;
        }
        if(per.difference(end_per) > 0)
        {
            end_per.p_y = per.p_y;
            end_per.p_s = per.p_s;
        }
    }
    sample = new Sample(start_per, end_per);

    // Returns the number of columns for the computed table + 1.
    int dim = COL_resize(ref_table, columns);
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
            kdb = K_RWS[I_VARIABLES][ref - 1];
            if(kdb == NULL) 
                throw std::invalid_argument("file[" + std::to_string(ref) + "] is not present");
            files.push_back(std::string(kdb->k_nameptr));
        }
    }

    // Get column names
    // QUESTION FOR JMP: Would it be possible to have a table with several cells containing the '#' character ?
    //                   How the GSAMPLE is build in that case ? -> What would be the column names ?
    // TODO JMP: please check the code to get column names below carefully.
    //           For instance, when there is only 1 file involved (current workspace), the returned column name 
    //           does not ends with '[1]' while it is the case in the old GUI. 
    for(int row=0; row < (int) ref_table->nb_lines(); row++)
    {
        TableLine* line = ref_table->get_line(row);

        if(line->get_line_type() != EnumLineType::IT_CELL) 
            continue;
        
        // QUESTION FOR JMP: Can we assume that the cell containing the '#' character will always be the second ?
        TableCell* cell = line->get_cell(1, ref_table->nb_columns());
        if(cell->get_type() == EnumCellType::IT_STRING)
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
        //    case KT_CELL  :
        //        if(cell[1].tc_type != KT_LEC) break;
        //        begin = 0;
        //        if(T_GraphLine(tbl, i, cls, &smpl, x, y, /*c, t,*/ fcls)) w = -1;
        //        break;
        // from which I understand that you assume that the LEC expression WILL be in the second cell
        if(line->get_line_type() == EnumLineType::IT_CELL && 
           line->get_cell(1, ref_table->nb_columns())->get_type() == EnumCellType::IT_LEC)
        {
            name = line->get_cell(0, ref_table->nb_columns())->get_content(false);
            line_names.push_back(name);
            v_line_pos_in_ref_table.push_back(row);

            std::vector<double> row_values(column_names.size(), L_NAN);
            values.push_back(row_values);
        }
    }

    compute_values();
}

ComputedTable::ComputedTable(const std::string& ref_table_name, const std::string& gsample) : gsample(gsample)
{
    ref_table = new Table(ref_table_name, nullptr);
    initialize();
}

ComputedTable::ComputedTable(Table* ref_table, const std::string& gsample) : gsample(gsample)
{
    if(!ref_table)
        throw std::runtime_error("Cannot compute table. Table is null.");
    this->ref_table = new Table(*ref_table);
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
        // of a GSAMPLE (precompiled into a COLS structure). 
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

    std::string formula = lec + " := " + std::format("{:20.8f}", value) + " * " + div_lec;
    CLEC* clec = L_solve(to_char_array(formula), to_char_array(var_name));

    int var_pos = K_find(KV_WS, to_char_array(var_name));
    
    // if the formula is not inversible regarding to the variable var_name, 
    // the Newton-Raphson method is used
    if(clec == NULL || clec->dupendo)
    {
        if(clec != NULL && clec->dupendo) 
            SW_nfree(clec);
        
        formula = lec + " := " + std::format("{:.15f}", value) + " * " + div_lec;
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

    if(!L_ISAN(res))
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
    if(!L_ISAN(value))
        throw IodeException("A NaN value is not accepted to edit a cell of a computed table");

    if(check_if_editable)
        if(!is_editable(line, col))
            throw IodeException("The cell corresponding to the line\n" + line_names[line] + "\n" +
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
    int period_pos = Period(column.cl_per[0]).difference(var_sample.start_period());

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
