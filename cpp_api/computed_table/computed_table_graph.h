#include <map>
#include <vector>
#include "computed_table.h"


class ComputedTableGraph: public ComputedTable
{
    int nb_decimals;

public:
    ComputedTableGraph(const std::string& ref_table_name, const std::string& gsample, 
        const int nb_decimals = 8);
    ComputedTableGraph(Table* ref_table, const std::string& gsample, 
        const int nb_decimals = 8);

    std::vector<Period> get_series_periods();

    std::string get_series_name(const int row, const int fileop);

    std::vector<double> get_series_values(const int row, const int fileop);

    /**
     * @brief Get the series to plot
     * 
     * @return std::map<std::string, std::vector<double>> series
     * 
     * @note Equivalent to T_GraphLine() (k_graph.c)
     */
    std::map<std::string, std::vector<double>> get_series(const int fileop);

    // TODO JMP : Can you check that the method below returns the number 
    //            of plotted series (IODE GUI -> Menu Print/Graph -> Graph from Tables)
    int get_nb_series() const
    {
        return get_nb_lines() * get_nb_files_ops();
    }

    std::string get_graph_title() const
    {
        return ref_table->get_title(0);
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
        return (double) ref_table->t_ymin;
    }

    double get_ymax() const
    {
        return (double) ref_table->t_ymax;
    }

    double get_zmin() const
    {
        return (double) ref_table->t_zmin;
    } 

    double get_zmax() const
    {
        return (double) ref_table->t_zmax;
    }

    bool logScale() const
    {
        return ref_table->get_graph_axis() == TableGraphAxis::TABLE_GRAPH_LOG;
    }

    TableGraphAlign get_alignement() const
    {
        return ref_table->get_graph_alignment();
    }

    short get_graph_box() const
    {
        return (short) ref_table->t_box;
    }

    short get_graph_shadow() const
    {
        return (short) ref_table->t_shadow;
    }
};
