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

// Test functions
static void InitTest();
static void StartTest();
static void PrivateMethodsTest();

// server data
static const string URL = "tcp://127.0.0.1:3306";
static const string USER = "root";
static const string PASS = "root";

static mysql::MySQL_Driver* g_driver = NULL;
static Connection* g_con = NULL;

ifstream in("in.txt");
ofstream out("out.txt");
streambuf *cinbuf = cin.rdbuf();
streambuf *coutbuf = cout.rdbuf();

int main()
{
	ConnectToSQL();
	
	InitTest();
	StartTest();
	PrivateMethodsTest();

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
    cout << "\n-- InitTest(): ";
	size_t errors = 0, passes = 0;

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);

	// print report
    cout << passes << " successful. "
    << errors << " errors." << endl;
}

static void StartTest()
{
    cout << "\n-- StartTest(): ";
	size_t errors = 0, passes = 0;

	// redirect io to file stream
    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
    petBook.Start();

	// reset to standard io again
	cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);

	// print report
    cout << passes << " successful. "
    << errors << " errors." << endl;
}

static void PrivateMethodsTest()
{
    cout << "\n-- PrivateMethodsTest():" << endl;
	size_t errors = 0, passes = 0;

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
	PbTestClass test(&petBook);

	cout << "---- ExecutInputTest(): " << endl;
	// redirect io to file stream
    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());
	test.ExecutInputTest();
	// reset to standard io again
	cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);

	cout << "---- MakeStringTest(): " << endl;
	// redirect io to file stream
    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());
	test.MakeStringTest();
	// reset to standard io again
	cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);

	cout << "---- StringFuncsTest(): " << endl;
	errors += test.StringFuncsTest();

	cout << "---- " << errors << " errors." << endl;

}