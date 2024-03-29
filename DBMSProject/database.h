/*
*	File: 	    database.h
*   Author:     Andrew Nunez, Janita Aamir, Saurav Gautam, Sanjeev Thakur
*   Date:       Sept. 23, 2021
*
*   This file holds a class that defines a database and it's data to read and write from a .bin file.
*/
#pragma once
#include <algorithm>
#include "filehelper.h"
//#include "table.h"
#include "bplustree.h"
#include <stdbool.h>
#include "row.h"
#include <cstring>
#include<iostream>
#include<string>
#include "bminustree.h"
#include "dbtable.h"
using namespace std;

class Database
{
private:
	void Read(std::string db_name);

public:
	
	/// <summary>
	/// The name of the database 
	/// </summary>
	std::string database_name;

	/// <summary>
	/// The tables associated to <database_name>
	/// </summary>
	std::vector<Table> tables;
	std::vector<Table> join_tables;
	std::vector<BTree<string>> secondaryStringTrees;
	std::vector<BTree<int>> secondaryIntTrees;
	std::vector<BTree<char*>> secondaryCharTrees;
	std::vector<BPTree> primary_key_trees;

	const string PRIMARY_KEY = "ID";

	// ***Unreachable
	void CreateTable(std::string tableName);

	static void List();
	void List_Tables();
	void AddTable(Table& tbl);
	void Delete();
	void Save();
	void SaveTable(Table table);
	void DropTable(std::string name);
	bool find_table(std::string name);
	void insert_into(std::string statement, std::string table_name);
	void List_Info();
	Table get_table(std::string tbl_name);
	Table join_table(std::string src_table, std::string dest_table, std::string foreign_key, std::string exkey);
	BPTree get_tree(string name);
	void UpdateTable(string table_name, vector<vector<string>> update_clause, vector<string> where_clause);
	void RenameTable(std::string old_table_name, std::string new_table_name);
	void RenameColumn(std::string old_column_name, std::string new_column_name, std::string table_name);
	void DeleteFrom(std::string tbl_name, std::string conditional, vector<string> clause);
	void delete_column(std::string column_name, std::string table_name);
	void keytotable(std::string keytype, std::string keyname, std::string table_name);
	void sortKeys();
	void updateRows();
	void updateSecondaryTrees();
	void newPrimaryTreeUpdate();
	float sumRows(std::string table, std::string column);
	int numberOfIntColumns(std::string sourceTable, std::string columnName);

	//if you change this you should probably change the error message below in updateRows
	const int DEFAULT_CHAR_ARRAY_SIZE = 15;

	
	/// <summary>
	/// constructor for empty database
	/// </summary>
	Database()
	{}

	
	/// <summary>
	/// Database contructor
	/// </summary>
	/// <param name="name">name for the new database</param>
	Database(std::string name)
	{
		Read(name);
	}
};


/// <summary>
/// delete the current database
/// </summary>
/// TODO: delete the associated .db file
void Database::Delete()
{
	// remove file
	delete this;
}

///////////////////////////////////////////////////////////////////////////////// this to be replaced by serialization
///// Author: Andrew Nunez
///// <summary>
///// Save the state of the current database to disk
///// </summary>
//void Database::Save()
//{
//	std::string line;
//	std::string contents;
//	std::ofstream out("data/" + database_name + ".db");
//
//	contents = "database:" + database_name + "\n";
//
//	auto table = tables.begin();
//
//	for (; table != tables.end(); table++)
//	{
//		contents += ("table_name:" + table->table_name);
//
//		// Add keys
//		if (table->keys.size() > 0)
//		{
//			contents += "\nkeys:";
//			for (auto const& key : table->keys)
//			{
//				contents += (key.first + " " + key.second + ",");
//			}
//
//			contents.pop_back();
//		}
//
//		// Add Columns
//		if (table->columns.size() > 0)
//		{
//			contents += "\ncolumns:";
//			for (auto const& column : table->columns)
//			{
//				contents += (column.first + " " + column.second + ",");
//			}
//
//			contents.pop_back();
//		}
//
//		// Add Rows
//		for (auto& row : table->rows)
//		{
//			contents += "\nrow:";
//
//			for (auto& val : row)
//			{
//				contents += (val + ",");
//			}
//
//			contents.pop_back();
//		}
//
//		contents += "\n;\n";
//	}
//
//	out << contents;insert
//	out.close();
//
//	updateRows();
//	sortKeys();
//	updateSecondaryTrees();
//	newPrimaryTreeUpdate();
//
//}

