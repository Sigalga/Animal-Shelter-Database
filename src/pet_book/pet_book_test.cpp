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
	ifstream in("in.txt");
    streambuf *cinbuf = cin.rdbuf();
    cin.rdbuf(in.rdbuf());

	ofstream out("out.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
    petBook.Start();

	// reset to standard io again
	// cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);

	// print report
    cout << passes << " successful. "
    << errors << " errors." << endl;
}

static void PrivateMethodsTest()
{
    cout << "\n-- PrivateMethodsTest():" << endl;
	size_t errors = 0, passes = 0;

	// redirect io to file stream
	ifstream in("in.txt");
    streambuf *cinbuf = cin.rdbuf();
    cin.rdbuf(in.rdbuf());
	ofstream out("out.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

	StmtStringGenerator strGen;
    PetBook petBook(g_con, &strGen);
	PbTestClass test(&petBook);
	
	test.ExecutInputTest();
	test.MakeStringTest();
	errors += test.StringFuncsTest();

	// reset to standard io again
	// cin.rdbuf(cinbuf);
    cout.rdbuf(coutbuf);
	cout << "---- StringFuncsTest(): "
	<< errors << " errors." << endl;

}



// void f()
// {
//     std::string line;
//     while(std::getline(std::cin, line))  //input from the file in.txt
//     {
//         std::cout << line << "\n";   //output to the file out.txt
//     }
// }
// int main()
// {
//     std::ifstream in("in.txt");
//     std::streambuf *cinbuf = std::cin.rdbuf(); //save old buf
//     std::cin.rdbuf(in.rdbuf()); //redirect std::cin to in.txt!

//     std::ofstream out("out.txt");
//     std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
//     std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

//     std::string word;
//     std::cin >> word;           //input from the file in.txt
//     std::cout << word << "  ";  //output to the file out.txt

//     f(); //call function


//     std::cin.rdbuf(cinbuf);   //reset to standard input again
//     std::cout.rdbuf(coutbuf); //reset to standard output again

//     std::cin >> word;   //input from the standard input
//     std::cout << word;  //output to the standard input
// }