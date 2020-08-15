// Standard C++ includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <vector>

// MySQL/C++ Connector includes
#include <cppconn/resultset.h>		    // sql::ResultSet
#include <cppconn/prepared_statement.h>	// sql::PreparedStatement
#include <cppconn/exception.h>          // sql::SQLException

#include "pet_book.hpp"

using namespace sql;
using namespace std;

namespace ashs
{
    

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    : con(con), stringGen(stringGen), petBookDB("pet_book")
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


// void PetBook::AddNamedStrFunc(string& name, StringFunc func)
// {
//     namedStrFuncs.push_back(pair<string, StringFunc>(name, func));
// }

// void PetBook::InitStringGen()
// {
//     // add all functions to stringGen
//     // for i in namedStrFuncs
//     //  stringGen->AddStringFunc(idx, i->second);
// }

void PetBook::DisplayStringFuncs()
{
    // // get all keys from the string generator and print them
    // // TO DO : make vec a member to improve efficiency
    // vector<Key>* vec = stringGen->GetKeys();
    // for (vector<Key>::iterator it = vec->begin();
    //     it != vec->end();
    //     ++it)
    // {
    //     cout << *it << "\n" << endl;
    // }

    // delete vec;
}

string& PetBook::MakeString()
{
    string key, col, val;

    // choose operation
    cout << "Select an operation by typing it" << endl;
    cin >> key;
    // enter column
    cout << "Enter the parameter" << endl;
    cin >> col;
    // enter value
    cout << "Enter a value for the parameter" << endl;
    cin >> val;

    return stringGen->GenerateString(key, col, val);
}

} // namespace ashs