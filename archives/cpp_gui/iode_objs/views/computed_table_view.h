#pragma once

#include <QTableView>

#include "numerical_view.h"

/* NOTE FOR THE DEVELOPERS:
 * Multiple Inheritance Requires QObject to Be First
 * If you are using multiple inheritance, moc assumes that the first inherited class
 * is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 * https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 * --> That's why the class below does not inherit from NumericalTableView but contains 
 *     a member of type NumericalTableView
 */


class ComputedTableView: public QTableView
{
    Q_OBJECT
    NumericalTableView numeric;

public:
    ComputedTableView(QWidget *parent = nullptr): QTableView(parent), numeric(false)
    {
        numeric.setup(this);
    }

protected:
	void contextMenuEvent(QContextMenuEvent* event) override
	{
		numeric.popupContextMenu(event);
		event->accept();
	}
};
