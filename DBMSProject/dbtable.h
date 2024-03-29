#pragma once


#include <string>
#include <vector>
#include "parser.h"
#include "row.h"
#include "bplustree.h"
#include "bminustree.h"
#include <map>

using namespace std;

struct Keys {
	string keyName;
	string type;
};


class Table {
private:

	bool wasCreated = true;

	template <typename T>
	BTree<T> FindSecondaryTree(string name, vector<BTree<T>> trees) {
		
		for (BTree<T> tree : trees) {
			if (tree.GetKeyName() == name) return tree;
		}

		cout << "could not find secondary tree named: " << name << endl;
		BTree<T> emptyTree = new BTree<T>();
		return emptyTree;

	}


	/// <summary>
	/// Assign columns and primary key
	/// </summary>
	/// <param name="name"></param>
	/// <param name="cols"></param>
	void RecordPrimaryKey(string name, vector<string> cols) {
		bool hasID = false;
		bool isCreate = true;

		for (string col : cols) {
			vector<string> tmp = Utils::split(col, " ");

			if (tmp.size() == 2)
			{
				if (tmp[0] == ("ID_" + name))
				{
					hasID = true;
					primaryKey.keyName = "ID_" + name;
					primaryKey.type = Utils::trim(tmp[1]);
				}				
				columns.insert(pair<string, string>(Utils::trim(tmp[0]), Utils::trim(tmp[1])));
				int letstes = 0;
			}
			else
			{
				std::cout << "Invalid format " << std::endl;
				isCreate = false;
				wasCreated = false;
				break;
			}
		}
		if (isCreate == true)
		{
			if (hasID == false) //We have no ID column defined by the user, manually add one
			{
				columns.insert(pair<string, string>(("ID_" + name), "int"));
				primaryKey.keyName = "ID_" + name;
				primaryKey.type = "int";

			}
		}			
	}



	/// <summary>
	/// Creates the secondary trees for each secondary key
	/// </summary>
	void CreateSecondaryTrees() {

		// create trees based on secondary key types
		for (Keys key : secondaryKeys) {
			if (key.type == "char") {
				BTree<char*> tree;
				tree.SetKeyName(key.keyName);
				CreateSecondaryTree(tree);
			}
			else if (key.type == "int") {
				BTree<int> tree;
				tree.SetKeyName(key.keyName);
				CreateSecondaryTree(tree);
			}
			else if (key.type == "string") {
				BTree<string> tree;
				tree.SetKeyName(key.keyName);
				CreateSecondaryTree(tree);
			}
		}

	}

	/// <summary>
	/// Creates a secondary tree and inserts all the rows
	/// </summary>
	/// <typeparam name="T">The type of key value</typeparam>
	/// <param name="tree">The tree to create an insert rows</param>
	template <typename T>
	void CreateSecondaryTree(BTree<T> tree) {
		string name = tree.GetKeyName();	// column name
		for (Row row : rows) {
			row.get
				int type = row.GetColumnType(name);
			if (type == 0) {
				// string
				tree.insert(row.GetStringColumnByName(name).GetValue(), row);
			}
			else if (type == 1) {
				// int 
				tree.insert(row.GetIntColumnByName(name).GetValue(), row);
			}
			else {
				// char
				tree.insert(row.GetCharColumnByName(name).GetValue(), row);
			}
		}
	}

public:	

	// Data to be saved (serialization)
	string table_name;	
	string primaryKeyName = "ID";
	map<string, string> columns;
	vector<Row> rows;
	vector<Keys> foreignKeys;
	vector<Keys> secondaryKeys;
	Keys primaryKey;
	vector<Keys> allKeys;

	// Data that is not saved
	vector<BTree<string>> secondaryStringTrees;
	vector<BTree<char*>> secondaryCharTrees;
	vector<BTree<int>> secondaryIntTrees;
	BPTree primaryTree;

	Table() {

	}

	Table(string name, vector<string> cols) {
		table_name = name;		
		
		RecordPrimaryKey(name, cols);
	}