/// Author: Andrew Nunez
/// <summary>
/// Save the state of the current database to disk
/// </summary>
void Database::Save()
{
	std::string line;
	std::string contents;
	std::ofstream out("data/" + database_name + ".db");

	contents = "database:" + database_name + "\n";

	auto table = tables.begin();

	for (; table != tables.end(); table++)
	{
		contents += ("table_name:" + table->table_name);

		// Add keys
		if (table->allKeys.size() > 0)
		{
			contents += "\nkeys:";
			for (Keys key : table->allKeys)
			{
				contents += (key.keyName + " " + key.type + ",");
			}

			contents.pop_back();
		}

		// Add Columns
		if (table->columns.size() > 0)
		{
			contents += "\ncolumns:";
			for (auto const& column : table->columns)
			{
				contents += (column.first + " " + column.second + ",");
			}

			contents.pop_back();
		}

		// Add Rows
		for (Row row : table->rows)
		{
			contents += "\nrow:";

			/*for (auto& val : row)
			{
				contents += (val + ",");
			}*/

			contents.pop_back();
		}

		contents += "\n;\n";
	}

	out << contents;
	out.close();

	updateRows();
	sortKeys();
	updateSecondaryTrees();
	newPrimaryTreeUpdate();

}

// TODO: Accept a list of columns, tie into user input. This might change to accepting a table name and a list of columns and creating a Table constructor. That may be the cleanest way

/// <summary>
/// Add a table to the associated database
/// </summary>
/// <param name="tbl">reference to the table to add</param>
void Database::AddTable(Table& tbl)
{
	tables.push_back(tbl);

	this->Save();
}


/// <summary>
/// For all tables in the database, if the table name matches, erase it from the file and save its new state
/// </summary>
/// <param name="name">name of table to remove</param>
void Database::DropTable(std::string name)
{
	int count = 0;
	for (auto& it : tables)
	{
		if (it.table_name == name)
		{
			tables.erase(tables.begin() + count);

			this->Save();
		}
		count = count + 1;
	}
}

/// TO BE UPDATED
/// 
/// 
/// 
/// <summary>
/// with an inputted name and update clause or multiple update clauses, 
/// the table is updated by creating a new table and deleting the old one, then saving it.
/// </summary>
/// <param name="table_name"></param>
/// <param name="update_clause"></param>
/// <param name="where_clause"></param/
/*
void Database::UpdateTable(string table_name, vector<vector<string>> update_clause, vector<string> where_clause) {
	Table tbl = this->get_table(table_name);

	for (size_t i = 0; i < update_clause.size(); i++)
	{
		vector<vector<string>> new_rows;
		//cout << "get_where_clause: " << where_clause[0] << endl;
		//cout << "get_update_clause: " << update_clause[0][i] << endl;
+-
		int where_idx = tbl.get_column_index(where_clause[0]);
		int update_idx = tbl.get_column_index(update_clause[i][0]);

		cout << "Where idx: " << where_idx << endl;
		cout << "Update idx: " << update_idx << endl;

		if (where_idx == -1 || update_idx == -1) {
			std::cout << "The column name is incorrect" << std::endl;
			break;
		}

		else
		{
			for (vector<string> row : tbl.rows) {
				if (row[where_idx] == where_clause[1]) {

					//cout << "get_update_clause: " << update_clause[i][1] << endl;

					row[update_idx] = update_clause[i][1];
				}
				new_rows.push_back(row);
			}

			tbl.rows = new_rows;

			this->DropTable(table_name);
			this->AddTable(tbl);

		}

	}
		this->Save();
}

////////////////////////////////////////////////////////////////////////////will be depreciated by serialization
///// Author: Andrew Nunez
///// <summary>
///// Read the given file name 
///// </summary>
///// <param name="db_name">name of the database to read</param>
//void Database::Read(std::string db_name)
//{
//	int tmp_size, i = 0;
//	std::string line;
//	std::string* tmp_parent_array;
//	std::string* tmp_child_array;
//	std::ifstream file("data/" + db_name + ".db");
//
//	std::string tbl_name;
//	std::map<std::string, std::string> keys;
//	std::vector<std::vector<std::string>> rows;
//	std::map<std::string, std::string> columns;
//
//	if (file.is_open())
//	{
//		while (getline(file, line))
//		{
//			tmp_size = std::count(line.begin(), line.end(), ',') + 1;
//
//			// Begin parsing each line
//			if (line.find("database:") == 0)
//			{
//				database_name = line.substr(line.find(":") + 1);
//			}
//			else if (line.find("table_name:") == 0)
//			{
//				tbl_name = line.substr(line.find(":") + 1);
//			}
//			else if (line.find("row:") == 0)
//			{
//				std::vector<std::string> tmp_v;
//				tmp_parent_array = Parser::split_str(line, ',');
//
//				for (i = 0; i < tmp_size; i++)
//				{
//					tmp_v.push_back(tmp_parent_array[i]);
//				}
//
//				rows.push_back(tmp_v);
//			}
//			else if (line.find("keys:") == 0)
//			{
//				tmp_parent_array = Parser::split_str(line, ',');
//				for (i = 0; i < tmp_size; i++)
//				{
//					tmp_child_array = Parser::split_str(tmp_parent_array[i], ' ');
//					keys.insert({ tmp_child_array[0], tmp_child_array[1] });
//				}
//
//			}
//			else if (line.find("columns:") == 0)
//			{
//				tmp_parent_array = Parser::split_str(line, ',');
//
//				for (i = 0; i < tmp_size; i++)
//				{
//					tmp_child_array = Parser::split_str(tmp_parent_array[i], ' ');
//
//					if (line.find("keys:") == 0)
//					{
//						keys.insert({ tmp_child_array[0], tmp_child_array[1] });
//					}
//					else if (line.find("columns:") == 0)
//					{
//						columns.insert({ tmp_child_array[0], tmp_child_array[1] });
//					}
//				}
//			}
//			/*else if (line.find(",") != std::string::npos)
//			{
//				tmp_parent_array = Parser::split_str(line, ',');
//
//				for (i = 0; i < tmp_size; i++)
//				{
//					tmp_child_array = Parser::split_str(tmp_parent_array[i], ' ');
//
//					if (line.find("keys:") == 0)
//					{
//						keys.insert({ tmp_child_array[0], tmp_child_array[1] });
//					}
//					else if (line.find("columns:") == 0)
//					{
//						columns.insert({ tmp_child_array[0], tmp_child_array[1] });
//					}
//				}
//			}*/
//			else if (line.find(";") == 0)
//			{
//				Table* tbl = new Table(tbl_name);
//				tbl->keys = keys;
//				tbl->rows = rows;
//				tbl->columns = columns;
//				//tbl->spread_keys();
//				this->AddTable(*tbl);
//
//				rows.clear();
//				columns.clear();
//				keys.clear();
//
//			}
//			else
//			{
//				std::cout << "Database is Corrupt!" << std::endl;
//			}
//		}
//		file.close();
//	}
//	else
//	{
//		std::cout << "Database does not exist!" << std::endl;
//	}
//}

