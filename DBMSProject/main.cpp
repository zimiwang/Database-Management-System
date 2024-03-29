// main function
/* run this program using the console pauser or add your own getch, system("pause") or input loop */


#define _CRT_SECURE_NO_WARNINGS		// error occurred because of security reasons. Needed to add this.
#include "database.h"
#include "login.h"
#include <vector>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <regex>
#include <iostream>
#include <cerrno>


#include "commandHandler.h"
#include "serialization.h"

//#include "mainReferenceHeader.h"

// global variables
HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

// ***Unreachable
Table* create_table(std::string table_name, std::vector<pair<std::string, std::string>> columns_info);
std::string to_lower(std::string str);
Database* read_sql_file(string path);
std::string login_interface();
int mainFunct(string cmd);

std::string current_db_name;
std::string table_name;
std::string db_name;
std::string cmd = "";
std::string statement;

Database* db = NULL;

// function initializers
void color(int s);
void setup_intro();

/*
void show_help();
void print_rows(Table tbl);
void table_info(Table tbl);
void insert_into(Database* db, vector<string> split_commands);
void drop_table(Database* db, Table* tbl);
void drop_database(string db_name);
bool has_special_char(std::string const& str);
void update_table(Database* db, std::string table_name, std::string col1, std::string toUpdate, std::string col2, std::string forVariable);
*/

CommandHandler* cmdHandler = new CommandHandler;

template <class T>
Serialization<T> * serializeInstance = new Serialization<T>;


/*	
	these functions actually call and return the values for their sister-functions in commandHander.h 
	the variables for cmd, db, etc. are updated after these functions run, just in case they are important
*/
int exitDBMS() { return cmdHandler->exitDBMS(); }
int helpMenu() { return cmdHandler->helpMenu(); } /* test incorperated in test.cpp */
int loginHandler() { return cmdHandler->loginDBMS(); }
int noSemiColon() { return cmdHandler->noSemiColon(); }
int openDatabase() { int retVal = cmdHandler->openDatabase(current_db_name, db, cmd); current_db_name = cmdHandler->current_db_name; db = cmdHandler->db; return retVal;}/* test incorperated in tests.cpp */
int createDatabase() { int retVal = cmdHandler->createDatabase(current_db_name, db, cmd); current_db_name = cmdHandler->current_db_name; db = cmdHandler->db; return retVal; }/* test incorperated in tests.cpp */
int listDatabases() { int retVal = cmdHandler->listDatabases(); return retVal; }
int loadSQLfile() { int retVal = cmdHandler->loadSQLfile(db, current_db_name); current_db_name = cmdHandler->current_db_name; db = cmdHandler->db; return retVal; }
int dropDatabase() { int retVal = cmdHandler->dropDatabase(cmd); current_db_name = cmdHandler->current_db_name; return retVal; } /* test incorperated in tests.cpp*/
int noDBopen() { int retVal = cmdHandler->noDBopen(); return retVal; }
int listTables() { int retVal = cmdHandler->listTables(db);  db = cmdHandler->db; return retVal; }
int dbInfo() { int retVal = cmdHandler->dbInfo(db); return retVal; }
int select() { int retVal = cmdHandler->select(db, cmd); current_db_name = cmdHandler->current_db_name; db = cmdHandler->db; return retVal;} //TODO ***************** untested ************************
int createTable() { int retVal = cmdHandler->createTable(db, cmd, table_name); table_name = cmdHandler->table_name; db = cmdHandler->db; return retVal; }
int insertInto() { int retVal = cmdHandler->insertInto(table_name, db, statement, cmd); table_name = cmdHandler->table_name; db = cmdHandler->db; return retVal; }  //tested, should work
int tableInfo() { int retVal = cmdHandler->tableInfo(db, cmd, table_name); return retVal; }
int dropTable() { int retVal = cmdHandler->dropTable(db, cmd); db = cmdHandler->db; return retVal; }
int update() { int retVal = cmdHandler->update(db, cmd); db = cmdHandler->db; return retVal; } //TODO ***************** untested ************************
int deleteFrom() { int retVal = cmdHandler->deleteFrom(db, statement, cmd); db = cmdHandler->db; return retVal; } //TODO ***************** untested ************************
int renameTable() { int retVal = cmdHandler->renameTable(db, cmd); db = cmdHandler->db; return retVal; }
int renameColumn() { int retVal = cmdHandler->renameColumn(db, cmd); db = cmdHandler->db; return retVal; }
int alterHandler() { int retVal = cmdHandler->alterHandler(cmd); db = cmdHandler->db; return retVal; }
int dropColumn() { int retVal = cmdHandler->dropColumn(cmd); db = cmdHandler->db; return retVal; }
template <class T> int serializeHandler() { int retVal = serializeInstance<class T>->serialize(); return retVal; }



