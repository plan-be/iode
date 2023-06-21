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

public:
	QIodeFileChooser(QWidget* parent = nullptr) : QWidget(parent),
		lineEdit(new QLineEdit()), browseButton(new QPushButton()), fileType(I_ANY_FILE), fileMode(EXISTING_FILE)
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

		// There is no QFileDialog::getSaveDirectory(). 
		// Only QFileDialog::getExistingDirectory()
		if (fileType == I_DIRECTORY) fileMode == EXISTING_FILE;
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

public slots:
	void updateFileType(const EnumIodeFile& fileType)
	{
		this->fileType = fileType;
	}

private slots:
	void browse()
	{
		QString path = lineEdit->text();
		QString name = QString::fromStdString(v_ext_names[fileType]);
		
		QString filter; 
		if(fileType != I_ANY_FILE)
		{
			filter = name + " (";
			for(const std::string& ext: get_extensions(fileType))
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
			if (fileType == I_DIRECTORY)
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