/// TO BE UPDATED
/// 
/// 
/// 
/// Author: Andrew Nunez
/// <summary>
/// Read the given file name 
/// </summary>
/// <param name="db_name">name of the database to read</param>
void Database::Read(std::string db_name)
{
	int tmp_size, i = 0;
	std::string line;
	std::string* tmp_parent_array;
	std::string* tmp_child_array;
	std::ifstream file("data/" + db_name + ".db");

	std::string tbl_name;
	std::map<std::string, std::string> keys;
	std::vector<std::vector<std::string>> rows;
	std::map<std::string, std::string> columns;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			tmp_size = std::count(line.begin(), line.end(), ',') + 1;

			// Begin parsing each line
			if (line.find("database:") == 0)
			{
				database_name = line.substr(line.find(":") + 1);
			}
			else if (line.find("table_name:") == 0)
			{
				tbl_name = line.substr(line.find(":") + 1);
			}
			else if (line.find("row:") == 0)
			{
				std::vector<std::string> tmp_v;
				tmp_parent_array = Parser::split_str(line, ',');

				for (i = 0; i < tmp_size; i++)
				{
					tmp_v.push_back(tmp_parent_array[i]);
				}

				rows.push_back(tmp_v);
			}
			else if (line.find("keys:") == 0)
			{
				tmp_parent_array = Parser::split_str(line, ',');
				for (i = 0; i < tmp_size; i++)
				{
					tmp_child_array = Parser::split_str(tmp_parent_array[i], ' ');
					keys.insert({ tmp_child_array[0], tmp_child_array[1] });
				}

			}
			else if (line.find("columns:") == 0)
			{
				tmp_parent_array = Parser::split_str(line, ',');

				for (i = 0; i < tmp_size; i++)
				{
					tmp_child_array = Parser::split_str(tmp_parent_array[i], ' ');

					if (line.find("keys:") == 0)
					{
						keys.insert({ tmp_child_array[0], tmp_child_array[1] });
					}
					else if (line.find("columns:") == 0)
					{
						columns.insert({ tmp_child_array[0], tmp_child_array[1] });
					}
				}
			}
			/*else if (line.find(",") != std::string::npos)
			{
				tmp_parent_array = Parser::split_str(line, ',');

				for (i = 0; i < tmp_size; i++)
				{
					tmp_child_array = Parser::split_str(tmp_parent_array[i], ' ');

					if (line.find("keys:") == 0)
					{
						keys.insert({ tmp_child_array[0], tmp_child_array[1] });
					}
					else if (line.find("columns:") == 0)
					{
						columns.insert({ tmp_child_array[0], tmp_child_array[1] });
					}
				}
			}*/
			//else if (line.find(";") == 0)
			//{
			//	Table* tbl = new Table(tbl_name);
			//	tbl->keys = keys;
			//	tbl->rows = rows;
			//	tbl->columns = columns;
			//	//tbl->spread_keys();
			//	this->AddTable(*tbl);

			//	rows.clear();
			//	columns.clear();
			//	keys.clear();

			//}
			else
			{
				std::cout << "Database is Corrupt!" << std::endl;
			}
		}
		file.close();
	}
	else
	{
		std::cout << "Database does not exist!" << std::endl;
	}
}

