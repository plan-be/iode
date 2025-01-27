#pragma once

#include <QDoubleValidator>


class IODEDoubleValidator : public QDoubleValidator
{
public:
	IODEDoubleValidator(double bottom, double top, int decimals, QObject* parent = nullptr) : QDoubleValidator(bottom, top, decimals, parent) {};

	IODEDoubleValidator(QObject* parent = nullptr) : QDoubleValidator(parent) {};

	// Documentation of QDoubleValidator::validate: 
	// By default, the pos parameter is not used by this validator.
	QValidator::State validate(QString& input, int& pos) const override
	{
		if (input == NAN_REP)
			return QValidator::Acceptable;
		else
			return QDoubleValidator::validate(input, pos);
	}
};
