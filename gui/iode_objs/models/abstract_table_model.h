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
#include "util/widgets/file_chooser.h"
#include <boost/functional/hash.hpp>


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


class IodeAbstractTableModel : public QAbstractTableModel
{
	Q_OBJECT

protected:
	QVector<QString> columnNames;
	size_t hashBefore;
	size_t hashAfter;	

signals:
	void databaseModified();

public:
	IodeAbstractTableModel(QVector<QString> columnNames, QObject* parent = nullptr) : 
		QAbstractTableModel(parent), columnNames(columnNames) {}


	int columnCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return columnNames.size();
	}

	virtual int getNbObjects() const = 0;

	virtual QString getFilepath() const = 0;

protected:
	void resetModel()
	{
		beginResetModel();
		endResetModel();
	}

public slots:
	virtual void reset() 
	{ 
		resetModel(); 
	}

	virtual void filter(const QString& pattern, const bool silent = false) = 0;

	/**
	 * @brief gets the list of scalars or variables listed in the clec structure or 
	 *        the object of the same name.
	 * 
	 * @param name 
	 * @param other_type 
	 */
	virtual QStringList getSameObjOrObjsFromClec(const QString& name, const EnumIodeType other_type) = 0;

	/**
	 * @brief get the list of all related objects of type other_type.
	 *  
	 * @param name 
	 * @param other_type 
	 */
	virtual QStringList getRelatedObjs(const QString& name, const EnumIodeType other_type) = 0;
};


template <class K> class IodeTemplateTableModel : public IodeAbstractTableModel
{
protected:
	K* kdb;
	K* kdb_filter;		///< to be deleted
	K* kdb_global;		///< to be deleted
	K* kdb_external;	///< must NOT be deleted (external owner)

public:
	IodeTemplateTableModel(QVector<QString> columnNames, QObject* parent = nullptr, K* kdb_external = nullptr) : 
		IodeAbstractTableModel(columnNames, parent)
	{
		kdb_filter = nullptr;
		if(kdb_external)
		{
			kdb_global = nullptr;
			this->kdb_external = kdb_external;
			kdb = kdb_external;
		}
		else
		{
			kdb_global = new K();
			this->kdb_external = nullptr;
			kdb = kdb_global;
		}
	}

	~IodeTemplateTableModel() 
	{ 
		if(kdb_global)
			delete kdb_global;

		if(kdb_filter)
			delete kdb_filter;

		kdb = nullptr; 
	}

	int rowCount(const QModelIndex& parent = QModelIndex()) const override
	{
		return kdb ? kdb->count() : 0;
	}

	int getNbObjects() const override
	{
		// NOTE: we don't simply use kdb since it may point to kdb_filter
		K* kdb_ = kdb_global ? kdb_global : kdb_external;
		return kdb_ ? kdb_->count() : 0;
	}

	QString getFilepath() const
	{
		// NOTE: we don't simply use kdb since it may point to kdb_filter
		K* kdb_ = kdb_global ? kdb_global : kdb_external;
		return kdb_ ? QString::fromStdString(kdb_->get_filename()) : "";
	}

	Qt::ItemFlags flags(const QModelIndex& index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void filter(const QString& pattern, const bool silent = false) override;

	void clearKDB() 
	{
		// NOTE: we don't simply use kdb since it may point to kdb_filter
		K* kdb_ = kdb_global ? kdb_global : kdb_external;
		if(kdb_)
			kdb_->clear();
	}

	bool load(const QString& filepath, const bool forceOverwrite);

	QString save(const QDir& projectDir, const QString& filepath="");

	QString saveAs(const QDir& projectDir);

	void computeHash(const bool before=false)
	{
		// NOTE: computing the hash value of 'kdb_filter' makes no sense.
		//       So we can't simply pass the 'kdb' class member to kdb_hasher 
		//       because 'kdb' may point to 'kdb_filter' if filtering is activated.
		K* kdb_ = kdb_global ? kdb_global : kdb_external;
		if(!kdb_)
			return;
		
		boost::hash<K> kdb_hasher;

		if(before)
    		hashBefore = kdb_hasher(*kdb_);
		else
		{
			hashAfter = kdb_hasher(*kdb_);
			if(hashAfter != hashBefore) 
				emit databaseModified();
		}
	}

	virtual QVariant dataCell(const int row, const int col) const = 0;

	virtual bool setValue(const int row, const int column, const QVariant& value) 
	{ 
		return false; 
	}

	QStringList getSameObjOrObjsFromClec(const QString& name, const EnumIodeType other_type) override;

	QStringList getRelatedObjs(const QString& name, const EnumIodeType other_type) override;

protected:
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

private:
	QString askFilepath(const QDir& projectDir)
	{
		QString filepath = askFilepathDialog(projectDir);
		if(filepath.isEmpty())
			return "";
		
		QFileInfo fileInfo(filepath);
		if (fileInfo.exists())
		{
			QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", "The file " + 
				fileInfo.fileName() + " already exists. Override it?", 
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Discard, QMessageBox::Yes);
			if (answer == QMessageBox::No) 
				askFilepath(projectDir);
			if (answer == QMessageBox::Discard) 
				filepath = "";
		}

		return filepath;
	}

	QString askFilepathDialog(const QDir& projectDir)
	{
		// NOTE: we don't simply use kdb since it may point to kdb_filter
		K* kdb_ = kdb_global ? kdb_global : kdb_external;

		if(!kdb_)
			return "";
		
		int iodeType = kdb_->get_iode_type();
		QString iodeTypeAsString = QString::fromStdString(vIodeTypes[iodeType]);

		QString defaultFilename = QString(I_DEFAULT_FILENAME) + "." + QString::fromStdString(v_binary_ext[iodeType]);
		QString defaultFilepath = projectDir.absoluteFilePath(defaultFilename);

		QDialog dialog;
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
		IodeFileChooser fileChooser(static_cast<QWidget*>(&dialog));
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
