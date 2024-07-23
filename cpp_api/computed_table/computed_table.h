#pragma once

#include "cpp_api/common.h"
#include "cpp_api/utils/utils.h"
#include "cpp_api/utils/iode_exceptions.h"
#include "cpp_api/time/sample.h"
#include "cpp_api/objects/table.h"
#include "cpp_api/KDB/kdb_tables.h"
#include "cpp_api/KDB/kdb_variables.h"


// NOTE: the class below aims to add C++ taste to the C version of gsample
//       see c_calc.c and c_cc.c from the C API

// TODO JMP: I would like to create a C++ class that gives all public methods I need 
//           to generates output tables and graphs provided in the menu Print/Graph of the GUI.
//           I would like you to read carefully the code of the ComputedTable class below and makes 
//           comments/recommandations to get a complete class that does what it is supposed to do.

// TODO ALD: - tranform FIL and COL into C++ classes
//           - replaced FILS and COLS structures by std::vector
//           - rewrite T_prep_cls(), T_prep_smpl() and T_find_files() in C++

/**
 * @brief The present class represents the output table resulting from the compilation 
 *        of a generalized sample given a reference table.
 * 
 * Given a ComputedTable table defined by 
 * - M lines,
 * - N observations (nb periods of the sample)
 * - 2 combinations file_1 (op) file_2      -> fop0 and fop1
 * - 2 combinations period_1 (op) period_2  -> pop0 and pop1 
 * the computed ComputedTable table is structured as follow:
 * 
 *          |           obs0             |  ...  |           obsN             |
 *          |    pop 0    |     pop1     |  ...  |    pop 0    |     pop1     |
 *          | fop0 | fop1 | fop 0 | fop1 |  ...  | fop0 | fop1 | fop 0 | fop1 |
 * ---------------------------------------------------------------------------|
 * line 0   |      |      |       |      |  ...  |      |      |       |      |
 * line ... |      |      |       |      |  ...  |      |      |       |      |
 * line M   |      |      |       |      |  ...  |      |      |       |      |
 */
class ComputedTable
{
protected:
    Table*       ref_table;
    std::string  gsample;
    Sample*      sample;
    COLS*        columns;
    std::vector<COL>         files_ops;
    std::vector<std::string> files;
    std::vector<std::string> line_names;
    std::vector<std::string> column_names;
    std::vector<int>         v_line_pos_in_ref_table;
    std::vector<int>         v_pos_in_columns_struct;
    // TODO ALD: consider Eigen 3 matrix
    std::vector<std::vector<double>> values;

protected:
    void compute_values();

    /**
     * @brief Tries to find the position in files_ops vector of the 
     *        operation on files contained in the passed COL struct.
     * 
     * @param col input COL struct
     * @return int position in the files_ops vector if operation on files 
     *             has been found, -1 otherwise
     * 
     * @note equivalent of the funtion T_find_opf() (k_graph.c)
     */
    int find_file_op(const COL& col);

private:
    /**
     * @warning to be called only by the constructors 
     */
    void initialize();

    /**
     * @brief 
     * 
     * @param lec 
     * @param div_lec 
     * @param var_name 
     * @param value 
     * @param var_pos 
     * @param period_pos 
     * @return true 
     * @return false 
     * 
     * @note same as VT_calc() function from o_vt.c from the old GUI
     */
    bool propagate_new_value(const std::string& lec, const std::string& div_lec, 
            const std::string& var_name, const double value, const int period_pos);

public:
    ComputedTable(const std::string& ref_table_name, const std::string& gsample);
    ComputedTable(Table* ref_table, const std::string& gsample);
    ~ComputedTable();

    int get_nb_lines() const
    {
        return (int) line_names.size();
    } 

    int get_nb_columns() const
    {
        return (int) column_names.size();
    }

    int get_nb_files() const
    {
        return (int) files.size();
    }

    int get_nb_periods() const
    {
        return sample->nb_periods();
    }

    int get_nb_files_ops() const
    {
        return (int) files_ops.size();
    }

    Sample* get_sample() const
    {
        return sample;
    }

    std::vector<std::string> get_list_files() const
    {
        return files;
    }

    std::string get_title() const
    {
        return ref_table->get_title(0);
    }

    std::string get_line_name(const int line) const
    {
        if(line >= line_names.size()) 
            throw IodeExceptionFunction("Cannot access line " + std::to_string(line) + 
            " since the table has " + std::to_string(line_names.size()) + " lines.");
        return line_names.at(line);
    }

    std::string get_column_name(const int col) const
    {
        if(col >= column_names.size()) 
            throw IodeExceptionFunction("Cannot access column " + std::to_string(col) + 
            " since the table has " + std::to_string(column_names.size()) + " columns.");
        return column_names.at(col);
    }

    double get_value(const int line, const int col, const int nb_decimals=2) const
    {
        if(line >= line_names.size()) 
            throw IodeExceptionFunction("Cannot access line " + std::to_string(line) + 
            " since the table has " + std::to_string(line_names.size()) + " lines.");
        if(col >= column_names.size()) 
            throw IodeExceptionFunction("Cannot access column " + std::to_string(col) + 
            " since the table has " + std::to_string(column_names.size()) + " columns.");
        double value = values.at(line).at(col);
        if(nb_decimals >= 0)
        {
            double power = pow(10, nb_decimals);
            value = round(value * power) / power;
        }
        return value;
    }

    /**
     * @brief Check if a cell is editable
     * 
     * @param line 
     * @param col 
     * @return true 
     * @return false 
     * 
     * @note see https://iode.plan.be/doku.php?id=edit_tables for the rules 
     *       and the function VT_edit() from o_vt.c from the old GUI
     */
    bool is_editable(const int line, const int col);

    /**
     * @brief Update the value of a given cell
     * 
     * @param line 
     * @param col 
     * @param value 
     * 
     * @note see the VT_edit() function from o_vt.c from the old GUI
     */
    void set_value(const int line, const int col, const double value, bool check_if_editable = true);
};