/*
* // short script for testing the unit tests' commands
int mainT(int argc, char** argv);
int main() {
       char* cmdCreateTable[] = { (char*)"create ",  (char*)"tableExample(char a, char b, char c);" };

       mainT(3, cmdCreateTable);

               return 1;
}
*/


/// <summary>
/// main function
/// </summary>
/// <param name="argc">number of inputs to function call</param>
/// <param name="argv">inputs to functions stored as string array (char**)</param>
/// <returns></returns>
int main(int argc, char** argv)
{
	// The user login interface has been commented out; if you want to use it, delete the comments
	//loginHandler();

	setup_intro();

	while (Parser::to_lower(cmd) != "exit")
	{
		cmd = "";


		// Setup the command to wait for input
		color(10);
		if (current_db_name.length() > 0)
		{
			std::cout << current_db_name << "@";
		}

		std::cout << "SQL>";
		color(7);


		// if there are no inputs, then use the stdin for user control
		if ( argc == 1 )
		{			
			std::getline(std::cin, cmd);
		}
		// if there are inputs, use the argc[] string array for inputs
		else
		{
			// simple parser for when command arguments are placed in the argc[] list
			cmd = string(argv[0]);
			for (int i = 1; i < argc-1; i++) 
			{
				cout << "argv[" << i << "] : " << argv[i] << "\t|\n";
				cmd += string(argv[i]); 
				
				cout << "cmd: \"" << cmd << "\"\n";
			}
			argc = 2;
			argv[0] = (char*) "exit";
		}

		statement = Parser::to_lower(cmd);



		/// <summary>
		/// list of sql commands and references to the functions they call
		/// </summary>
		/// <param name="argc">command name</param>
		/// <param name="argv">function reference</param>
		/// <returns></returns>
		//std::template <typename T>
		map<string, int (*)() > sqlCommands;
		sqlCommands = {
			{ "exit", &exitDBMS, },
			{ "help", &helpMenu, },
			{ "noSemiColon", &noSemiColon, },
			{ "openDatabase", &openDatabase, },
			{ "createDatabase", &createDatabase, },
			{ "listDatabases", &listDatabases, },
			{ "loadSQLfile", &loadSQLfile, },
			{ "dropDatabase", &dropDatabase, },
			{ "noDBopen", &noDBopen, },
			{ "listTables", &listTables,},
			{ "dbInfo",&dbInfo, },
			{ "select", &select, },
			{ "createTable", &createTable, },
			{ "insertInto", &insertInto, },
			{ "tableInfo", &tableInfo, },
			{ "dropTable", &dropTable, },
			{ "update", &update, },
			{ "deleteFrom", &deleteFrom, },
			{ "renameTable", &renameTable, },
			{ "renameColumn", &renameColumn, },
			{ "alterHandl", &alterHandler, },
			{ "loginHandler", &loginHandler, },
			//{ "serializeHandler", &serializeHandler, },
		};


		// Display all of the mapped functions
		std::map<string, int(*)()>::const_iterator it = sqlCommands.begin();
		std::map<string, int(*)()>::const_iterator end = sqlCommands.end();

		// condition checking. If true, execute the function pointer (located in sqlCommands)
		if (statement == "")								cout << "";
		else if (statement == "exit")						(*sqlCommands.find("exit")).second();
		else if (statement == "help")						(*sqlCommands.find("help")).second();
		else if (statement.back() != ';')					(*sqlCommands.find("noSemiColon")).second();
		else if (statement.find("open database ") == 0)		(*sqlCommands.find("openDatabase")).second();
		else if (statement.find("create database") == 0)	(*sqlCommands.find("createDatabase")).second();
		else if (statement == "list databases;")			(*sqlCommands.find("listDatabases")).second();
		else if (statement.find("load sqlfile ") == 0)		(*sqlCommands.find("loadSQLfile")).second();
		else if (statement.find("drop database ") == 0)		(*sqlCommands.find("dropDatabase")).second();
		else if (current_db_name.length() == 0)				(*sqlCommands.find("noDBopen")).second();
		else if (statement == "list tables;")				(*sqlCommands.find("listTables")).second();
		else if (statement == "db info;")					(*sqlCommands.find("dbInfo")).second();
		else if (statement.find("select ") == 0)			(*sqlCommands.find("select")).second();
		else if (statement.find("create table ") == 0)		(*sqlCommands.find("createTable")).second();
		else if (statement.find("insert into") == 0)		(*sqlCommands.find("insertInto")).second();
		else if (statement.find("table info ") == 0)		(*sqlCommands.find("tableInfo")).second();
		else if (statement.find("drop table ") == 0)		(*sqlCommands.find("dropTable")).second();
		else if (statement.find("update ") == 0)			(*sqlCommands.find("update")).second();
		else if (statement.find("delete from ") == 0)		(*sqlCommands.find("deleteFrom")).second();
		else if (statement.find("rename table ") == 0)		(*sqlCommands.find("renameTable")).second();
		else if (statement.find("rename column ") == 0)		(*sqlCommands.find("renameColumn")).second();
		else if (statement.find("alter ") == 0)             (*sqlCommands.find("alterHandl")).second();
		else if (statement.find("logout") == 0)             (*sqlCommands.find("loginHandler")).second();
		else if (statement.find("serialize") == 0)			{ serializeHandler<class T>(); }

		else												std::cout << "Invalid Command." << std::endl;

	}

	return 1;
}
// end of main function