/// list the "data" files in the db
void Database::List()
{
	FileHelper::listfiles("data", "db");
}

/// list the tables
void Database::List_Tables()
{
	for (Table tbl : tables)
	{
		if (tbl.table_name == "exT") cout << "exT table found\n";
		std::cout << tbl.table_name << std::endl;
	}
}

/// return a boolean; true if the a table is found, false otherwise
bool Database::find_table(std::string name)
{
	for (Table tbl : tables)
	{

		if (name == tbl.table_name)
		{
			return true;
		}
		else
			return false;
	}
}


/// return the table reference with a particular name
Table Database::get_table(std::string name)
{
	Table ret;

	for (Table tbl : tables)
	{
		if (tbl.table_name == name)
		{
			ret = tbl;

			break;
		}
	}

	return ret;
}



/// <summary>
/// return the sum of the column
/// </summary>
/// <param name="table">table name to use</param>
/// <param name="column">name of which column to add</param>
/// <returns>float of the sum</returns>
/*
float Database::sumRows(std::string fromTable, std::string column)
{
	float sum = 0;
	//cout << "runing sumROws\n" << "from table: \""<<fromTable<<"\"\t column: \"" << column << "\"\n";

	//get the table information (table, column, rows)
	Table table = this->get_table(fromTable);
	int columnIndex = table.get_column_index(column);
	//std::vector<std::vector<std::string>> rows = table.rows;
	vector<Row> rows = table.rows;
	int columnType = rows[0].GetColumnType(column);
	
	// iterate through the rows
	for (Row row : rows) {
		try {			
			string value;
			if (columnType == 0) {
				value = row.GetStringColumnByName(column).GetValue();
			}
			else if (columnType == 1) {
				value = to_string(row.GetIntColumnByName(column).GetValue());
			}
			else {
				value = row.GetCharColumnByName(column).GetValue();
			}

			sum += stoi(value);
		}
		catch (const exception& e) {
			sum += 0;
		}

		return sum;
	}



	// iterate through the rows
	//for (int i = 0; i < rows.size(); i++)
	//{
	//	try {
	//		string value = string(rows[i]);
	//		sum += stoi(value);
	//	}
	//	// this catch should run if the row is not a string that can be converted into an int (! 0-9)
	//	catch(const std::exception& e)
	//	{
	//		sum += 0; 
	//	}
	//}

	//return sum;
}
*/



/// <summary>
/// give the number of columns of a specific table that are integers
/// </summary>
/// <param name="sourceTable">string of the table to read from</param>
/// <param name="columnName">string of the column name</param>
/// <returns>integer value of the number of int columns</returns>
/*
int Database::numberOfIntColumns(std::string sourceTable, std::string columnName)
{
	int rowsNumber = 0;

	//get the table information (table, column, rows)
	Table table = this->get_table(sourceTable);
	int columnIndex = table.get_column_index(columnName);
	//std::vector<std::vector<std::string>> rows = table.rows;
	vector<Row> rows = table.rows;
	int columnType = rows[0].GetColumnType(columnName);

	// iterate through the rows
	for (Row row : rows) {
		try {
			string value;
			if (columnType == 0) {
				value = row.GetStringColumnByName(columnName).GetValue();
			}
			else if (columnType == 1) {
				value = to_string(row.GetIntColumnByName(columnName).GetValue());
			}
			else {
				value = row.GetCharColumnByName(columnName).GetValue();
			}

			stoi(value);
			rowsNumber++;
		}
		// this catch should run if the row is not a string that can be converted into an int (! 0-9)
		catch (const exception& e) {
			rowsNumber += 0;
		}
	}

	//for (int i = 0; i < rows.size(); i++)
	//{
	//	try {
	//		std::string value = std::string(rows[i][columnIndex]);
	//		stoi(value);
	//		rowsNumber++;
	//	}
	//	// this catch should run if the row is not a string that can be converted into an int (! 0-9)
	//	catch (const std::exception& e)
	//	{
	//		rowsNumber += 0;
	//	}
	//}

	return rowsNumber;
}
*/



