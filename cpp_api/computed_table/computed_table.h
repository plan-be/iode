#pragma once

#include "cpp_api/common.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"


// NOTE: the class below aims to add C++ taste to the C version of gsample
//       see c_calc.c and c_cc.c from the C API

// TODO JMP: I would like to create a C++ class that gives all public methods I need 
//           to generates output tables and graphs provided in the menu Print/Graph of the GUI.
//           I would like you to read carefully the code of the ComputedTable class below and makes 
//           comments/recommandations to get a complete class that does what it is supposed to do.

const static std::string ALLOWED_FORMATS = "HMRCD";

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
struct ComputedTable
{
    Table*       ref_table;
    std::string  gsample;
    Sample*      sample;
    int          nb_decimals;

    int   dim; 
    COLS* columns;
    std::vector<COL>         files_ops;
    std::vector<std::string> files;
    std::vector<std::string> line_names;
    std::vector<std::string> column_names;
    std::vector<int>         v_line_pos_in_ref_table;
    std::vector<int>         v_pos_in_columns_struct;
    // TODO ALD: consider Eigen 3 matrix
    std::vector<std::vector<double>> values;

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
    ComputedTable(Table* ref_table, const std::string& gsample, const int nb_decimals = 2);
    ~ComputedTable();

    int get_nb_decimals() const
    {
        return nb_decimals;
    }

    void set_nb_decimals(const int nb_decimals)
    {
        if (nb_decimals < 0 || nb_decimals > 99)
           throw std::invalid_argument("nb_decimals must be between 0 and 99");
        this->nb_decimals = nb_decimals;
    }

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
        return sample->nb_periods;
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
        return ref_table->get_title();
    }

    TableGraphGrid get_grid_xaxis() const
    {
        return ref_table->get_gridx();
    }

    TableGraphGrid get_grid_yaxis() const
    {
        return ref_table->get_gridy();
    }

    double get_ymin() const
    {
        return (double) ref_table->y_min;
    }

    double get_ymax() const
    {
        return (double) ref_table->y_max;
    }

    double get_zmin() const
    {
        return (double) ref_table->z_min;
    } 

    double get_zmax() const
    {
        return (double) ref_table->z_max;
    }

    bool logScale() const
    {
        return ref_table->get_graph_axis() == TableGraphAxis::TABLE_GRAPH_LOG;
    }

    TableGraphAxis get_graph_axis() const
    {
        return ref_table->get_graph_axis();
    }

    TableTextAlign get_alignement() const
    {
        return ref_table->get_text_alignment();
    }

    short get_graph_box() const
    {
        return (short) ref_table->chart_box;
    }

    short get_graph_shadow() const
    {
        return (short) ref_table->chart_shadow;
    }

    std::string get_line_name(const int line) const
    {
        if(line >= line_names.size()) 
            throw std::runtime_error("Cannot access line " + std::to_string(line) + 
            " since the table has " + std::to_string(line_names.size()) + " lines");
        return line_names.at(line);
    }

    std::string get_column_name(const int col) const
    {
        if(col >= column_names.size()) 
            throw std::runtime_error("Cannot access column " + std::to_string(col) + 
            " since the table has " + std::to_string(column_names.size()) + " columns");
        return column_names.at(col);
    }

    double get_value(const int line, const int col, const bool full_precision = false) const
    {
        if(line >= line_names.size()) 
            throw std::runtime_error("Cannot access line " + std::to_string(line) + 
            " since the table has " + std::to_string(line_names.size()) + " lines");
        if(col >= column_names.size()) 
            throw std::runtime_error("Cannot access column " + std::to_string(col) + 
            " since the table has " + std::to_string(column_names.size()) + " columns");
        double value = values.at(line).at(col);
        if(!full_precision)
        {
            double power = pow(10, nb_decimals);
            value = round(value * power) / power;
        }
        return value;
    }

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
    
    /**
     * @brief Initialize C API parameters for printing.
     * Argument `format` must be in the list:
     *   - 'H' (HTML file)
     *   - 'M' (MIF file)
     *   - 'R' (RTF file)
     *   - 'C' (CSV file)
     *   - 'D' (DUMMY file)
     * 
     * If argument `format` is null (default), the A2M format will be used 
     * to print the output.
     * 
     * If the filename does not contain an extension, it is automatically 
     * added based on the format.
     * 
     * @param destination_file 
     * @param format 
     */
    static void initialize_printing(const std::string& destination_file, const char format = '\0');

    static void finalize_printing()
    {
        W_flush();
        W_close();
    }

    static std::string get_destination_file()
    {
        return std::string(W_filename);
    }

    /**
     * @brief Print the present computed table to a file.
     * Version without argument is dedicated to be called from KDBTables::print_to_file()
     */
    void print_to_file();

    /**
     * @brief Print the present computed table to a file.
     * Argument `format` must be in the list:
     *   - 'H' (HTML file)
     *   - 'M' (MIF file)
     *   - 'R' (RTF file)
     *   - 'C' (CSV file)
     *   - 'D' (DUMMY file)
     * 
     * If argument `format` is null (default), the A2M format will be used 
     * to print the output.
     * 
     * If the filename does not contain an extension, it is automatically 
     * added based on the format.
     * 
     * @param destination_file 
     * @param format 
     */
    void print_to_file(const std::string& destination_file, const char format = '\0');
};
