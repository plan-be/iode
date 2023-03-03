#pragma once

#include <QObject>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QAbstractTableModel>
#include <QDebug>

#include "utils.h"
#include "custom_widgets/qfilechooser.h"


/* NOTE FOR THE DEVELOPPERS:
 * Because of how Qt is implemented, it is NOT possible for a template class
 * to define/override signals and slots (using Q_OBJECT)
 * ( see https://doc.qt.io/qt-5/moc.html#limitations )
 * 
 * Read-only model: https://doc.qt.io/qt-6/model-view-programming.html#read-only-access
 * Editable model:  https://doc.qt.io/qt-6/model-view-programming.html#editable-items
 * Resizable model: https://doc.qt.io/qt-6/model-view-programming.html#resizable-models
 * 
 */


template <class K> class IODEAbstractTableModel : public QAbstractTableModel
{
protected:
	K* kdb;
	QVector<QString> columnNames;

public:
	IODEAbstractTableModel(QVector<QString> columnNames, QObject* parent = nullptr) : QAbstractTableModel(parent),
		kdb(new K()), columnNames(columnNames) {}

	~IODEAbstractTableModel() { delete kdb; }

	int rowCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return kdb->count();
	}

	int columnCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return columnNames.size();
	}

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void filter(const QString& pattern);

	void clearKDB() { kdb->clear(); }

	bool load(const QString& filepath, const bool forceOverwrite);

	QString save(const QDir& projectDir, const QString& filepath="");

	QString saveAs(const QDir& projectDir);

protected:
	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

	virtual QVariant dataCell(const int row, const int col) const = 0;

	virtual bool setValue(const int row, const int column, const QVariant& value) { return false; }

	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

private:
	QString askFilepath(const QDir& projectDir)
	{
		QString filepath = askFilepathDialog(projectDir);
		QFileInfo fileInfo(filepath);
		if (fileInfo.exists())
		{
			QWidget* mainwin = get_main_window_ptr();
			QMessageBox::StandardButton answer = QMessageBox::warning(mainwin, "Warning", "The file " + 
				fileInfo.fileName() + " already exists. Override it?", 
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Discard, QMessageBox::Yes);
			if (answer == QMessageBox::No) askFilepath(projectDir);
			if (answer == QMessageBox::Discard) filepath = "";
		}
		return filepath;
	}

	QString askFilepathDialog(const QDir& projectDir)
	{
		int iodeType = kdb->get_iode_type();
		QString iodeTypeAsString = QString::fromStdString(vIodeTypes[iodeType]);

		QString defaultFilename = QString(I_DEFAULT_FILENAME) + QString::fromStdString(vFileExtensions[iodeType].ext);
		QString defaultFilepath = projectDir.absoluteFilePath(defaultFilename);

		QWidget* mainwin = get_main_window_ptr();
		QDialog dialog(mainwin);
		dialog.setObjectName(QString::fromUtf8("ask_save_to_dialog"));
		dialog.setMinimumWidth(380);
		dialog.setMinimumHeight(100);
		dialog.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		dialog.setWindowTitle("Where to save " + iodeTypeAsString + " ?");
		// layout
        QVBoxLayout* layout = new QVBoxLayout(static_cast<QWidget*>(&dialog));
        layout->setObjectName(QString::fromUtf8("layout"));
		// label
		QLabel label("Please provide the path to the file where " + iodeTypeAsString + " will be stored");
		layout->addWidget(&label, 0, Qt::AlignLeft);
		// line edit + browse button
		QIodeFileChooser fileChooser(static_cast<QWidget*>(&dialog));
		fileChooser.setMinimumWidth(360);
		fileChooser.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		fileChooser.setFileType((EnumIodeFile) iodeType);
		fileChooser.setFileMode(FILE_MAY_EXIST);
		fileChooser.setFilepath(defaultFilepath);
		layout->addWidget(static_cast<QWidget*>(&fileChooser), 0, Qt::AlignLeft);
		// Ok / Cancel button
		QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    	connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
		layout->addWidget(static_cast<QWidget*>(&buttonBox), 0, Qt::AlignCenter);
		// execute dialog box
		QDialog::DialogCode answer = (QDialog::DialogCode) dialog.exec();
		// extract filepath if user clicked on Ok
		QString filePath;
		if (answer == QDialog::Accepted) filePath = fileChooser.getFilepath();
		// return filepath
		return filePath;
	}
};
