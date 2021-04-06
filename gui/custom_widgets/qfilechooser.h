#pragma once

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

#include <string>
#include <stdexcept>
#include <exception>

#include "../utils.h"


constexpr int MAX_VISIBLE_ITEMS = 10;
constexpr int MAX_COUNT = 30;

enum EnumFileMode
{
	NewFile,
	ExistingFile,
	FileMayExist,
};


class QFileChooser : public QWidget
{
	Q_OBJECT

public:
	Q_ENUM(EnumItemType)
	Q_ENUM(EnumIodeFile)
	Q_ENUM(EnumFileMode)

private:
	QComboBox* comboBox;
	QPushButton* browseButton;

	EnumIodeFile fileType;
	EnumFileMode fileMode;

	QString anyExt;

public:
	QFileChooser(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f),
		comboBox(new QComboBox()), browseButton(new QPushButton()), fileType(AnyFile), fileMode(ExistingFile), anyExt(qmapIodeFileExtension.value(AnyFile).ext)
	{
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setMargin(0);

		comboBox->setPlaceholderText("*.*");
		comboBox->setEditable(true);
		comboBox->setMaxCount(MAX_COUNT);
		comboBox->setMaxVisibleItems(MAX_VISIBLE_ITEMS);
		layout->addWidget(comboBox);

		browseButton->setText("Browse");
		browseButton->setFixedWidth(browseButton->fontMetrics().width("  " + browseButton->text() + "  "));
		layout->addWidget(browseButton);

		setFocusProxy(comboBox);

		connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
	}

	~QFileChooser()
	{
		delete comboBox;
		delete browseButton;
	}

	void setFileType(const EnumIodeFile& fileType) { this->fileType = fileType; }
	void setFileMode(const EnumFileMode& fileMode) { this->fileMode = fileMode; }

	QString getFilepath() { return comboBox->currentText(); }
	void setFilepath(const QString& filepath) { comboBox->setCurrentText(filepath); }

	void verify(const QString& name, const EnumItemType& type)
	{
		QString error = QString("ERROR in field %1: ").arg(name);
		QString file = comboBox->currentText();
		// check if empty
		if (file.isEmpty())
		{
			if (type == Optional) return;
			throw std::runtime_error(error.toStdString() + "No file provided");
		}

		QFileInfo fileInfo = QFileInfo(file);
		// check if file is not a directory
		if (fileInfo.isDir())  throw std::runtime_error((error + QString("The filepath %1 represents a directory, not a file").arg(file)).toStdString());
		// check if correct/must provide extension
		QString extension = fileInfo.suffix();
		QString expectedExt = qmapIodeFileExtension.value(fileType).ext;
		if (extension.isEmpty()) 
		{
			if (expectedExt == anyExt) throw std::runtime_error(error.toStdString() + "You must provide a file extension");
			fileInfo = QFileInfo(file + expectedExt);
		}
		else
		{
			if (expectedExt != anyExt && extension != expectedExt)
				throw std::runtime_error((error + QString("Wrong file extension. Expected %1 but got %2").arg(expectedExt).arg(extension)).toStdString());
		}
		// check if file exists
		if (!fileInfo.exists()) throw std::runtime_error((error + QString("The file %1 does not exist !").arg(file)).toStdString());
	}

public slots:
	void updateFileType(const EnumIodeFile& fileType)
	{
		this->fileType = fileType;
	}

private slots:
	void browse()
	{
		QString filename = "";
		QString fileTypeName = qmapIodeFileExtension.value(fileType).name;
		QString expectedExt = qmapIodeFileExtension.value(fileType).ext;

		QString dir = QString();
		QString caption = fileTypeName + " File";
		QString filter = expectedExt == anyExt ? QString() : fileTypeName + " (*." + expectedExt + ")";
		if (fileMode == NewFile)
		{
			caption = "Save " + caption;
			filename = QFileDialog::getSaveFileName(this, caption, dir, filter);
		}
		else
		{
			caption = "Open " + caption;
			filename = QFileDialog::getOpenFileName(this, caption, dir, filter);
		}
		if (!filename.isEmpty()) comboBox->setCurrentText(filename);
	}
};
