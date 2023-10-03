#pragma once
#include "abstract_table_model.h"


template <class K>
Qt::ItemFlags IodeTemplateTableModel<K>::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// Vertical header = IODE object names
template <class K>
QVariant IodeTemplateTableModel<K>::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return columnNames[section];
	else
	{
		if(!kdb)
			return QVariant(" ");
		
		try
		{			
			return QString::fromStdString(kdb->get_name(section));
		}
		catch(const std::exception& e)
		{
			return QVariant(" ");
		}
	}

	return QVariant(" ");
}

template <class K>
bool IodeTemplateTableModel<K>::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	if(orientation == Qt::Horizontal)
		return false;

	if(role != Qt::EditRole)
		return false;

	if(section >= rowCount())
		return false;

	if(!kdb)
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
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
		return false;
	}
}

template <class K>
QVariant IodeTemplateTableModel<K>::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(Qt::AlignLeft);

	if ((role == Qt::DisplayRole || role == Qt::EditRole) && kdb)
		return dataCell(index.row(), index.column());

	return QVariant();
}

template <class K>
bool IodeTemplateTableModel<K>::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		if(!kdb)
			return false;
		
		if(data(index, Qt::DisplayRole) == value) 
			return false;
		
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
void IodeTemplateTableModel<K>::filter(const QString& pattern, const bool silent)
{
	if (!pattern.isEmpty())
	{
		try
		{
			if(kdb_filter) 
				delete kdb_filter;
			kdb_filter = new K(KDB_SHALLOW_COPY, pattern.toStdString());
			kdb = kdb_filter;
		}
		catch (const std::exception& e)
		{
			kdb_filter = nullptr;
			kdb = kdb_global ? kdb_global : kdb_external;
			if(!silent)
				QMessageBox::warning(static_cast<QWidget*>(parent()), "WARNING", QString(e.what()));
		}
	}
	else
		kdb = kdb_global ? kdb_global : kdb_external;
}

template <class K>
bool IodeTemplateTableModel<K>::load(const QString& filepath, const bool forceOverwrite)
{
	std::string s_filepath = filepath.toStdString();
	int type_ = kdb ? kdb->get_iode_type() : get_iode_file_type(s_filepath);

	if (type_ < 0 || type_ > I_VARIABLES) 
		return false;

	EnumIodeType iodeType = (EnumIodeType) type_;

	try
	{
		s_filepath = check_filepath(s_filepath, (EnumIodeFile) iodeType, "load file", true);

		if(!forceOverwrite && is_global_kdb_loaded(iodeType))
		{
			QString iodeTypeName = QString::fromStdString(vIodeTypes[iodeType]);
			QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", "There are " + 
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
		QMessageBox::critical(nullptr, "ERROR", QString(e.what()));
		return false;
	}
}

template <class K>
QString IodeTemplateTableModel<K>::save(const QDir& projectDir, const QString& filepath)
{
	if(!kdb)
		return "";

	if (kdb->count() == 0) 
		return ""; 

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
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return "";
	}

	return fullPath;
}

template <class K>
QString IodeTemplateTableModel<K>::saveAs(const QDir& projectDir)
{
	if(!kdb)
		return "";

	if (kdb->count() == 0) 
		return ""; 
	
	// ask user for new filepath
	QString filepath = askFilepath(projectDir);
	// call the save method
	filepath = save(projectDir, filepath);
	// update KDB filename
	if (!filepath.isEmpty()) kdb->set_filename(filepath.toStdString());
	return filepath;
}

template <class K>
bool IodeTemplateTableModel<K>::removeRows(int position, int rows, const QModelIndex& index)
{
	if(!kdb)
		return false;

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
		QMessageBox::warning(static_cast<QWidget*>(parent()), tr("WARNING"), tr(e.what()));
	}

	endRemoveRows();
	return true;
}

template <class K>
QStringList IodeTemplateTableModel<K>::getSameObjOrObjsFromClec(const QString& name, const EnumIodeType other_type)
{
	QStringList list;

	if(!kdb)
		return list;

	std::string std_name = name.toStdString();
	int this_type = kdb->get_iode_type();
	bool listedInClec = other_type == I_SCALARS || other_type == I_VARIABLES;

	if(this_type == I_EQUATIONS && listedInClec)
	{
		Equation eq(std_name);

		std::vector<std::string> std_list;
		if(other_type == I_SCALARS)
			std_list =  eq.get_coefficients_list();
		else
			std_list = eq.get_variables_list();

		if(std_list.size() == 0)
			return list;

		for(const std::string& obj_name : std_list)
			list << QString::fromStdString(obj_name);
	}
	else if(this_type == I_IDENTITIES && listedInClec)
	{
		Identity idt(std_name, nullptr);

		std::vector<std::string> std_list;
		if(other_type == I_SCALARS)
			std_list =  idt.get_coefficients_list();
		else
			std_list = idt.get_variables_list();

		if(std_list.size() == 0)
			return list;

		for(const std::string& obj_name : std_list)
			list << QString::fromStdString(obj_name);
	}
	else
	{
		KDBComments kdb_cmt;
		KDBEquations kdb_eqs;
		KDBIdentities kdb_idt;
		KDBLists kdb_lst;
		KDBScalars kdb_scl;
		KDBTables kdb_tbl;
		KDBVariables kdb_var;

		switch (other_type)
		{
		case I_COMMENTS:
			if(kdb_cmt.contains(std_name))
				list << name;
			break;
		case I_EQUATIONS:
			if(kdb_eqs.contains(std_name))
				list << name;
			break;
		case I_IDENTITIES:
			if(kdb_idt.contains(std_name))
				list << name;
			break;
		case I_LISTS:
			if(kdb_lst.contains(std_name))
				list << name;
			break;
		case I_SCALARS:
			if(kdb_scl.contains(std_name))
				list << name;
			break;
		case I_TABLES:
			if(kdb_tbl.contains(std_name))
				list << name;
			break;
		case I_VARIABLES:
			if(kdb_var.contains(std_name))
				list << name;
			break;
		default:
			break;
		}
	}

	return list;
}

template <class K>
QStringList IodeTemplateTableModel<K>::getRelatedObjs(const QString& name, const EnumIodeType other_type)
{
	QStringList list;

	if(!kdb)
		return list;
	
	std::vector<std::string> std_list = kdb->get_associated_objects_list(name.toStdString(), other_type);
	if(std_list.size() == 0)
		return list;

	for(const std::string& obj_name: std_list)
		list << QString::fromStdString(obj_name);
	return list;
}
