#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "utils.h"


class TestsEqsModel : public QAbstractTableModel
{
    QVector<NamedEquation> v_eqs;

public:
    TestsEqsModel(EditAndEstimateEquations* edit_est_eqs, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const
	{
		return I_NB_EQ_TESTS;
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const
	{
		return v_eqs.size();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};