/// <summary>
/// Joins two tables based on a foreign key
/// </summary>
/// <param name="src_table">table that has the foreign key</param>
/// <param name="dest_table">table who's primary key is the foreign key</param>
/// <param name="foreign_key">which column of table 1 is the foreign key</param>
/// <returns></returns>
/*
Table Database::join_table(std::string src_table, std::string dest_table, std::string foreign_key, std::string exkey)
{
	Table join;
	Table src = this->get_table(src_table);
	Table dest = this->get_table(dest_table);
	join.table_name = src_table + "_" + dest_table;
	int colindex = src.get_column_index(foreign_key);
	int col2index = dest.get_column_index(exkey);
	int rowcount = 1;
	if (colindex != -1 && col2index != -1)
	{
		//colindex shouldn't ever be -1 but from here we loop through the rows and match local foreign key to foreign 
		//primary key --- TO NOTE | CURRENTLY CAN ONLY JOIN ON INT COLUMNS
		for (Row r : src.rows)
		{
			
			for (Column<int> col : r.intColumn)
			{	
				if (col.GetName() == foreign_key) //this should pass exactly once per row
				{
					//we have the local row value, search through the primary index of the destination tree for the matching row
					Row foreignRow = dest.primaryTree.search(col.GetValue());
					Row destrow = Row();  //empty by default
					bool keeprow = false;
					for (Row r2 : dest.rows)
					{
						for (Column<int> col2 : r2.intColumn)
						{
							if (col2.GetName() == exkey)
							{
								if (col.GetValue() == col2.GetValue())
								{
									destrow = r2;
									//combine the two table's row structure
									Row combinedRow = Row(r); //i believe this is how you do a deep copy in C++
									for (Column<int> loc : destrow.intColumn)
									{
										combinedRow.intColumn.push_back(loc);
									}
									for (Column<string> loc : destrow.strColumn)
									{
										combinedRow.strColumn.push_back(loc);
									}
									for (Column<char *> loc : destrow.charColumn)
									{
										combinedRow.charColumn.push_back(loc);
									}

									//push our merged row into the new table
									Column<int> newindex;
									newindex.SetName("ID_" + join.table_name);
									newindex.AddValue(rowcount);
									rowcount++;
									combinedRow.intColumn.push_back(newindex);
									join.rows.push_back(combinedRow);
								}
							}
						}
					}
					
				}
			}
		}
	}
	join.primaryKeyName = "ID_" + join.table_name;
	this->join_tables.push_back(join);
	return join;
}
*/

BPTree Database::get_tree(string name) {
	BPTree ret;

	for (BPTree tree : primary_key_trees) {
		if (tree.Name == name) {
			ret = tree;
			break;
		}
	}

	return ret;
}


/// Author: Saurav Gautam
/// Save the table after being updated in the console
/// Replaces the table by creating a new one
void Database::SaveTable(Table table)
{

	std::string tableName = table.table_name;
	int count = 0;

	for (Table tbl : tables)
	{
		if (tbl.table_name == tableName)
		{
			break;
		}
		count = count + 1;
	}

	tables[count] = table;
}

/// <summary>
/// Rename an existing table
/// </summary>
/// <param name="old_table_name"></param>
/// <param name="new_table_name"></param>
void Database::RenameTable(std::string old_table_name, std::string new_table_name)
{

	Table tbl = this->get_table(old_table_name);

	tbl.table_name = new_table_name;

	this->DropTable(old_table_name);
	this->AddTable(tbl);

	this->Save();
}

/// NEEDS TO BE UPDATED
/// 
/// 
/// 
/// <summary>
/// 
/// </summary>
/// <param name="tbl_name"></param>
/// <param name="conditional"></param>
/// <param name="clause"></param>
void Database::DeleteFrom(std::string tbl_name, std::string conditional, vector<string> clause) {

	int count = 0;
	string value = clause[1];

	Table currentTable = this->get_table(tbl_name);
	int col_ndx = currentTable.get_column_index(clause[0]);
	//vector<vector<string> > rows = currentTable.rows;
	vector<Row> rows = currentTable.rows;

	for (Row row : rows) {

		if (col_ndx != -1)
		{
			//cout << row[col_ndx] << conditional << value << endl;

		/*	if (conditional == "=") {
				if (row[col_ndx] == value)
				{
					currentTable.DeleteRow(row);
					count += 1;
				}
			}
			else if (conditional == ">=") {
				if (row[col_ndx] >= value)
				{
					currentTable.DeleteRow(row);
					count += 1;
				}
			}
			else if (conditional == "<=") {
				if (row[col_ndx] <= value)
				{
					currentTable.DeleteRow(row);
					count += 1;
				}
			}
			else if (conditional == ">") {
				if (row[col_ndx] > value)
				{
					currentTable.DeleteRow(row);
					count += 1;
				}
			}
			else if (conditional == "<") {
				if (row[col_ndx] < value)
				{
					currentTable.DeleteRow(row);
					count += 1;
				}
			}
			else if (conditional == "!=") {
				if (row[col_ndx] != value)
				{
					currentTable.DeleteRow(row);
					count += 1;
				}
			}
			else {
				std::cout << "Given conditional statement is not supported!" << std::endl;
			}*/


		}
		else {
			std::cout << count << "Incorrect column name" << endl;
			break;
		}
	}
	SaveTable(currentTable);
}
*/


