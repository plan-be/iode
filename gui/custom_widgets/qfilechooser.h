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
		lineEdit(new QLineEdit()), browseButton(new QPushButton()), fileType(I_ANY_FILE), fileMode(EXISTING_FILE), 
		anyExt(QString::fromStdString(vFileExtensions[I_ANY_FILE].ext))
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
