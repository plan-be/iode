#pragma once

#include "utils.h"

inline bool no_precision = false;

/* NOTE FOR THE DEVELOPPERS:
 * Multiple Inheritance Requires QObject to Be First
 * If you are using multiple inheritance, moc assumes that the first inherited class
 * is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 * https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 * --> That's why we do not make the class below to inherit from any Qt class
 */

/**
 * @brief Handles the number of significant digits to display in any numerical tables.
 */
class NumericalTableModel
{
    int  precision;
    int  min_precision;      // 0 if format = 'f', 1 if format = 'g' or 'e'
    char format;

public:
    NumericalTableModel(const int precision = 2, const char format = 'f'): precision(precision), format(format) {}

    QString valueToString(const double value) const
    {
        // QLocale::FloatingPointShortest -> the conversion algorithm will try to find the shortest accurate 
        // representation for the given number. 'Accurate' means that you get the exact same number back from 
        // an inverse conversion on the generated string representation. In particular, trailing zeros are omitted.
        // see https://doc.qt.io/qt-6/qlocale.html#FloatingPointPrecisionOption-enum 
        int precision_ = no_precision ? QLocale::FloatingPointShortest : precision;
        return IODE_IS_A_NUMBER(value) ? QString::number(value, format, precision_) : NAN_REP;
    }

    int get_precision() const
    {
        return precision;
    }

    virtual void set_precision(const int precision)
    {
        if(precision < min_precision)
            this->precision = min_precision;
        else if(precision > MAX_PRECISION_NUMBERS)
            this->precision = MAX_PRECISION_NUMBERS;
        else
            this->precision = precision;

        updateDisplayValues();
    }

    int get_min_precision() const
    {
        return min_precision;
    }

    char get_format() const
    {
        return format;
    }

    void set_format(const char format)
    {
        this->format = format;
        min_precision = (format == 'f') ? 0 : 1;
        updateDisplayValues();
    }

    void precision_plus()
    {
        if(precision < MAX_PRECISION_NUMBERS)
        {
            precision++;
            updateDisplayValues();
        }
    }

    void precision_minus()
    {
        if(precision > min_precision)
        {
            precision--;
            updateDisplayValues();
        }
    }

    // must call the reset() method of the derived class
    virtual void updateDisplayValues() = 0;
};