/// <summary>
/// Rename a column in an existing table
/// </summary>
/// <param name="old_column_name"></param>
/// <param name="new_column_name"></param>
/// <param name="table_name"></param>
/*
void Database::RenameColumn(std::string old_column_name, std::string new_column_name, std::string table_name)
{
	Table tbl = this->get_table(table_name);
	tbl.RenameColumn(old_column_name, new_column_name);

	/*std::map<std::string, std::string> new_columns;

	new_columns = tbl.Rename_column(new_column_name, old_column_name);
	tbl.columns = new_columns;

	this->DropTable(table_name);
	this->AddTable(tbl);

	this->Save();*/
}
*/


/// list the tables and print their information
/*
void Database::List_Info() {
	int size = 0;
	std::cout << "Database Name:    	" << database_name << endl;
	std::cout << "Number of Tables: 	" << tables.size() << endl;
	std::cout << "==========================" << endl;
	/*for (Table tbl : tables) {
		cout << ">    " << tbl.table_name << endl;

		for (vector<string> row : tbl.rows) {
			for (string str : row) {
				size += str.length();
			}
		}

	}
	std::cout << "==========================" << endl;

	std::cout << "Total Size: " << size << " bytes" << endl;*/
}
*/



/// <summary>
/// Read the insert statement and insert values in the table
/// </summary>
/// <param name="statement">user-inputted command</param>
/// <param name="table_name">name of the table</param>
void Database::insert_into(std::string statement, std::string table_name)
{
	Table current_table = get_table(table_name);;
	vector<string> columns = Parser::get_insert_columns(statement, table_name);
	vector<vector<string> > values = Parser::get_insert_rows(statement, table_name);

	//for (int i = 0; i < columns.size(); i++) cout << "column:" << columns[i] << "\n";

	//check to see if ID is present in the columns
	if (std::find(columns.begin(), columns.end(), ("ID_" + table_name)) != columns.end())
	{
		//no worries, the user SHOULD be setting their own ID
	}
	else
	{
		//manually add ID to columns, then a basic iterator representing row number to the values
		columns.push_back(("ID_" + table_name));
		string newid = std::to_string(current_table.rows.size() + 1);
		
		values[0].push_back(newid);
	}

	map<string, string> tabcols = current_table.columns;
	
	Row r = Row();
	std::map<string, string> merge;
	std::transform(columns.begin(), columns.end(), values.begin(), std::inserter(merge, merge.end()), std::make_pair<string const&, string const&>);
	for (string c : columns)
	{
		std::map<string, string>::iterator it = tabcols.find(c);
		if (it != tabcols.end())
		{
			string coltype = it->second;

			if (coltype == "int")
			{
				Column<int> column = new Column<int>();
				std::map<string, string>::iterator valit = merge.find(c);
				column.SetName(c);
				column.AddValue(stoi(valit->second));
				r.intColumn.push_back(column);
			}
			else if (coltype == "char")
			{
				std::map<string, string>::iterator valit = merge.find(c);
				//check for declared length of char array
					int size = -1;
					try 
					{
						size = stoi(Utils::get_string_between_two_strings(valit->first, "[", "]"));
					}
					catch(exception &err)
					{
						//catch here and just use the default length
						size = DEFAULT_CHAR_ARRAY_SIZE;
						//tell the dummy that they didn't provide a char array length and the default is being used instead
						string mes = "ERROR. NO CHAR ARRAY SIZE LIMIT FOUND FOR COLUMN " + valit->first + ". USING SYSTEM DEFAULT OF 15.";
						std::cerr << mes << endl;
					}

					//initialize the character array
					char * char_arr = new char[size];
					memset(char_arr, ' ', size);
					string str_obj(valit->second);

					//copy the string making sure to terminate it regardless of the length of the string provided
					copy(str_obj.begin(), str_obj.end(), char_arr);
					char_arr[size-1] = '\0';

					//create the column and push it to the row
					Column<char *> newcol = Column<char *>();
					newcol.AddValue(char_arr);
					newcol.SetName(valit->first);
					r.charColumn.push_back(newcol);
			}
			else if (coltype == "string")
			{
				Column<string> column = new Column<string>();
				std::map<string, string>::iterator valit = merge.find(c);
				column.SetName(c);
				column.AddValue(valit->second);
				r.strColumn.push_back(column);
			}
			else
			{
				//unsupported coltype found - shouldn't ever happen
			}
		}
		else
		{
			//col provided not present within the table - user goofed up 
		}
	}

}

