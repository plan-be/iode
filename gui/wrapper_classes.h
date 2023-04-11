#pragma once

#include <QString>
#include <QList>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

#include "./custom_widgets/qfilechooser.h"
#include "./custom_widgets/qsampleedit.h"
#include "utils.h"


/* Base template class to define Wrapper classes.
 *
 * The purpose of the Wrapper classes is to force the developer to implement
 * the function extractAndVerify() which checks that the user entered a correct
 * input in regards to the type of the corresponding field.
 *
 * Note: This Pure Virtual Class
 */

template <class T, class U> class TemplateWrapper : public BaseWrapper
{
protected:
    const QString name;
    T& qfield;
    const EnumItemType type;

public:
    TemplateWrapper(const QString name, T& qfield, const EnumItemType type) : name(name), qfield(qfield), type(type) {};

    T& getQField() { return qfield; }

    virtual U extractAndVerify() = 0;
};


class WrapperComboBox : public TemplateWrapper<QComboBox, int>
{
    const QList<QString> list_values;

public:
    WrapperComboBox(const QString name, QComboBox& qfield, const EnumItemType type, const QList<QString>& list_values, bool editable = false) :
        TemplateWrapper(name, qfield, type), list_values(list_values)
    {
        qfield.addItems(list_values);
        qfield.setEditable(editable);
    };

    QVariant getQValue()
    {
        int index = qfield.currentIndex();
        return QVariant(index);
    }

    void setQValue(const QVariant& qvalue)
    {
        int value = qvalue.toInt();
        qfield.setCurrentIndex(value);
    }

    // QString getTextValue() { return qfield.currentText(); }

    int extractAndVerify()
    {
        if(type == REQUIRED_FIELD)
        {
            QString text = qfield.currentText();
            if (!list_values.contains(text)) 
                throw std::runtime_error("Value " + text.toStdString() + " not allowed in " + name.toStdString());
        }
        int value = qfield.currentIndex();
        return value;
    }
};


class WrapperQLineEdit : public TemplateWrapper<QLineEdit, QString>
{
public:
    WrapperQLineEdit(const QString name, QLineEdit& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        QString value = qfield.text();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qfield.setText(value);
    }

    QString extractAndVerify()
    {
        QString value = qfield.text();

        if (type == REQUIRED_FIELD && value.isEmpty())
            throw std::runtime_error(QString("ERROR in field %1: Empty !").arg(name).toStdString());

        return value;
    }
};


class WrapperIodeNameEdit : public TemplateWrapper<QLineEdit, QString>
{
private:
    EnumIodeType iodeType;

public:
    WrapperIodeNameEdit(const QString name, QLineEdit& qfield, const EnumIodeType iodeType, const EnumItemType type) : TemplateWrapper(name, qfield, type), iodeType(iodeType) {};

    QVariant getQValue()
    {
        QString value = qfield.text();
        check_name(value.toStdString(), iodeType);
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        check_name(value.toStdString(), iodeType);
        qfield.setText(value);
    }

    QString extractAndVerify()
    {
        QString value = qfield.text();

        if (type == REQUIRED_FIELD && value.isEmpty())
            throw std::runtime_error(QString("ERROR in field %1: Empty !").arg(name).toStdString());

        check_name(value.toStdString(), iodeType);

        return value;
    }
};


class WrapperQTextEdit : public TemplateWrapper<QTextEdit, QString>
{
public:
    WrapperQTextEdit(const QString name, QTextEdit& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        QString value = qfield.toPlainText();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qfield.setText(value);
    }

    QString extractAndVerify()
    {
        QString value = qfield.toPlainText();

        if (type == REQUIRED_FIELD && value.isEmpty())
            throw std::runtime_error(QString("ERROR in field %1: Empty !").arg(name).toStdString());

        return value;
    }
};


class WrapperSpinBox : public TemplateWrapper<QSpinBox, int>
{
public:
    WrapperSpinBox(const QString name, QSpinBox& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        double value = qfield.value();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        double value = qvalue.toDouble();
        qfield.setValue(value);
    }

    int extractAndVerify()
    {
        return qfield.value();
    }
};


class WrapperDoubleSpinBox : public TemplateWrapper<QDoubleSpinBox, double>
{
public:
    WrapperDoubleSpinBox(const QString name, QDoubleSpinBox& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        double value = qfield.value();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        double value = qvalue.toDouble();
        qfield.setValue(value);
    }

    double extractAndVerify()
    {
        return qfield.value();
    }
};


class WrapperCheckBox : public TemplateWrapper<QCheckBox, bool>
{
public:
    WrapperCheckBox(const QString name, QCheckBox& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        double value = qfield.isChecked();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        double value = qvalue.toBool();
        qfield.setChecked(value);
    }

    bool extractAndVerify()
    {
        return qfield.isChecked();
    }
};


// ---- Custom Widgets ----

class WrapperFileChooser : public TemplateWrapper<QIodeFileChooser, QString>
{
public:
    WrapperFileChooser(const QString name, QIodeFileChooser& qfield, const EnumItemType type, const EnumIodeFile& fileType, const EnumFileMode& fileMode) :
        TemplateWrapper(name, qfield, type)
    {
        this->qfield.setFileType(fileType);
        this->qfield.setFileMode(fileMode);
    }

    QVariant getQValue()
    {
        QString value = qfield.getFilepath();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qfield.setFilepath(value);
    }

    QString extractAndVerify()
    {
        return qfield.getFilepath();
    }
};


class WrapperSampleEdit : public TemplateWrapper<QIodeSampleEdit, QString>
{
public:
    WrapperSampleEdit(const QString name, QIodeSampleEdit& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        QString value = qfield.text();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qfield.setText(value);
    }

    QString extractAndVerify()
    {
        QString value = qfield.text();

        // throw an error if the string is not valid for an IODE period
        if (type == REQUIRED_FIELD && value.isEmpty()) Period per(value.toStdString());

        return value;
    }
};
