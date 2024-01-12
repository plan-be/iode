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
    int precision;
    int min_precision;      // 0 if format = 'f', 1 if format = 'g' or 'e'
    char format;

public:
#ifdef _GSAMPLE_
    _NUMERICAL_MODEL_CLASS_NAME_(const int precision = 2, QObject* parent = nullptr): 
        QAbstractTableModel(parent), precision(precision), format('f')
    {}
#else
    _NUMERICAL_MODEL_CLASS_NAME_(QVector<QString> columnNames, QObject* parent = nullptr, 
        const int precision = 2, K* kdb_external = nullptr): 
        IodeTemplateTableModel<K>(columnNames, parent, kdb_external), precision(precision), format('f')
    {
        this->alignment = Qt::AlignRight;
    }
#endif

    QString valueToString(const double value) const
    {
        return L_ISAN(value) ? QString::number(value, format, precision) : NAN_REP;
    }

    int get_precision() const
    {
        return precision;
    }

    void set_precision(const int precision)
    {
        if(precision < min_precision)
            this->precision = min_precision;
        else if(precision > MAX_PRECISION_NUMBERS)
            this->precision = MAX_PRECISION_NUMBERS;
        else
            this->precision = precision;

        this->reset();
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
    }

    void precision_plus()
    {
        if(precision < MAX_PRECISION_NUMBERS)
        {
            precision++;
            this->reset();
        }
    }

    void precision_minus()
    {
        if(precision > min_precision)
        {
            precision--;
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