/// <summary>
/// Deletes a column from the database
/// </summary>
/// <param name="column_name"></param>
/// <param name="table_name"></param>
/*
void Database::delete_column(std::string column_name, std::string table_name)
{
	//get the index of the column name
	Table current_table = get_table(table_name);;

	current_table.DeleteColumn(column_name);
	
	//SaveTable(current_table);
	//updateRows();
	//updateSecondaryTrees();
	//newPrimaryTreeUpdate();
	
	
}
*/



/// <summary>
/// adds a key to the old key storage from the table
/// </summary>
/// <param name="keytype"></param>
/// <param name="keyname"></param>
/// <param name="table_name"></param>
void Database::keytotable(std::string keytype, std::string keyname, std::string table_name)
{
	//get the index of the column name
	Table current_table = get_table(table_name);;
	
	current_table.AddKey(keytype, keyname);

	SaveTable(current_table);
}

/// <summary>
/// sorts keys into the lists they belong in. Something to note: Since we only have 1 primary key, it will be set to the most recently specified primary
/// </summary>
/*
void Database::sortKeys()
{
	//for (Table tbl : tables)
	//{
	//	tbl.secondaryKeys.clear();
	//	tbl.foreignKeys.clear();
	//	for (std::pair<std::string, std::string> current_key : tbl.keys)
	//	{
	//		if (current_key.first == "primary")
	//		{
	//			//found primary key
	//			tbl.primaryKeyName = current_key.second;
	//		}
	//		else if (current_key.first == "secondary")
	//		{
	//			//found secondary key
	//			tbl.secondaryKeys.push_back(current_key.second);
	//		}
	//		else if (current_key.first == "foreign")
	//		{
	//			//foreign key found
	//			tbl.foreignKeys.push_back(current_key.second);

	//		}

	//	}
	//	SaveTable(tbl);
	//}
}


/// <summary>
/// Updates the new row data structure from each tree based on the old row storage methodology.
/// </summary>
void Database::updateRows()
{
	//for (Table tbl : tables)
	//{
	//	tbl.newrows.clear();
	//	for (std::vector<std::string> rw : tbl.rows)
	//	{
	//		Row nrow = Row();
	//		int rowfind = 0;
	//		int introws = 0;
	//		int strrows = 0;
	//		int charrows = 0;

	//		for (std::pair<std::string, std::string> col : tbl.columns)
	//		{
	//			if (col.second == "string")
	//			{
	//				Column<string> newcol = Column<string>();
	//				newcol.AddValue(rw[rowfind]);
	//				newcol.SetName(col.first);
	//				nrow.strColumn.push_back(newcol);
	//				
	//			}
	//			else if (col.second == "int")
	//			{
	//				Column<int> newcol = Column<int>();
	//				newcol.AddValue(stoi(rw[rowfind]));
	//				newcol.SetName(col.first);
	//				nrow.intColumn.push_back(newcol);

	//			}
	//			else if (col.second == "char")
	//			{
	//				//check for declared length of char array
	//				int size = -1;
	//				try 
	//				{
	//					size = stoi(Utils::get_string_between_two_strings(col.first, "[", "]"));
	//				}
	//				catch(exception &err)
	//				{
	//					//catch here and just use the default length
	//					size = DEFAULT_CHAR_ARRAY_SIZE;
	//					//tell the dummy that they didn't provide a char array length and the default is being used instead
	//					string mes = "ERROR. NO CHAR ARRAY SIZE LIMIT FOUND FOR COLUMN " + col.first + ". USING SYSTEM DEFAULT OF 15.";
	//					std::cerr << mes << endl;
	//				}

	//				//initialize the character array
	//				char * char_arr = new char[size];
	//				memset(char_arr, ' ', size);
	//				string str_obj(rw[rowfind]);

	//				//copy the string making sure to terminate it regardless of the length of the string provided
	//				copy(str_obj.begin(), str_obj.end(), char_arr);
	//				char_arr[size-1] = '\0';

	//				//create the column and push it to the row
	//				Column<char *> newcol = Column<char *>();
	//				newcol.AddValue(char_arr);
	//				newcol.SetName(col.first);
	//				nrow.charColumn.push_back(newcol);
	//			}
	//			else
	//			{
	//				//unsupported column type - assume string? - come back to this
	//				Column<string> newcol = Column<string>();
	//				newcol.AddValue(rw[rowfind]);
	//				newcol.SetName(col.first);
	//				nrow.strColumn.push_back(newcol);
	//			}
	//			rowfind = rowfind + 1;
	//		}
	//		tbl.newrows.push_back(nrow);
	//	}		
	/*SaveTable(tbl);
	}*/
}
*/



