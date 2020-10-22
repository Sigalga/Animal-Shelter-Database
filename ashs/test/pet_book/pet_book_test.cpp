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

// server data
static const string URL = "tcp://127.0.0.1:3306";
static const string USER = "root";
static const string PASS = "root";

int main()
{
	mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
	shared_ptr<Connection> sp_con(driver->connect(URL, USER, PASS));

	PbTestClass test(sp_con);
	test.StringFuncsTest();
    test.PrivateMethodsTest();
	test.PublicMethodsTest();

    return 0;
}