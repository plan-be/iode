#pragma once

#include <QObject>

#include "iode_objs/models/scalars_model.h"
#include "iode_objs/delegates/scalars_delegate.h"
#include "iode_objs/new/add_scalar.h"
#include "numerical_view.h"

/* NOTE FOR THE DEVELOPERS:
 * Multiple Inheritance Requires QObject to Be First
 * If you are using multiple inheritance, moc assumes that the first inherited class
 * is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 * https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 * --> That's why the class below does not inherit from NumericalTableView but contains 
 *     a member of type NumericalTableView
 */


class ScalarsView : public IodeAbstractTableView, public TableViewAddObj<ScalarsModel, AddScalarDialog>
{
	Q_OBJECT
	NumericalTableView numeric;

public:
	ScalarsView(QWidget* parent = nullptr) 
		: IodeAbstractTableView(SCALARS, new ScalarsDelegate(parent), parent), TableViewAddObj(this), numeric(false) 
	{
		numeric.setup(this);
	};

protected:
	void contextMenuEvent(QContextMenuEvent* event) override
	{
		numeric.popupContextMenu(event);
		event->accept();
	}

public slots:
	void new_obj()  { openAddDialog(); }
};