/// <summary>
/// Updates the primary key trees for each table. TODO - Add a check to see if the table needs to be updated - maybe a bool flag in
/// table.h that says whether or not it has been altered since the last table was generated.
/// </summary>
	//for (Table tbl : tables)
	//{
	//	BTree<int> secondaryIntKeyTree;
	//	BTree<char *> secondaryCharKeyTree;
	//	BTree<string> secondaryStringKeyTree;

	//	for (Row r : tbl.newrows)
	//	{
	//		/*Row* rpoint = &r;*/
	//		r.InUse();
	//		for (Column<int> c : r.intColumn)
	//		{							
	//			// check to see if the colname is a secondary key
	//			if (count(tbl.secondaryKeys.begin(), tbl.secondaryKeys.end(), c.GetName())) {
	//				// add value to secondary tree
	//				secondaryIntKeyTree.insert(c.GetValue(), r);
	//				secondaryIntKeyTree.SetKeyName(c.GetName());
	//			}
	//		}

	//		// check for other secondary keys
	//		if (r.hasSecondaryKeyChar(tbl.secondaryKeys)) {
	//			for (Column<char *> c : r.charColumn)
	//			{				
	//				// check to see if the colname is a secondary key
	//				if (count(tbl.secondaryKeys.begin(), tbl.secondaryKeys.end(), c.GetName())) {
	//					// add value to secondary tree
	//					secondaryCharKeyTree.insert(c.GetValue(), r);
	//					secondaryCharKeyTree.SetKeyName(c.GetName());
	//				}
	//			}
	//		}
	//		if (r.hasSecondaryKeyString(tbl.secondaryKeys)) {
	//			for (Column<string> c : r.strColumn)
	//			{
	//				/*secondaryStringKeyTree.insert(c.GetValue(), r);
	//				secondaryStringKeyTree.SetKeyName(c.GetName());*/
	//				// check to see if the colname is a secondary key
	//				if (count(tbl.secondaryKeys.begin(), tbl.secondaryKeys.end(), c.GetName())) {
	//					// add value to secondary tree
	//					secondaryStringKeyTree.insert(c.GetValue(), r);
	//					secondaryStringKeyTree.SetKeyName(c.GetName());
	//				}
	//			}
	//		}

	//	}
	//	// add the secondarytrees to the arrays if they are not empty
	//	if (!secondaryIntKeyTree.IsEmpty()) {
	//		secondaryIntTrees.push_back(secondaryIntKeyTree);
	//	}
	//	if (!secondaryCharKeyTree.IsEmpty()) {
	//		secondaryCharTrees.push_back(secondaryCharKeyTree);
	//	}
	//	if (!secondaryStringKeyTree.IsEmpty()) {
	//		secondaryStringTrees.push_back(secondaryStringKeyTree);
	//	}

	//	SaveTable(tbl);
	//}
}

/// <summary>
/// Updates Primary Key Trees without destroying the old ones.
/// </summary>
/*
inline void Database::newPrimaryTreeUpdate()
{
	//for (Table t : tables)
	//{
	//	string tablename = t.table_name;
	//	std::vector<BPTree>::iterator it = std::find(primary_key_trees.begin(), primary_key_trees.end(), t.primaryKeyTree);
	//	if (it != primary_key_trees.end())
	//	{
	//		// tree exists, now we need to see if it should be updated
	//		for (Row r : t.newrows)
	//		{
	//			r.InUse();
	//			for (Column<int> c : r.intColumn)
	//			{

	//				//check to see if the colname is the primary key
	//				if (c.GetName() == t.primaryKeyName)
	//				{
	//					int search = primary_key_trees[it - primary_key_trees.begin()].search(c.GetValue()).GetIntColumnByName(c.GetName()).GetValue();
	//					if (search != c.GetValue())
	//					{
	//						// DEBUG MESSAGE cout << "Inserting new found row into tree." << endl;
	//						primary_key_trees[it - primary_key_trees.begin()].insert(c.GetValue(), r);
	//					}
	//				}
	//			}
	//		}
	//		t.primaryKeyTree = primary_key_trees[it - primary_key_trees.begin()];
	//	}
	//	else
	//	{
	//		//tree does not exist, create one, populate it, then push it to the primary keys
	//		BPTree newtree;
	//		newtree.SetPrimaryKey(t.primaryKeyName);
	//		newtree.Name = t.table_name;
	//		for (Row r : t.newrows)
	//		{
	//			r.InUse();
	//			for (Column<int> c : r.intColumn)
	//			{
	//				//check to see if the colname is the primary key
	//				if (c.GetName() == t.primaryKeyName)
	//				{
	//					newtree.insert(c.GetValue(), r);
	//				}
	//			}
	//		}
	//		t.primaryKeyTree = newtree;
	//		primary_key_trees.push_back(newtree);
	//		this->SaveTable(t);
	//	}
	//}
}
*/