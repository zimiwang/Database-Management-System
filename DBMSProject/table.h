/*
*	File: 	    table.h
*   Author:     Andrew Nunez
*   Date:       Sept. 23, 2021
*
*   This file holds a class that defines a table and it's data to read and write from a .bin file.
*/
#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "parser.h"

class Table {
public:
	/// The name of the table
	std::string table_name;

	/// The name of the Database
	std::string database_name;

	/// The collection of keys <type, value> of the table
	std::map<std::string, std::string> keys;

	/// The collection of column names and types to a table
	std::map<std::string, std::string> columns;

	/// The collection of arrays of rows for the table.
	std::vector<std::vector<std::string>> rows;

	void Print_Rows(std::vector<std::string> column_names, vector<string> where_clause, string conditional);

	void Insert(std::vector<std::string> row);

	void AddKey(std::string key, std::string value);

	int GetLargestColumnSize();

	void DeleteRow(vector<string> row);

	int get_column_index(std::string column_name);

	// TODO: Add column names
	std::vector<std::vector<std::string> > Select(std::vector<std::string> col_names);

	std::vector<std::string> get_column_names();

	void Delete();

	Table() {

	}

	// Use this as as create in DB CreateTable method
	// TODO: Tie into user input
	Table(std::string name) {
		this->table_name = name;
	}

	/// Author: Janita Aamir
	/// Creates a new instances of a table with columns assigned
	Table(std::string name, vector<string> cols)
		: Table(name) {
		for (std::string col : cols) {
			vector<string> tmp = Utils::split(col, " ");

			//columns.insert(Utils::trim(tmp[0]), Utils::trim(tmp[1]));
			columns.insert(std::pair<std::string, std::string>(Utils::trim(tmp[0]), Utils::trim(tmp[1])));


		}
	}


};

/// Author: Andrew Nunez
/// Gets the size of the largest column
int Table::GetLargestColumnSize() {
	int ret = 0;

	for (std::vector<std::string> row : rows) {
		for (std::string& value : row) {
			if (value.length() > ret) {
				ret = value.length();
			}
		}

	}

	for (auto const& it : columns) {
		if (it.first.length() > ret) {
			ret = it.first.length();
		}

	}

	return ret;

}

/// Author: Andrew Nunez
/// Gets the index of the provided column
int Table::get_column_index(std::string column_name) {
	int ret = -1;
	std::map<std::string, std::string>::iterator it;
	int col_index;

	it = columns.find(column_name);

	if (it != columns.end()) {
		col_index = std::distance(columns.begin(), it);
		ret = col_index;
	}

	return ret;
}

/// Author: Andrew Nunez
/// Prints the rows using the specified where clause and column names
void Table::Print_Rows(std::vector<std::string> column_names, vector<string> where_clause, string conditional) {
	int row_count = 0;

	std::map<std::string, std::string> print_cols;
	std::vector<std::vector<std::string> > print_rows;

	std::vector<int> indices;
	std::map<std::string, std::string>::iterator it;
	int col_index;
	int where_idx = -1;

	if (where_clause.size() > 0) {
		where_idx = this->get_column_index(where_clause[0]);
	}


	if (column_names.size() > 0 && column_names[0] == "*") {
		column_names.clear();

		for (auto const& it : columns) {
			column_names.push_back(it.first);
		}
	}

	for (std::string name : column_names) {
		it = columns.find(name);

		if (it != columns.end()) {
			col_index = std::distance(columns.begin(), it);
			print_cols.insert({ it->first, it->second });
			indices.push_back(col_index);
		}
		else {
			std::cout << "Column Name does not exist: " << name << std::endl;
			return;
		}

	}

	// Where clause goes here
	for (std::vector<std::string> row : rows) {
		std::vector<std::string> new_row;

		for (int i = 0; i < indices.size(); i += 1) {
			new_row.push_back(row[indices[i]]);
		}

		if (where_idx == -1) {
			print_rows.push_back(new_row);
		}
		else if (new_row[where_idx] == where_clause[1]) {
			print_rows.push_back(new_row);
		}

	}

	columns = print_cols;
	rows = print_rows;

	int col_char_count = GetLargestColumnSize();
	int row_char_count = (columns.size() * (col_char_count + 3)) + 1;

	std::cout << "| ";

	for (std::string name : column_names) {
		std::cout << name;

		for (int i = 0; i < col_char_count - name.length(); i += 1) {
			std::cout << " ";
		}

		std::cout << " | ";
	}

	std::cout << std::endl;

	for (int i = 0; i < row_char_count; i += 1) {
		std::cout << "=";
	}

	std::cout << std::endl;

	for (std::vector<std::string> row : print_rows) {
		std::cout << "| ";

		for (std::string& value : row) {
			std::cout << value;

			for (int i = 0; i < col_char_count - value.length(); i += 1) {
				std::cout << " ";
			}

			std::cout << " | ";

		}

		std::cout << std::endl;

		for (int i = 0; i < row_char_count; i += 1) {
			std::cout << "-";
		}

		std::cout << std::endl;

		row_count += 1;
	}

	std::cout << row_count << " rows selected." << std::endl;

}

/// Author: Andrew Nunez
/// Adds a key to the table
void Table::AddKey(std::string key, std::string value) {
	std::cout << "Adding: " << key << " " << value << std::endl;

	keys.insert(std::pair<std::string, std::string>(key, value));

}

/// Author: Andrew Nunez
// Inserts a row vector to the table
void Table::Insert(std::vector<std::string> row) {
	rows.push_back(row);
}

// Selects the columns from the table
std::vector<std::vector<std::string> > Table::Select(std::vector<std::string> col_names) {
	return rows;
}


// Deletes the table
void Table::Delete() {
	delete this;
}

std::vector<std::string> Table::get_column_names() {
	std::vector<std::string> temp;
	auto iter = columns.begin();

	while (iter != columns.end()) {
		temp.push_back(Utils::trim(iter->first));
		iter++;
	}

	return temp;
}

void Table::DeleteRow(vector<string> row) {
	rows.erase(std::remove(rows.begin(), rows.end(), row), rows.end());
}
#pragma once