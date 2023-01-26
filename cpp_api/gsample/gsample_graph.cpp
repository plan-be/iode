#include "gsample_graph.h"


GSampleGraph::GSampleGraph(const std::string& ref_table_name, const std::string& gsample) :
    GSampleTable(ref_table_name, gsample)
{
}

std::string GSampleGraph::get_series_name(const int row)
{
    return ref_table->getCellContent(row, 0, false);
}

double GSampleGraph::get_series_value(const int row, const int column)
{
    return 0.0;
}

std::vector<double> GSampleGraph::get_series_values(const int row)
{
    std::vector<double> values;
    return values;
}

std::map<std::string, std::vector<double>> GSampleGraph::extract_series()
{
    std::map<std::string, std::vector<double>> series;
    /*
    COL_clear(columns);
    int row = v_table_values_pos[line_pos];

    if(COL_exec(ref_table.c_table, row, columns) < 0)
        thow IodeExceptionFunction("Cannot get value of generalized table for line " + )

    for(k = 0 ; k < fcls->cl_nb ; k++) {
        T_GraphLineTitle(line, fcls, k);

        for(j = 0 ; j < smpl->s_nb ; j++) y[j] = L_NAN;
        for(j = 1; j < cls->cl_nb; j += 2) {
            cl = cls->cl_cols + j;
            if(T_find_opf(fcls, cl) != k) continue;
            dt = PER_diff_per(cl->cl_per, &(smpl->s_p1));
            y[dt] = cl->cl_res;
        }

        T_GraphTimeData(smpl, y);
    }
    */
   return series;
}
