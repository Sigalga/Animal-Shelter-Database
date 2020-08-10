
// Standard C++ includes
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>


#include "mysql_connection.h"		// sql::mysql::MySQL_Connection
#include "mysql_driver.h"			// sql::mysql::MySQL_Driver
#include <cppconn/resultset.h>		// sql::ResultSet
#include <cppconn/statement.h>		// sql::Statement
#include <cppconn/prepared_statement.h>	// sql::PreparedStatement
#include <cppconn/exception.h>	
// #include <cppconn/connection.h>	// sql::Connection
// #include <cppconn/driver.h>		// sql::Driver
// #include <cppconn/prepared_statement.h>

using namespace std;

int main(int argc, const char **argv)
{
	const string url="tcp://127.0.0.1:3306";
	const string user="root";
	const string pass="root";
	const string dataBase="adoptions";

	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;

	// connect to server and database
	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect(url, user, pass);
	con->setSchema("adoptions");

	pstmt = con->prepareStatement("SELECT pet_id, name FROM pets");
	res = pstmt->executeQuery();
	while (res->next())
	{
		cout << "\t... MySQL replies: " << endl;
		/* Access column data by alias or column name */
		cout << res->getInt64("pet_id") << endl;
		cout << res->getString("name") << endl;
		cout << "\t... MySQL says it again: ";
		/* Access column data by numeric offset, 1 is the first column */
		cout << res->getString(1) << endl;
  	}

	delete res;
	delete pstmt;
	delete con;

  	return 0;
}