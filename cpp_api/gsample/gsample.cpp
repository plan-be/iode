#include "gsample.h"


GSampleTable::GSampleTable(const std::string& ref_table_name, const std::string& gsample) : gsample(gsample)
{
    IodeExceptionInitialization error("GSampleOutput", "Cannot compile generalized sample " + 
                                       gsample + " according to the input table " + ref_table_name);

    ref_table = new Table(ref_table_name);

    /* ---- see c_calc.c ----
     *      1. call COL_cc(smpl) to compile the GSAMPLE in a COLS struct, say cls.
     *      2. call COL_resize() to extend COLS according to the number of columns in the TBL 
     *      3. for each TBL line, call: 
     *          COL_clear(cls) to reset the COLS values 
     *          COL_exec(tbl, i, cls) to store in cls the computed values of the cells in line i
     *          COL_text() to generate the value of the KT_STRING cells
     */

    // Compiles a GSAMPLE into a COLS struct and resizes COLS according to the nb of cols in the passed table.
    COLS* columns = COL_cc(to_char_array(gsample));
    if(columns == NULL) throw error;

    // Returns the number of columns for the computed table + 1.
    int dim = COL_resize(ref_table->c_table, columns);
    if(dim == 0) throw error;

    // Get filepath of each reference file
    // Note: - equivalent to T_find_files()
    COL column;
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
                throw IodeExceptionInitialization("GSampleOutput", "file[" + std::to_string(ref) + "] is not present");
            files.push_back(std::string(kdb->k_nameptr));
        }
    }

    // Compute minimum sample containing all periods present in the columns.
    // Note: - equivalent to T_prep_smpl()
    column = columns->cl_cols[0];
    Period start_per(column.cl_per);
    Period end_per(column.cl_per);
    for(int col=0; col < columns->cl_nb; col++)
    {
        Period per(columns->cl_cols[col].cl_per);
        if(per.difference(start_per) < 0)
        {
            start_per.c_period->p_y = per.c_period->p_y;
            start_per.c_period->p_s = per.c_period->p_s;
        }
        if(per.difference(end_per) > 0)
        {
            end_per.c_period->p_y = per.c_period->p_y;
            end_per.c_period->p_s = per.c_period->p_s;
        }
    }
    sample = new Sample(start_per, end_per);

    // Get column names
    // QUESTION FOR JMP: Would it be possible to have a table with several cells containing the '#' character ?
    //                   How the GSAMPLE is build in that case ? -> What would be the column names ?
    // TODO JMP: please check the code to get column names below carefully.
    //           For instance, when there is only 1 file involved (current workspace), the returned column name 
    //           does not ends with '[1]' while it is the case in the old GUI. 
    for(int row=0; row < (int) ref_table->nbLines(); row++)
    {
        if(ref_table->getLineType(row) != EnumLineType::IT_CELL) continue;
        // QUESTION FOR JMP: Can we assume that the cell containing the '#' character will always be the second ?
        if(ref_table->getCellType(row, 1) == EnumCellType::IT_STRING)
        {
            std::string content = ref_table->getCellContent(row, 1, false);
            if(content.find('#') != std::string::npos)
            {
                std::string column_name;
                char* c_content = to_char_array(content);
                int nb_files = (int) files.size();
                for(int col=1; col < columns->cl_nb; col+=2)
                {
                    column_name = std::string(COL_text(&columns->cl_cols[col], c_content, nb_files));
                    column_names.push_back(column_name); 
                }
                break;
            }
        }
    }

    // For each table line, compute and store all values
    int res;
    for(int row=0; row < (int) ref_table->nbLines(); row++)
    {
        // QUESTION FOR JMP: Can we always assume that 
        //                   - the first cell will contain the name of the line 
        //                   - the second cell will contain either the '#' character or a LEC expression
        // In k_graph.c, in function T_graph_tbl_1() lines 181 to 185, the code is:
        //    case KT_CELL  :
        //        if(cell[1].tc_type != KT_LEC) break;
        //        begin = 0;
        //        if(T_GraphLine(tbl, i, cls, &smpl, x, y, /*c, t,*/ fcls)) w = -1;
        //        break;
        // from which I understand that you assume that the LEC expression WILL be in the second cell
        if(ref_table->getLineType(row) == EnumLineType::IT_CELL && ref_table->getCellType(row, 1) == EnumCellType::IT_LEC)
        {
            line_names.push_back(ref_table->getCellContent(row, 0, false));
            
            // reset the COLS structure
            COL_clear(columns);
            
            // Calculates the values of all LEC formulas in ONE table line for all columns 
            // of a GSAMPLE (precompiled into a COLS structure). 
            // Stores each column calculated values in cls[i]->cl_res.
            res = COL_exec(ref_table->c_table, row, columns);
            if(res < 0) throw IodeExceptionInitialization("GSampleOutput", "Cannot compute value corresponding to row " + 
                std::to_string(row) + " of table " + ref_table_name);
            
            // store all values
            std::vector<double> line_values;
            for(int col=1; col < columns->cl_nb; col+=2)
                line_values.push_back(columns->cl_cols[col].cl_res);

            values.push_back(line_values);
        }
    }

    COL_free_cols(columns);
}

GSampleTable::~GSampleTable()
{
    delete ref_table;
    delete sample;
}
