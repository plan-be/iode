#pragma once

#include "abstract_table_model.h"

const static int MAX_NB_DIGITS_TABLE = 16;


/**
 * @brief Intermediate class to handle the number of significant digits 
 *        to display in tables.
 * 
 * @tparam K 
 */
template <class K> class NumericalTableModel: public QIodeTemplateTableModel<K>
{
protected:
    int nb_digits;

public:
    NumericalTableModel(QVector<QString> columnNames, QObject* parent = nullptr, 
        K* kdb_external = nullptr, const int nb_digits = 2) 
        : QIodeTemplateTableModel<K>(columnNames, parent, kdb_external), nb_digits(nb_digits) {}

    int get_nb_digits() const
    {
        return nb_digits;
    }

protected:
    QString valueToString(const double value) const
    {
        return L_ISAN(value) ? QString::number(value, 'g', nb_digits) : NAN_REP;
    }

public slots:
    void set_nb_digits(int nb_digits)
    {
        if(nb_digits < 1)
            return;

        if(nb_digits > MAX_NB_DIGITS_TABLE)
            nb_digits = MAX_NB_DIGITS_TABLE;

        this->nb_digits = nb_digits;
        QIodeAbstractTableModel::resetModel();
    }

    void nb_dec_plus()
    {
        if(nb_digits < MAX_NB_DIGITS_TABLE)
        {
            nb_digits++;
            QIodeAbstractTableModel::resetModel();
        }
    }

    void nb_dec_minus()
    {
        if(nb_digits > 1)
        {
            nb_digits--;
            QIodeAbstractTableModel::resetModel();
        }
    }
};
