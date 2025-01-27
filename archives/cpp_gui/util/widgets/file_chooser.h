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

#include "utils.h"


enum EnumFileMode
{
	NEW_FILE,
	EXISTING_FILE,
	FILE_MAY_EXIST
};


class IodeFileChooser : public QWidget
{
	Q_OBJECT

public:
	Q_ENUM(EnumItemType)
	Q_ENUM(IodeFileType)
	Q_ENUM(EnumFileMode)

private:
	QLineEdit* lineEdit;
	QPushButton* browseButton;

	IodeFileType fileType;
	EnumFileMode fileMode;

public:
	IodeFileChooser(QWidget* parent = nullptr) : QWidget(parent),
		lineEdit(new QLineEdit()), browseButton(new QPushButton()), fileType(FILE_ANY), fileMode(EXISTING_FILE)
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

		connect(browseButton, &QPushButton::clicked, this, &IodeFileChooser::browse);

		// There is no QFileDialog::getSaveDirectory(). 
		// Only QFileDialog::getExistingDirectory()
		if (fileType == DIRECTORY) fileMode == EXISTING_FILE;
	}

	~IodeFileChooser()
	{
		delete lineEdit;
		delete browseButton;
	}

	void setFileType(const IodeFileType& fileType) { this->fileType = fileType; }
	void setFileMode(const EnumFileMode& fileMode) { this->fileMode = fileMode; }

	QString getFilepath() { return lineEdit->text(); }
	void setFilepath(const QString& filepath) { lineEdit->setText(filepath); }

public slots:
	void updateFileType(const IodeFileType& fileType)
	{
		this->fileType = fileType;
	}

private slots:
	void browse()
	{
		QString path = lineEdit->text();
		QString name = QString::fromStdString(v_file_types[fileType].name);
		
		QString filter; 
		if(fileType != FILE_ANY)
		{
			filter = name + " (";
			for(const std::string& ext: v_file_types[fileType].v_ext)
        		filter += "*" + QString::fromStdString(ext) + " ";
			filter.chop(1);
			filter += ")";
		}

		QString rootDir;
		if (path.isEmpty())
			rootDir = QDir::homePath();
		else
		{
			QDir parentDir = QFileInfo(path).absoluteDir();
			rootDir = (parentDir.exists()) ? parentDir.absolutePath() : QDir::homePath();
		}

		QString caption = name + " File";
		if (fileMode == EXISTING_FILE)
		{
			if (fileType == DIRECTORY)
				path = QFileDialog::getExistingDirectory(this, "Open Directory", rootDir, QFileDialog::ShowDirsOnly);
			else
			{
				caption = "Open " + caption;
				path = QFileDialog::getOpenFileName(this, caption, rootDir, filter);
			}
		}
		else
		{
			caption = "Save " + caption;
			path = QFileDialog::getSaveFileName(this, caption, rootDir, filter);
		}
		if (!path.isEmpty()) lineEdit->setText(path);
	}
};