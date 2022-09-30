#pragma once
#include "abstract_table_model.h"


template <class K>
Qt::ItemFlags QIodeTemplateTableModel<K>::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// Vertical header = IODE object names
template <class K>
QVariant QIodeTemplateTableModel<K>::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return columnNames[section];
	else
	{
		try
		{
			return QString::fromStdString(kdb->get_name(section));
		}
		catch(const std::exception& e)
		{
			return QVariant("  ");
		}
	}

	return QVariant("  ");
}

template <class K>
bool QIodeTemplateTableModel<K>::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	if(orientation == Qt::Horizontal)
		return false;

	if(role != Qt::EditRole)
		return false;

	if(section >= rowCount())
		return false;

	try
	{
		QString old_name = headerData(section, orientation).toString();
		QString new_name = value.toString();
		kdb->rename(old_name.toStdString(), new_name.toStdString());
		emit headerDataChanged(orientation, section, section);
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
		return false;
	}
}

template <class K>
QVariant QIodeTemplateTableModel<K>::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignLeft);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return dataCell(index.row(), index.column());

	return QVariant();
}

template <class K>
bool QIodeTemplateTableModel<K>::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		bool success = setValue(index.row(), index.column(), value);
		if (success)
		{
			emit dataChanged(index, index, { role });
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

template <class K>
void QIodeTemplateTableModel<K>::filter(const QString& pattern)
{
	try
	{
		if(kdb_filter) delete kdb_filter;
		if (!pattern.isEmpty())
		{
			kdb_filter = new K(KDB_SHALLOW_COPY, pattern.toStdString());
			kdb = kdb_filter;
		}
		else
		{
			kdb = (kdb_global != nullptr) ? kdb_global : kdb_external;
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(static_cast<QWidget*>(parent()), tr("Error"), tr(e.what()));
	}
}


template <class K>
bool QIodeTemplateTableModel<K>::load(const QString& filepath, const bool forceOverwrite)
{
	QWidget* mainwin = get_main_window_ptr();

	int type_ = kdb->get_iode_type();
	if (type_ < 0) return false;

	EnumIodeType iodeType = (EnumIodeType) type_;
	try
	{
		std::string s_filepath = filepath.toStdString();
		s_filepath = check_filepath(s_filepath, (EnumIodeFile) iodeType, "load file", true);

		if(!forceOverwrite && is_global_kdb_loaded(iodeType))
		{
			QString iodeTypeName = QString::fromStdString(vIodeTypes[iodeType]);
			QMessageBox::StandardButton answer = QMessageBox::warning(mainwin, "Warning", "There are " + 
				iodeTypeName + " already loaded. Would like to override " + iodeTypeName + " ?", 
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(answer == QMessageBox::No) return false;
		}
		// load Iode file
		load_global_kdb(iodeType, s_filepath);
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(mainwin, tr("ERROR"), tr(e.what()));
		return false;
	}
}

template <class K>
QString QIodeTemplateTableModel<K>::save(const QDir& projectDir, const QString& filepath)
{
		if (kdb->count() == 0) return ""; 

		EnumIodeType iodeType = (EnumIodeType) kdb->get_iode_type();
		
		// if not provided as argument, get path to the file associated with KDB of objects of type iodeType
		std::string std_filepath = filepath.isEmpty() ? kdb->get_filename() : filepath.toStdString();

		// if KDB not linked to any file, ask the user to give/create a file to save in.
		// Otherwise, check the filepath 
		if (std_filepath.empty() || std_filepath == std::string(I_DEFAULT_FILENAME))
		{
			// open a box dialog 
			QString filepath = askFilepath(projectDir);
			// filepath is empty if the user clicked on the Discard button
			if (filepath.isEmpty()) return "";
			std_filepath = filepath.toStdString();
		}
		
		std_filepath = check_filepath(std_filepath, (EnumIodeFile) iodeType, "tab " + vIodeTypes[iodeType], false);

		QFileInfo fileInfo(QString::fromStdString(std_filepath));
		QString fullPath = fileInfo.absoluteFilePath();

		try
		{
			kdb->set_filename(fullPath.toStdString());
			std::string full_path = fullPath.toStdString();
			kdb->dump(full_path);
		}
		catch (const IodeException& e)
		{
			QWidget* mainwin = get_main_window_ptr();
			QMessageBox::critical(mainwin, "Error", e.what());
			return "";
		}

		return fullPath;
}

template <class K>
QString QIodeTemplateTableModel<K>::saveAs(const QDir& projectDir)
{
	if (kdb->count() == 0) return ""; 
	
	// ask user for new filepath
	QString filepath = askFilepath(projectDir);
	// call the save method
	filepath = save(projectDir, filepath);
	// update KDB filename
	if (!filepath.isEmpty()) kdb->set_filename(filepath.toStdString());
	return filepath;
}

template <class K>
bool QIodeTemplateTableModel<K>::removeRows(int position, int rows, const QModelIndex& index)
{
	std::string name;
	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	try
	{
		for (int row = position; row < position + rows; row++)
		{
			name = dataCell(row, 0).toString().toStdString();
			kdb->remove(row);
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("Warning"), tr(e.what()));
	}

	endRemoveRows();
	return true;
}