///Author: Andrew Nunez
///Sets the console output color
void color(int s)
{
	SetConsoleTextAttribute(h, s);
}




/*
///Author: Janita Aamir
///This function drops the given table from the current database.
void drop_table(Database* db, Table* tbl) {
	tbl->Delete();

	for (std::vector<Table>::iterator it = db->tables.begin(); it != db->tables.end(); ++it)
	{
		if (it->table_name == tbl->table_name)
		{
			db->tables.erase(it);
			break;
		}
	}
	db->Save();
}
*/




///Janita Aamir
///This function is used within create table. It checks to see if the
///database selected has any special characters that aren't allowed.
bool has_special_char(std::string const& s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (!std::isalpha(s[i]) && !std::isdigit(s[i]) && s[i] != '_')
			return false;
	}
}

/// Author: Andrew Nunez

/// <summary>
/// Setups the intro, emulating a startup sequence... we can probably have it set to actually do something interesting
/// </summary>
void setup_intro()
{
	std::cout << "ISU RDBMS Project" << std::endl;
	std::cout << "Opening RDBMS Shell.";
	Sleep(400);
	std::cout << ".";
	Sleep(400);
	std::cout << ".";
	Sleep(400);
	std::cout << ".";
	Sleep(400);
	std::cout << ".";
	Sleep(400);
	std::cout << ".";

	std::cout << std::endl
		<< std::endl
		<< "Success! Here is your shell." << std::endl
		<< "Type [help] for a list of commands. Type [exit] to quit." << std::endl
		<< std::endl;
}