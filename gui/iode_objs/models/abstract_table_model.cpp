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
		try
		{			
			return QString::fromStdString(displayed_database->get_name(section));
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

	try
	{
		QString old_name = headerData(section, orientation).toString();
		QString new_name = value.toString();
		displayed_database->rename(old_name.toStdString(), new_name.toStdString());
		emit headerDataChanged(orientation, section, section);
		return true;
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return false;
	}
}

template <class K>
QVariant IodeTemplateTableModel<K>::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
		return int(alignment);

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return dataCell(index.row(), index.column());

	return QVariant();
}

template <class K>
bool IodeTemplateTableModel<K>::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{	
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
	if(database_subset)
	{
		delete database_subset;
		database_subset = nullptr;
	} 

	if (pattern.isEmpty())
		displayed_database = database;
	else
	{
		try
		{
			database_subset = database->subset(pattern.toStdString());
			displayed_database = database_subset;
		}
		catch (const std::exception& e)
		{
			database_subset = nullptr;
			displayed_database = database;
			if(!silent)
				QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		}
	}
}

template <class K>
bool IodeTemplateTableModel<K>::load(const QString& filepath, const bool forceOverwrite)
{
	std::string std_filepath = filepath.toLocal8Bit().toStdString();
	int type_ = database->get_iode_type();

	if (type_ < 0 || type_ > VARIABLES) 
		return false;

	EnumIodeType iodeType = (EnumIodeType) type_;

	try
	{
		// NOTE: check_filepath() converts to absolute path
		std_filepath = check_filepath(std_filepath, (EnumIodeFile) iodeType, "load file", true);

		if(!forceOverwrite && (database->count() > 0))
		{
			QString iodeTypeName = QString::fromStdString(vIodeTypes[iodeType]);
			QMessageBox::StandardButton answer = QMessageBox::warning(nullptr, "WARNING", "There are " + 
				iodeTypeName + " already loaded. Would like to override " + iodeTypeName + " ?", 
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(answer == QMessageBox::No) return false;
		}

		// load Iode file in the global database
		K kdb_(std_filepath);

		// reset subset
		if(database_subset)
		{
			delete database_subset;
			database_subset = nullptr;
		}

		// point 'displayed_database' to the newly loaded 'database'
		displayed_database = database;

		// update the view
		resetModel();

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
	// NOTE: we don't simply use 'database' since it may point to 'database_subset'
	if (database->count() == 0) 
		return ""; 

	EnumIodeType iodeType = (EnumIodeType) database->get_iode_type();
	
	// if not provided as argument, get path to the file associated with KDB of objects of type iodeType
	std::string std_filepath = filepath.isEmpty() ? database->get_filename() : filepath.toLocal8Bit().toStdString();

	// if KDB not linked to any file, ask the user to give/create a file to save in.
	// Otherwise, check the filepath 
	if (std_filepath.empty() || std_filepath == std::string(I_DEFAULT_FILENAME))
	{
		// open a box dialog 
		QString filepath = askFilepath(projectDir);
		// filepath is empty if the user clicked on the Discard button
		if (filepath.isEmpty()) return "";
		std_filepath = filepath.toLocal8Bit().toStdString();
	}
	
	// NOTE: check_filepath() converts to absolute path
	std_filepath = check_filepath(std_filepath, (EnumIodeFile) iodeType, "tab " + vIodeTypes[iodeType], false);

	try
	{
		database->save(std_filepath);
		database->set_filename(std_filepath);
	}
	catch (const IodeException& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
		return "";
	}

	QString fullPath = QString::fromLocal8Bit(QByteArray(std_filepath.c_str())); 
	return fullPath;
}

template <class K>
QString IodeTemplateTableModel<K>::saveAs(const QDir& projectDir)
{
	// NOTE: we don't simply use 'database' since it may point to 'database_subset'
	if (database->count() == 0) 
		return ""; 
	
	// ask user for new filepath
	QString filepath = askFilepath(projectDir);
	// call the save method
	filepath = save(projectDir, filepath);
	// update KDB filename
	if (!filepath.isEmpty()) database->set_filename(filepath.toLocal8Bit().toStdString());
	return filepath;
}

template <class K>
bool IodeTemplateTableModel<K>::removeRows(int position, int rows, const QModelIndex& index)
{
	QString name;
	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	try
	{
		for (int row = position; row < position + rows; row++)
		{
			name = headerData(row, Qt::Vertical).toString();
			displayed_database->remove(name.toStdString());
			emit objectRemoved(name);
		}
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(nullptr, "WARNING", QString(e.what()));
	}

	endRemoveRows();
	return true;
}

template <class K>
QStringList IodeTemplateTableModel<K>::getSameObjOrObjsFromClec(const QString& name, const EnumIodeType other_type)
{
	QStringList list;

	if(database->count() == 0)
		return list;

	std::string std_name = name.toStdString();
	int this_type = database->get_iode_type();
	bool listedInClec = other_type == SCALARS || other_type == VARIABLES;

	if(this_type == EQUATIONS && listedInClec)
	{
		Equation eq(std_name);

		std::vector<std::string> std_list;
		if(other_type == SCALARS)
			std_list =  eq.get_coefficients_list();
		else
			std_list = eq.get_variables_list();

		if(std_list.size() == 0)
			return list;

		for(const std::string& obj_name : std_list)
			list << QString::fromStdString(obj_name);
	}
	else if(this_type == IDENTITIES && listedInClec)
	{
		Identity idt(std_name, nullptr);

		std::vector<std::string> std_list;
		if(other_type == SCALARS)
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
		switch (other_type)
		{
		case COMMENTS:
			if(Comments.contains(std_name))
				list << name;
			break;
		case EQUATIONS:
			if(Equations.contains(std_name))
				list << name;
			break;
		case IDENTITIES:
			if(Identities.contains(std_name))
				list << name;
			break;
		case LISTS:
			if(Lists.contains(std_name))
				list << name;
			break;
		case SCALARS:
			if(Scalars.contains(std_name))
				list << name;
			break;
		case TABLES:
			if(Tables.contains(std_name))
				list << name;
			break;
		case VARIABLES:
			if(Variables.contains(std_name))
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

	if(database->count() == 0)
		return list;
	
	std::string cpp_name = name.toStdString();
	std::vector<std::string> std_list;
	switch (other_type)
	{
	case COMMENTS:
		std_list = Comments.search(cpp_name);
		break;
	case EQUATIONS:
		std_list = Equations.search(cpp_name);
		break;
	case IDENTITIES:
		std_list = Identities.search(cpp_name);
		break;
	case LISTS:
		std_list = Lists.search(cpp_name);
		break;
	case SCALARS:
		std_list = Scalars.search(cpp_name);
		break;
	case TABLES:
		std_list = Tables.search(cpp_name);
		break;
	case VARIABLES:
		std_list = Variables.search(cpp_name);
		break;
	default:
		break;
	}
	
	if(std_list.size() == 0)
		return list;

	for(const std::string& obj_name: std_list)
		list << QString::fromStdString(obj_name);
	return list;
}
