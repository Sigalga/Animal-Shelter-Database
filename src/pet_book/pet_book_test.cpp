// Standard C++ includes
#include <string>
#include <iostream>
#include <fstream>

// MySQL/C++ Connector includes
#include "mysql_driver.h"			// sql::mysql::MySQL_Driver
#include <cppconn/connection.h>     // sql::Connection

#include "pb_test_class.hpp"

using namespace sql;
using namespace std;
using namespace ashs;

// Helper functions
static void ConnectToSQL();
static void AtExit();

// server data
static const string URL = "tcp://127.0.0.1:3306";
static const string USER = "root";
static const string PASS = "root";

static mysql::MySQL_Driver* g_driver = NULL;
static Connection* g_con = NULL;

int main()
{
	ConnectToSQL();

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
	PbTestClass test(petBook);
	
    test.StartTest();
    test.PrivateMethodsTest();
    test.ExecutInputTest();
	test.MakeStringTest();
	test.StringFuncsTest();

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