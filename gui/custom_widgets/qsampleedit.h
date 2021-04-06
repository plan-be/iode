#pragma once
#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>
#include <QLineEdit>

#include <string>

#include "../utils.h"


class QSampleEdit : public QLineEdit
{
	Q_OBJECT

public:
	QSampleEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{
		this->setPlaceholderText("yyyyPppp");
#if QT_CONFIG(tooltip)
		this->setToolTip("-yyyy is the year\n"
		                 "-P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
		                 "-ppp is the period of the year");
#endif
		this->setInputMask("9999AAAA");
		QRegExp rx("\\d{4}[YQMW]\\d{3}");
		QValidator* validator = new QRegExpValidator(rx, this);
		this->setValidator(validator);
	}
};
