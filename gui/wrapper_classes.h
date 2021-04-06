#pragma once

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
    T& qitem;
    const EnumItemType type;

public:
    TemplateWrapper(const QString name, T& qitem, const EnumItemType& type) : name(name), qitem(qitem), type(type) {};

    T& getQItem() { return qitem; }

    virtual U extractAndVerify() = 0;
};


class WrapperComboBox : public TemplateWrapper<QComboBox, int>
{
    const QMap<QString, int>& map_values;

public:
    WrapperComboBox(const QString name, QComboBox& qitem, const EnumItemType& type, const QMap<QString, int>& map_values) :
        TemplateWrapper(name, qitem, type), map_values(map_values) {};

    QVariant getQValue()
    {
        QString text = qitem.currentText();
        int value = map_values.value(text, -1);
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        int value = qvalue.toInt();
        qitem.setCurrentIndex(value);
    }

    // QString getTextValue() { return qitem.currentText(); }

    int extractAndVerify()
    {
        QString text = qitem.currentText();
        if (!map_values.contains(text)) throw std::runtime_error("Value " + text.toStdString() + " not allowed in " + name.toStdString());
        int value = map_values.value(text);
        return value;
    }
};


class WrapperQLineEdit : public TemplateWrapper<QLineEdit, QString>
{
public:
    WrapperQLineEdit(const QString name, QLineEdit& qitem, const EnumItemType& type) : TemplateWrapper(name, qitem, type) {};

    QVariant getQValue()
    {
        QString value = qitem.text();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qitem.setText(value);
    }

    QString extractAndVerify()
    {
        QString value = qitem.text();

        if (type == Required && value.isEmpty())
            throw std::runtime_error(QString("ERROR in field %1: Empty !").arg(name).toStdString());

        return value;
    }
};


// ---- Custom Widgets ----

class WrapperFileChooser : public TemplateWrapper<QFileChooser, QString>
{
public:
    WrapperFileChooser(const QString name, QFileChooser& qitem, const EnumItemType& type, const EnumIodeFile& fileType, const EnumFileMode& fileMode) :
        TemplateWrapper(name, qitem, type)
    {
        this->qitem.setFileType(fileType);
        this->qitem.setFileMode(fileMode);
    }

    QVariant getQValue()
    {
        QString value = qitem.getFilepath();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue)
    {
        QString value = qvalue.toString();
        qitem.setFilepath(value);
    }

    QString extractAndVerify()
    {
        qitem.verify(name, type);
        return qitem.getFilepath();
    }
};


class WrapperSampleEdit : public TemplateWrapper<QSampleEdit, QString>
{
public:
    WrapperSampleEdit(const QString name, QSampleEdit& qitem, const EnumItemType& type) : TemplateWrapper(name, qitem, type) {};

    QVariant getQValue()
    {
        QString value = qitem.text();
        return QVariant(value);
    }

    void setQValue(const QVariant& qvalue) 
    { 
        QString value = qvalue.toString(); 
        qitem.setText(value);
    }

    QString extractAndVerify() 
    {
        return qitem.text();
    }
};
