#pragma once

#include <QObject>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QLineEdit>

#include "utils.h"


class IodeSampleEdit : public QLineEdit
{
	Q_OBJECT

public:
	IodeSampleEdit(QWidget* parent = nullptr) : QLineEdit(parent)
	{
		this->setPlaceholderText("yyyyPppp");
#if QT_CONFIG(tooltip)
		this->setToolTip("-yyyy is the year\n"
			"-P is the periodicity (Y=yearly, Q=quaterly, M=monthly, W=weekly)\n"
			"-ppp is the period of the year");
#endif
		// https://doc.qt.io/qt-5/qregexp.html
		QRegularExpression re("[1-9][0-9YQMW]{5,7}");
		QRegularExpressionValidator* validator = new QRegularExpressionValidator(re, this);
		this->setValidator(validator);
	}
};
