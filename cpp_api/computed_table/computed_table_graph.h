#include <map>
#include <vector>
#include "computed_table.h"


struct ComputedTableGraph: public ComputedTable
{
public:
    ComputedTableGraph(Table* ref_table, const std::string& gsample);

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
};
