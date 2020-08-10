#ifndef ASHS_PET_BOOK_HPP
#define ASHS_PET_BOOK_HPP

// Standard C++ includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>

// MySQL/C++ Connector includes
#include "mysql_connection.h"		    // sql::mysql::MySQL_Connection
#include "mysql_driver.h"			    // sql::mysql::MySQL_Driver
#include <cppconn/resultset.h>		    // sql::ResultSet
#include <cppconn/prepared_statement.h>	// sql::PreparedStatement
#include <cppconn/exception.h>          // sql::SQLException

// ASHS includes
#include "../../include/stmt_string_generator.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

static const string petBookDB = "pet_book";

class PetBook
{
public:
    PetBook(Connection* con, StmtStringGenerator* stringGen);

    void Start();

private:
    Connection* con;
    StmtStringGenerator* stringGen;

    void DisplayMenu();
    void ExecuteRequests();
    void DisplayResults();
};


PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    : con(con), stringGen(stringGen)//, stmt(NULL), resault(NULL)
{
	con->setSchema(petBookDB);
}

void PetBook::Start()
{
    DisplayMenu();
    ExecuteRequests();
}

void PetBook::DisplayMenu()
{
    cout << "\nWelcome to the PetBook\n"
    << "To go back to the main menu, press q\n"
    << "Enter a query: "
    << endl;
}

void PetBook::ExecuteRequests()
{
    string query;
    PreparedStatement* pstmt;
    
    getline(cin, query);
    while (0 != query.compare("q"))
    {
        try
        {
            pstmt = con->prepareStatement(query);
            pstmt->executeQuery();
            DisplayResults();
            getline(cin, query);
        }
        catch (sql::SQLException &e)
        {
            cout << "# ERR: " << e.what() << endl;
            getline(cin, query);
        }
    }
}

void PetBook::DisplayResults()
{
    PreparedStatement* pstmt;
    ResultSet* res;
    
    pstmt = con->prepareStatement("SELECT * FROM pets ORDER BY pet_id ASC");
    res = pstmt->executeQuery();

    while (res->next())
    {
        cout
        << res->getInt("pet_id")        << "\t"
        << res->getString("adopter_id") << "\t"
        << res->getString("name")       << "\t"
        << res->getString("age_months") << "\t"
        << res->getString("status")     << endl;
    }

    // delete res;
	// delete pstmt;
}

// INSERT INTO `pets` VALUES (13,NULL,'Tira',2,'in shelter')

} // namespace ashs

#endif // ASHS_PET_BOOK_HPP