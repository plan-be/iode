#include "computed_table_graph.h"


ComputedTableGraph::ComputedTableGraph(Table* ref_table, const std::string& gsample) 
    : ComputedTable(ref_table, gsample)
{
}

std::vector<Period> ComputedTableGraph::get_series_periods()
{
    std::vector<Period> x;
    x.reserve(get_nb_periods());

    Period start = sample->start_period;
    x.push_back(start);
    
    for(int i=1; i < get_nb_periods(); i++)
        x.push_back(start.shift(i));

    return x;
}

/**
 * @brief Get the name of the corresponding series 
 * 
 * @param row 
 * @param fileop 
 * @return std::string 
 * 
 * @note see function APIGraphLineTitle() (k_graph.c)
 */
std::string ComputedTableGraph::get_series_name(const int row, const int fileop)
{
    std::string name = line_names[row];
    if(files_ops.size() > 1 || files_ops[fileop].cl_opf != COL_NOP)
        name += " " + std::string(COL_ctoa(&files_ops[fileop], 'f', 0, 2));
    return name;
}

// TODO JMP : please check that the comment below and the method implementation are correct.
/**
 * @note 
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
 * 
 * See function T_GraphLine() (k_graph.c)
 */
std::vector<double> ComputedTableGraph::get_series_values(const int row, const int fileop)
{
    int nb_periods = get_nb_periods();
    int nb_files_ops = get_nb_files_ops();

    std::vector<double> y(nb_periods, IODE_NAN);

    COL column;
    int pos;
    int period_pos;
    int col_val = 0;
    int step = ref_table->nb_columns;          // to skip first column of the reference table containing text 
    for(int col=1; col < columns->cl_nb; col+=step)
    {
        column = columns->cl_cols[col];
        pos = find_file_op(column);
        if(pos == fileop)
        {
            period_pos = Period(column.cl_per[0]).difference(sample->start_period);
            y[period_pos] = get_value(row, col_val, true);
        }
        col_val++;
    }

    return y;
}

std::map<std::string, std::vector<double>> ComputedTableGraph::get_series(const int fileop)
{
    if(fileop < 0 || fileop >= files_ops.size())
    {
        std::string msg = "Out of range value for the argument 'fileop':\n";
        msg += "Valid range of possible is [0, " + std::to_string(files_ops.size() - 1) + "]\n";
        msg += "Got value " + std::to_string(fileop);
        throw std::range_error(msg);
    }

    std::string name;
    std::map<std::string, std::vector<double>> series;

    // recompute all values (in case Variables values have changed)
    compute_values();

    // prepare series
    for(int row = 0; row < get_nb_lines(); row++)
    {
        name = get_series_name(row, fileop);
        series[name] = get_series_values(row, fileop);
    }
    return series;
}
