#pragma once

#include <QList>
#include <QString>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QFontComboBox>
#include <QPlainTextEdit>
#include <QDoubleSpinBox>

#include "./util/widgets/file_chooser.h"
#include "./util/widgets/sample_edit.h"
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


class WrapperRadioButton : public TemplateWrapper<QRadioButton, bool>
{
public:
    WrapperRadioButton(const QString name, QRadioButton& qfield) :
        TemplateWrapper(name, qfield, EnumItemType::REQUIRED_FIELD) {}

    QVariant getQValue()
    {
        return QVariant(qfield.isChecked());
    }

    void setQValue(const QVariant& qvalue)
    {
        bool value = qvalue.toBool();
        qfield.setChecked(value);
    }

    bool extractAndVerify()
    {
        return qfield.isChecked();
    }
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
                throw std::invalid_argument("Value " + text.toStdString() + " not allowed in " + name.toStdString());
        }
        int value = qfield.currentIndex();
        return value;
    }
};


class WrapperQFontComboBox : public TemplateWrapper<QFontComboBox, int>
{
public:
    WrapperQFontComboBox(const QString name, QFontComboBox& qfield, const EnumItemType type) :
        TemplateWrapper(name, qfield, type) {};

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
            throw std::invalid_argument(QString("ERROR in field %1: Empty !").arg(name).toStdString());

        return value;
    }
};


class WrapperIodeNameEdit : public TemplateWrapper<QLineEdit, QString>
{
private:
    IodeType iodeType;

public:
    WrapperIodeNameEdit(const QString name, QLineEdit& qfield, const IodeType iodeType, const EnumItemType type) : TemplateWrapper(name, qfield, type), iodeType(iodeType) {};

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
            throw std::invalid_argument(QString("ERROR in field %1: Empty !").arg(name).toStdString());

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
            throw std::invalid_argument(QString("ERROR in field %1: Empty !").arg(name).toStdString());

        return value;
    }
};


class WrapperQPlainTextEdit : public TemplateWrapper<QPlainTextEdit, QString>
{
public:
    WrapperQPlainTextEdit(const QString name, QPlainTextEdit& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

    QVariant getQValue()
    {
        QString value = qfield.toPlainText();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qfield.setPlainText(value);
    }

    QString extractAndVerify()
    {
        QString value = qfield.toPlainText();

        if (type == REQUIRED_FIELD && value.isEmpty())
            throw std::invalid_argument(QString("ERROR in field %1: Empty !").arg(name).toStdString());

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

class WrapperFileChooser : public TemplateWrapper<IodeFileChooser, QString>
{
public:
    WrapperFileChooser(const QString name, IodeFileChooser& qfield, const EnumItemType type, const IodeFileType& fileType, const EnumFileMode& fileMode) :
        TemplateWrapper(name, qfield, type)
    {
        this->qfield.setFileType(fileType);
        this->qfield.setFileMode(fileMode);
    }

    void setFileType(const IodeFileType fileType)
    {
        this->qfield.setFileType(fileType);
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


class WrapperSampleEdit : public TemplateWrapper<IodeSampleEdit, QString>
{
public:
    WrapperSampleEdit(const QString name, IodeSampleEdit& qfield, const EnumItemType type) : TemplateWrapper(name, qfield, type) {};

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

        if (type == REQUIRED_FIELD)
        {
            if(value.isEmpty())
                throw std::invalid_argument(QString("ERROR in field %1: Empty !").arg(name).toStdString());
            
            // throw an error if the string is not valid for an IODE period
            Period per(value.toStdString());
        }

        return value;
    }
};
