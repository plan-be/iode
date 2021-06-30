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
		// https://doc.qt.io/qt-5/qlineedit.html#inputMask-prop
		// 9: character of the Number category required, e.g 0-9. 
		// 0: character of the Number category permitted but not required.
		// A: character of the Letter category required, such as A-Z, a-z.
		this->setInputMask("9999A900");
		QRegExp rx("\\d{4}[YQMW]\\d{3}");
		QValidator* validator = new QRegExpValidator(rx, this);
		this->setValidator(validator);
	}
};