	/// <summary>
	/// Adds a new row to the table. Updates the Primary and Secondary Keys accordingly
	/// </summary>
	/// <param name="key">The key of the Row</param>
	/// <param name="row">The row</param>
	/// <param name="primaryKey">True if the row is being inserted into the primarykey</param>
	/// <param name="secondaryKey">True if the row is being inserted into the secondarKey</param>
	/// <param name="sKey">The secondary Key of the Row</param>
	/// <param name="secondaryKeyType">The type of key value for the secondaryKey</param>
	template <typename T>
	void AddRow(T key, Row row, bool primaryKey = true, bool secondaryKey = false, T sKey = "", string secondaryName = "", string secondaryKeyType = "int") {
		bool errorFound = false;
		if (primaryKey) {
			primaryTree.insert(key, row.);
			rows.push_back(row);
		}
		if (secondaryKey) {
			if (secondaryKeyType == "int") {
				BTree<int> tree = FindSecondaryTree(secondaryName, secondaryIntTrees);
				if (!tree.IsEmpty()) tree.insert(sKey, row);
				else errorFound = true;
			}
			else if (secondaryKeyType == "string") {
				BTree<string> tree = FindSecondaryTree(secondaryName, secondaryStringTrees);
				if (!tree.IsEmpty()) tree.insert(sKey, row);
				else erroFound = true;
			}
			else {
				BTree<char*> tree = FindSecondaryTree(secondaryName, secondaryCharTree);
				if (!tree.IsEmpty()) tree.insert(sKey, row);
				else errorFound = true;
			}

			if (errorFound) cout << "Error: An error occured while trying to insert a row." << endl;
			else rows.push_back(row);
		}


		if (primaryKey || secondarKey) {
			cout << "Could not update tree. PrimarKey or SecondaryKey not Defined." << endl;
		}
	}

	/// <summary>
	/// Adds a new key to the table
	/// </summary>
	/// <param name="keytype"></param>
	/// <param name="keyname"></param>
	void AddKey(string keytype, string keyname) {
		Keys newKey;
		newKey.keyName = keyname;
		newKey.type = keytype;

		if (keytype == "primary") {
			primaryKey = newKey;
		}
		else if (keytype == "secondary") {			
			secondaryKeys.push_back(newKey);		
		}
		else if (keytype == "foreign") {
			foreignKeys.push_back(newKey);
		}

		// add key to all keys
		allKeys.push_back(newKey);
	}

	/// <summary>
	///  Creates all the trees for the table.
	///	 NOTE: If the primary keys, secondary keys, and rows aren't
	///  in the table already, then it won't create any trees.
	/// </summary>
	void CreateTrees() {
		// create primary tree
		if (primaryKey.keyName != "") {
			
			for (Row row : rows) {
				primaryTree.insert(row.GetIntColumnByName(primaryKey.keyName).GetValue(), row);
			}
		}
		
		// create secondary trees
		CreateSecondaryTrees();
	}
	
	/// <summary>
	/// Gets the index of the provided column
	/// </summary>
	/// <param name="column_name"></param>
	/// <returns>index of the provided column</returns>
	int get_column_index(std::string column_name) {
		int ret = -1;
		//std::map<std::string, std::string>::iterator it;
		int col_index;

		//it = columns.find(column_name);
		auto it = columns.find(column_name);

		if (it != columns.end()) {
			col_index = std::distance(columns.begin(), it);
			ret = col_index;
		}

		return ret;
	}


	void DeleteRow(Row row) {
		for (int i = 0; i < rows.size(); i++) {
			if (rows[i] == row) {
				rows.erase(rows.begin() + i);
			}
		}
	}


	void DeleteColumn(string columnName) {
		// delete column name
		columns.erase(columnName);

		for (Row row : rows) {
			row.deleteColumn(columnName);
		}
	}


	void RenameColumn(string prevColumn, string newColumn) {
		// delete column name
		auto it = columns.find(prevColumn);
		string type = it->second;
		columns.erase(prevColumn);

		// add new column name
		columns.insert(pair<string, string>(newColumn, type));


		for (Row row : rows) {
			row.renameColumn(prevColumn, newColumn);
		}
	}

};