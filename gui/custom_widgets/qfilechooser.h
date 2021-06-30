#pragma once

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

#include <string>
#include <stdexcept>
#include <exception>

#include "../utils.h"


enum EnumFileMode
{
	NewFile,
	ExistingFile,
	FileMayExist,
};


class QIodeFileChooser : public QWidget
{
	Q_OBJECT

public:
	Q_ENUM(EnumItemType)
	Q_ENUM(EnumIodeFile)
	Q_ENUM(EnumFileMode)

private:
	QLineEdit* lineEdit;
	QPushButton* browseButton;

	EnumIodeFile fileType;
	EnumFileMode fileMode;

	QString anyExt;

public:
	QIodeFileChooser(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f),
		lineEdit(new QLineEdit()), browseButton(new QPushButton()), fileType(AnyFile), fileMode(ExistingFile), anyExt(qmapIodeFileExtension.value(AnyFile).ext)
	{
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setMargin(0);

		lineEdit->setPlaceholderText("*.*");
		layout->addWidget(lineEdit);

		browseButton->setText("Browse");
		browseButton->setFixedWidth(browseButton->fontMetrics().width("  " + browseButton->text() + "  "));
		layout->addWidget(browseButton);

		setFocusProxy(lineEdit);

		connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
	}

	~QIodeFileChooser()
	{
		delete lineEdit;
		delete browseButton;
	}

	void setFileType(const EnumIodeFile& fileType) { this->fileType = fileType; }
	void setFileMode(const EnumFileMode& fileMode) { this->fileMode = fileMode; }

	QString getFilepath() { return lineEdit->text(); }
	void setFilepath(const QString& filepath) { lineEdit->setText(filepath); }

	void verify(const QString& name, const EnumItemType& type)
	{
		QString error = QString("ERROR in field %1: ").arg(name);
		QString file = lineEdit->text();
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
		IodeFileExtension expectedExt = qmapIodeFileExtension.value(fileType);
		if (extension.isEmpty()) 
		{
			if (expectedExt.ext == anyExt) throw std::runtime_error(error.toStdString() + "You must provide a file extension");
			fileInfo = QFileInfo(file + expectedExt.ext);
		}
		else
		{
			if (expectedExt.ext != anyExt && extension != expectedExt.ext && extension != expectedExt.ascii)
				throw std::runtime_error((error + QString("Wrong file extension. Expected %1 or %2 but got %3")
					.arg(expectedExt.ext).arg(expectedExt.ascii).arg(extension)).toStdString());
		}
		// check if file exists
		if (fileMode == ExistingFile && !fileInfo.exists())
			throw std::runtime_error((error + QString("The file %1 does not exist !").arg(file)).toStdString());
	}

public slots:
	void updateFileType(const EnumIodeFile& fileType)
	{
		this->fileType = fileType;
	}

private slots:
	void browse()
	{
		QString filename;
		IodeFileExtension expectedExt = qmapIodeFileExtension.value(fileType);

		QString dir;
		QString caption = expectedExt.name + " File";
		QString filter = expectedExt.ext == anyExt ? QString() : expectedExt.name + " (*." + expectedExt.ext + " *." + expectedExt.ascii + ")";
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
		if (!filename.isEmpty()) lineEdit->setText(filename);
	}
};
