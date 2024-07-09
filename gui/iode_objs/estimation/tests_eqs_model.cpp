#include "tests_eqs_model.h"


TestsEqsModel::TestsEqsModel(EditAndEstimateEquations* edit_est_eqs, QObject* parent) : QAbstractTableModel(parent)
{
    KDBEquations* kdb_eqs = edit_est_eqs->get_equations();

    // WARNING: Be aware that the Equations database belonging to the EditAndEstimateEquations instance 
    //          may contain more equations than the ones contained in the currently evaluated block.
    //          The method get_list_equations() gives the list of currently estimated equations.
    for(const std::string& eq_name: edit_est_eqs->get_list_equations())
        v_eqs.append(kdb_eqs->get(eq_name));
}

QVariant TestsEqsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal)
        return QString::fromStdString(v_eqs[section].get_endo());
    else
        return QString::fromStdString(vEquationTests[section]);
}

QVariant TestsEqsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignRight);

    if (role == Qt::DisplayRole)
    {
        Equation eq = v_eqs[index.column()];
        float value = eq.get_test((IodeEquationTest) index.row());
        return QString::number(value, 'g', 8);
    }
    
    return QVariant();
}
