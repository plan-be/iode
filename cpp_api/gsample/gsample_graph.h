#include "gsample.h"
#include <map>
#include <vector>


class GSampleGraph: public GSampleTable
{
private:
    std::string get_series_name(const int row);

    double get_series_value(const int row, const int column);

    std::vector<double> get_series_values(const int row);


public:
    GSampleGraph(const std::string& ref_table_name, const std::string& gsample);

    std::map<std::string, std::vector<double>> extract_series();

    std::string get_graph_title() const
    {
        return ref_table->getTitle(0);
    }

    EnumGraphGrid get_grid_xaxis() const
    {
        return ref_table->getGridX();
    }

    EnumGraphGrid get_grid_yaxis() const
    {
        return ref_table->getGridY();
    }

    double get_ymin() const
    {
        return (double) ref_table->getYMin();
    }

    double get_ymax() const
    {
        return (double) ref_table->getYMax();
    }

    double get_zmin() const
    {
        return (double) ref_table->getZMin();
    } 

    double get_zmax() const
    {
        return (double) ref_table->getZMax();
    }

    bool logScale() const
    {
        return ref_table->getGraphAxis() == EnumGraphAxis::IG_LOG;
    }

    EnumGraphAlign get_alignement() const
    {
        return ref_table->getGraphAlignement();
    }

    short get_graph_box() const
    {
        return (short) ref_table->getGraphBox();
    }

    short get_graph_shadow() const
    {
        return (short) ref_table->getGraphShadow();
    }
};
