#pragma once

#include "table.h"


// ================ TABLE ================

Table::Table(const int nb_columns) : TBL(nb_columns) {}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& vars, 
	bool mode, bool files, bool date): TBL(nb_columns)
{
	char* c_def = to_char_array(def);

	char** c_lecs = NULL;
	if(vars.size() > 0)
	{
		std::string vars_list;
		for(const std::string& var : vars) 
			vars_list += var + ';';
		c_lecs = B_ainit_chk(to_char_array(vars_list), NULL, 0);
	}

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	T_auto(this, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const int nb_columns, const std::string& def, const std::vector<std::string>& titles, 
		     const std::vector<std::string>& lecs, bool mode, bool files, bool date): TBL(nb_columns)
{
	char* c_def = to_char_array(def);

	if(titles.size() != lecs.size())
		throw std::invalid_argument(std::string("Table: the list of title lines and the list of LEC ") + 
			"expressions must have the same size.\n" + 
			"Size of the list of title lines: " + std::to_string(titles.size()) + "\n" +
			"Size of the list of LEC expressions: " + std::to_string(lecs.size()));

	char** c_titles = vector_to_double_char(titles);
	char** c_lecs = vector_to_double_char(lecs);

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	T_default(this, c_def, c_titles, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_titles);
	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const int nb_columns, const std::string& def, const std::string& lecs, 
	bool mode, bool files, bool date): TBL(nb_columns)
{
	char* c_def = to_char_array(def);

	char** c_lecs = NULL;
	if(!lecs.empty())
	{
		char* pattern = to_char_array(lecs);
		// Retrieves all variable names matching one or more patterns in KV_WS (similar to grep)
		char* lst = K_expand(VARIABLES, NULL, pattern, '*');
		// Parses a string and replaces @filename and $listname by their contents
		c_lecs = B_ainit_chk(lst, NULL, 0);
		SCR_free(lst);
	}

	int c_mode = mode ? 1 : 0;
	int c_files = files ? 1 : 0;
	int c_date = date ? 1 : 0;

	T_auto(this, c_def, c_lecs, c_mode, c_files, c_date);

	SCR_free_tbl((unsigned char**) c_lecs);
}

Table::Table(const TBL* c_table): TBL(*c_table) {}

Table::Table(const Table& table): TBL((TBL&) table) {}
