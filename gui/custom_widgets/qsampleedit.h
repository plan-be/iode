#pragma once
#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>
#include <QLineEdit>

#include <string>

#include "../utils.h"


class QIodeSampleEdit : public QLineEdit
{
	Q_OBJECT

public:
	QIodeSampleEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{
		this->setPlaceholderText("yyyyPppp");
#if QT_CONFIG(tooltip)
		this->setToolTip("-yyyy is the year\n"
		                 "-P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
		                 "-ppp is the period of the year");
#endif
		// https://doc.qt.io/qt-5/qregexp.html
		QRegExp rx("\\d{4}[YQMW]\\d{1,3}");
		QValidator* validator = new QRegExpValidator(rx, this);
		this->setValidator(validator);
	}
};
