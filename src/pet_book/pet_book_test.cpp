// Standard C++ includes
#include <string>
#include <iostream>

// MySQL/C++ Connector includes
#include "mysql_driver.h"			// sql::mysql::MySQL_Driver
#include <cppconn/connection.h>     // sql::Connection

#include "pet_book.hpp"

using namespace sql;
using namespace std;
using namespace ashs;

// Helper functions
static void ConnectToSQL();
static void AtExit();

// Test functions
static void InitTest();
static void StartTest();
static void GetPKTest();

// server data
static const string URL="tcp://127.0.0.1:3306";
static const string USER="root";
static const string PASS="root";

static mysql::MySQL_Driver* g_driver;
static Connection* g_con;

int main()
{
	ConnectToSQL();
	
	InitTest();
	StartTest();
	GetPKTest();

	AtExit();

    return 0;
}

// Helper functions ////////////////////////////////////////////////////////////////////////

static void ConnectToSQL()
{
	g_driver = mysql::get_mysql_driver_instance();
	g_con = g_driver->connect(URL, USER, PASS);
}

static void AtExit()
{
	delete g_con;
}

// Test functions //////////////////////////////////////////////////////////////////////////

static void InitTest()
{
	size_t errors = 0, passes = 0;

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);

	// print report
    cout << "AddStringFunc Test: "
    << passes << " successful. "
    << errors << " errors." << endl;
}

static void StartTest()
{
	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
    petBook.Start();
}

static void GetPKTest()
{
	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
    cout << petBook.GetCurrPK() << endl;
}