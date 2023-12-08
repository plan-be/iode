/* NOTE FOR THE DEVELOPPERS:
 * 1. We do not use the preprocessor directive #pragma once so that two 
 *    different versions of the class _NUMERICAL_MODEL_CLASS_NAME_ below can be built.
 *    Depending on if _GSAMPLE_ is defined or not.
 * 
 * 2. Multiple Inheritance Requires QObject to Be First
 *    If you are using multiple inheritance, moc assumes that the first inherited class
 *    is a subclass of QObject. Also, be sure that only the first inherited class is a QObject.
 *    https://doc.qt.io/qt-6/moc.html#multiple-inheritance-requires-qobject-to-be-first
 *    --> That's why we to play with preprocessor directives to inherit from either 
 *        QAbstractTableModel or IodeTemplateTableModel.
 * 
 * 3. Do NOT forget to add safeguards when including the present hearder!
 */

#ifdef _GSAMPLE_
#define GSAMPLE_NUMERICAL_MODEL_HEADER

#include "utils.h"

#define _NUMERICAL_MODEL_CLASS_NAME_ GSampleNumericalTableModel

/**
 * @brief Handles the number of significant digits to display in 
 *        GSample tables.
 */
class _NUMERICAL_MODEL_CLASS_NAME_ : public QAbstractTableModel
#else
#define TEMPLATE_NUMERICAL_MODEL_HEADER

#include "abstract_table_model.h"

#define _NUMERICAL_MODEL_CLASS_NAME_ TemplateNumericalTableModel

/**
 * @brief Handles the number of significant digits to display in 
 *        Scalar and Variable tables.
 * 
 * @tparam K 
 */
template <class K> class _NUMERICAL_MODEL_CLASS_NAME_ : public IodeTemplateTableModel<K>
#endif
{
    int nb_digits;
    char format;

public:
#ifdef _GSAMPLE_
    _NUMERICAL_MODEL_CLASS_NAME_(const int nb_digits = 2, QObject* parent = nullptr): 
        QAbstractTableModel(parent), nb_digits(nb_digits), format('f')
    {}
#else
    _NUMERICAL_MODEL_CLASS_NAME_(QVector<QString> columnNames, QObject* parent = nullptr, 
        const int nb_digits = 2, K* kdb_external = nullptr): 
        IodeTemplateTableModel<K>(columnNames, parent, kdb_external), nb_digits(nb_digits) 
    {}
#endif

    QString valueToString(const double value) const
    {
        return L_ISAN(value) ? QString::number(value, format, nb_digits) : NAN_REP;
    }

    int get_nb_digits() const
    {
        return nb_digits;
    }

    void set_nb_digits(int nb_digits)
    {
        if(nb_digits < 1)
            return;

        if(nb_digits > MAX_NB_DIGITS_TABLE)
            nb_digits = MAX_NB_DIGITS_TABLE;

        this->nb_digits = nb_digits;
        this->reset();
    }

    char get_format() const
    {
        return format;
    }

    void set_format(const char format)
    {
        this->format = format;
    }

    void nb_dec_plus()
    {
        if(nb_digits < MAX_NB_DIGITS_TABLE)
        {
            nb_digits++;
            this->reset();
        }
    }

    void nb_dec_minus()
    {
        if(nb_digits > 1)
        {
            nb_digits--;
            this->reset();
        }
    }
#ifdef _GSAMPLE_
public slots:
    void reset()
	{
		beginResetModel();
		endResetModel();
	}
#endif
};
