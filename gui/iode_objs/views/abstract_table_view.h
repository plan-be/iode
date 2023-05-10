#pragma once

#include <QPrinter>
#include <QTableView>
#include <QLineEdit>
#include <QShortcut>
#include <QStringList>
#include <QModelIndex>
#include <QHeaderView>
#include <QTextDocument>
#include <QModelIndexList>
#include <QAbstractItemModel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "utils.h"
#include "util/print.h"
#include "iode_objs/delegates/base_delegate.h"


class AbstractTableView : public QTableView
{
	Q_OBJECT

protected:
	EnumIodeType iodeType;
	BaseDelegate* delegate;
	QLineEdit* filterLineEdit;

	int sectionEdited;
	QLineEdit* objectNameEdit;

	QPrinter printer;
	QTextDocument document;

	QShortcut* deleteShortcut;
	std::shared_ptr<QString> project_settings_filepath;

protected:
	void keyPressEvent(QKeyEvent* event);

public:
	AbstractTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr);
	~AbstractTableView();

	virtual void setup(std::shared_ptr<QString>& settings_filepath)
	{
		this->project_settings_filepath = settings_filepath;
	}

signals:
	void newObjectInserted();

protected slots:
	void renderForPrinting() 
	{
		document.print(&printer);
	}

public slots:
	void removeObjects();
	void openEditorName(int section);
	virtual void editName();
	void closeNameEditor()
	{
		objectNameEdit->setHidden(true);
		objectNameEdit->setText("");
	}
};


/* NOTE FOR THE DEVELOPERS:
 * Template classes not supported by Q_OBJECT (and then Signals and Slots)
 */


template <class M> class TemplateTableView : public AbstractTableView
{
protected:
	void filter_and_update()
	{
		QString pattern = filterLineEdit->text().trimmed();
		M* table_model = static_cast<M*>(model());
		table_model->filter(pattern);
		update();
	}

public:
	TemplateTableView(EnumIodeType iodeType, BaseDelegate* delegate, QWidget* parent = nullptr) 
		: AbstractTableView(iodeType, delegate, parent) {}

	void setupView(M* model, QLineEdit* filterLineEdit)
	{
		setModel(model);
		setItemDelegate(delegate);
		this->filterLineEdit = filterLineEdit;
	}

	void update()
	{
		M* table_model = static_cast<M*>(model());
		table_model->reset();
	}

	QStringList getSelectedObjectsNames() const
	{
		QStringList names;
		QModelIndexList selection = this->selectionModel()->selectedRows();
		if(selection.size() == 0) 
			return names;

		QAbstractItemModel* model_obj = this->model();
		foreach(const QModelIndex& index, selection) 
			names << model_obj->headerData(index.row(), Qt::Vertical, Qt::DisplayRole).toString();
		return names;
	}
};
