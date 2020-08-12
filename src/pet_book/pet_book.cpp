// Standard C++ includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>

// MySQL/C++ Connector includes
#include <cppconn/resultset.h>		    // sql::ResultSet
#include <cppconn/prepared_statement.h>	// sql::PreparedStatement
#include <cppconn/exception.h>          // sql::SQLException

#include "pet_book.hpp"

using namespace sql;
using namespace std;

namespace ashs
{
    
static const string petBookDB = "pet_book";

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    : con(con), stringGen(stringGen)
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

} // namespace ashs