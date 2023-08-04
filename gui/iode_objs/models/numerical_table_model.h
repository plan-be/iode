#pragma once

#include "utils.h"


const static int MAX_NB_DIGITS_TABLE = 10;

/* NOTE FOR THE DEVELOPERS:
 * Multiple Inheritance Requires QObject to Be First
 * If you are using multiple inheritance, moc assumes that the first inherited class
 * is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 * https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 */

/**
 * @brief Intermediate class to handle the number of significant digits 
 *        to display in tables.
 * 
 * @tparam K 
 */
class NumericalTableModel
{
    int nb_digits;

public:
    NumericalTableModel(const int nb_digits = 2, QObject* parent = nullptr): nb_digits(nb_digits) {}

    int get_nb_digits() const
    {
        return nb_digits;
    }

    QString valueToString(const double value) const
    {
        return L_ISAN(value) ? QString::number(value, 'g', nb_digits) : NAN_REP;
    }

    void set_nb_digits(int nb_digits)
    {
        if(nb_digits < 1)
            return;

        if(nb_digits > MAX_NB_DIGITS_TABLE)
            nb_digits = MAX_NB_DIGITS_TABLE;

        this->nb_digits = nb_digits;
    }

    void nb_dec_plus()
    {
        if(nb_digits < MAX_NB_DIGITS_TABLE)
            nb_digits++;
    }

    void nb_dec_minus()
    {
        if(nb_digits > 1)
            nb_digits--;

    }
};
