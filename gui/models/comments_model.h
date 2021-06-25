#pragma once

#include <QObject>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QAbstractTableModel>

#include "../utils.h"
#include "../bridge.h"


class CommentsModel : public QAbstractTableModel
{
	Q_OBJECT

	int nbColumns = 2;

public:
	CommentsModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const 
	{ 
		return get_nb_elements_WS(Comments); 
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const 
	{ 
		return nbColumns; 
	}
	
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
	void reset();
};
