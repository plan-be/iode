#pragma once

#include "common.h"
#include "utils/utils.h"
#include "utils/iode_exceptions.h"
#include "time/sample.h"
#include "objects/table.h"

// NOTE: the class below aims to add C++ taste to the C version of gsample
//       see c_calc.c and c_cc.c from the C API

// TODO JMP: I would like to create a C++ class that gives all public methods I need 
//           to generates output tables and graphs provided in the menu Print/Graph of the GUI.
//           I would like you to read carefully the code of the GSampleTable class below and makes 
//           comments/recommandations to get a complete class that does what it is supposed to do.

// TODO ALD: - tranform FIL and COL into C++ classes
//           - replaced FILS and COLS structures by std::vector
//           - rewrite T_prep_cls(), T_prep_smpl() and T_find_files() in C++

/**
 * @brief The present class represents the output table resulting from the compilation 
 *        of a generalized sample given a reference table.
 * 
 */
class GSampleTable
{
protected:
    Table*       ref_table;
    std::string  ref_table_name;
    std::string  gsample;
    Sample*      sample;
    COLS*        columns;
    std::vector<std::string> files;
    std::vector<std::string> column_names;
    std::vector<std::string> line_names;
    // TODO ALD: consider Boost array or Eigen 3 matrix
    std::vector<std::vector<double>> values;

protected:
    void compute_values();

public:
    GSampleTable(const std::string& ref_table_name, const std::string& gsample);
    ~GSampleTable();

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
        return ref_table->getTitle(0);
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

};
