#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

#include "../utils.h"


enum EnumFileMode
{
	NEW_FILE,
	EXISTING_FILE,
	FILE_MAY_EXIST
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
		lineEdit(new QLineEdit()), browseButton(new QPushButton()), fileType(ANY_FILE), fileMode(EXISTING_FILE), 
		anyExt(QString::fromStdString(vFileExtensions[ANY_FILE].ext))
	{
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->setSpacing(1);

		lineEdit->setPlaceholderText("*.*");
		layout->addWidget(lineEdit);

		browseButton->setText("Browse");
		browseButton->setFixedWidth(browseButton->fontMetrics().horizontalAdvance("  " + browseButton->text() + "  "));
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
			if (type == OPTIONAL_FIELD) return;
			throw std::runtime_error(error.toStdString() + "No file provided");
		}

		QFileInfo fileInfo = QFileInfo(file);
		// check if file is not a directory
		if (fileInfo.isDir())  throw std::runtime_error((error + QString("The filepath %1 represents a directory, not a file").arg(file)).toStdString());
		// check if correct/must provide extension
		QString extension = fileInfo.suffix();
		QString ext = QString::fromStdString(vFileExtensions[fileType].ext);
		if (extension.isEmpty())
		{
			if (ext == anyExt) throw std::runtime_error(error.toStdString() + "You must provide a file extension");
			fileInfo = QFileInfo(file + ext);
		}
		else
		{
			QString ascii = QString::fromStdString(vFileExtensions[fileType].ascii);
			if (ext != anyExt && extension != ext && extension != ascii)
				throw std::runtime_error((error + QString("Wrong file extension. Expected %1 or %2 but got %3")
					.arg(ext).arg(ascii).arg(extension)).toStdString());
		}
		// check if file exists
		if (fileMode == EXISTING_FILE && !fileInfo.exists())
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
		IodeFileExtension expectedExt = vFileExtensions[fileType];
		QString name = QString::fromStdString(expectedExt.name);
		QString ext = QString::fromStdString(expectedExt.ext);
		QString ascii = QString::fromStdString(expectedExt.ascii);

		QString dir;
		QString caption = name + " File";
		QString filter = ext == anyExt ? QString() : name + " (*." + ext + " *." + ascii + ")";
		if (fileMode == EXISTING_FILE)
		{
			caption = "Open " + caption;
			filename = QFileDialog::getOpenFileName(this, caption, dir, filter);
		}
		else
		{
			caption = "Save " + caption;
			filename = QFileDialog::getSaveFileName(this, caption, dir, filter);
		}
		if (!filename.isEmpty()) lineEdit->setText(filename);
	}
};
