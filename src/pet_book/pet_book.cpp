// Standard C++ includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <vector>  

// MySQL/C++ Connector includes
#include <cppconn/prepared_statement.h>	// sql::PreparedStatement
#include <cppconn/exception.h>          // sql::SQLException

#include "pet_book.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

const string g_welcome("\nWelcome to the PetBook");

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    :   con(con),
        stringGen(stringGen),
        petBookDB("pet_book"),
        operationNames(stringGen->GetKeys()),
        isRunning(false),
        currId("0")
{
	con->setSchema(petBookDB);
}

PetBook::~PetBook()
{
    delete operationNames;
}

void PetBook::Start()
{
    operationNames = stringGen->GetKeys();
    isRunning = true;
    
    cout << g_welcome << endl;
    ExecuteInput();
}

void PetBook::ExecuteInput()
{
    string query;
    PreparedStatement* pstmt;
    
    query = MakeString();
    while (isRunning)
    {
        try
        {
            pstmt = con->prepareStatement(query);
            DisplayResults(pstmt->executeQuery());    
        }
        catch (sql::SQLException &e)
        {
            cout << "# ERR: " << e.what() << endl;
        }

        query = MakeString();
    }
}

// String Makers ///////////////////////////////////////////////////////////
string& PetBook::MakeString()
{
    string key, col="", val="", val2="", id="";

    // choose operation
    DisplayOperMenu();
    cin >> key;

    if (0 == key.compare("find"))
    {
        currId = "0";

        // enter column
        DisplayFieldMenu();
        cin >> col;

        // enter value
        cout << "Enter a value for the parameter" << endl;
        cin >> val;
    }
    else if (0 == key.compare("update"))
    {
        cout << "choose an entry and enter its pet_id" << endl;
        cin >> val2;
        currId = val2;

        // enter column
        DisplayFieldMenu();
        cin >> col;

        // enter value
        cout << "Enter a value for the parameter" << endl;
        cin >> val;
    }

    return stringGen->GenerateString(key, col, val, val2);
}

// Result Displayers ///////////////////////////////////////////////////////
void PetBook::DisplayResults(ResultSet* res)
{   
    ResultSet* fields = GetFields();

    while (res->next())
    {
        for (fields->beforeFirst(); fields->next(); )
        {
            cout << res->getString(fields->getString("COLUMN_NAME")) << "\t";
        }
        cout << endl;
    }
}

void PetBook::DisplayAll()
{
    PreparedStatement* pstmt;
    ResultSet* res;
    
    pstmt = con->prepareStatement("SELECT * FROM pets ORDER BY pet_id ASC");
    res = pstmt->executeQuery();

    DisplayResults(res);
}

// Menu Displayers /////////////////////////////////////////////////////////
void PetBook::DisplayOperMenu()
{
    cout << "Select an operation by typing it.\n"
    << "Available operations are:" << endl;

    DisplayStringFuncs();
}

void PetBook::DisplayStringFuncs()
{
    // get all keys from the string generator and print them
    for (vector<Key>::iterator it = operationNames->begin();
        it != operationNames->end();
        ++it)
    {
        cout << "- " << *it << endl;
    }
}

void PetBook::DisplayFieldMenu()
{
    cout << "Enter the parameter to search by or modify. options are:" << endl;

    ResultSet* res = GetFields();
    while (res->next())
    {
        cout << "- " << res->getString("COLUMN_NAME") << endl;
    }
}

ResultSet* PetBook::GetFields()
{
    PreparedStatement* pstmt = con->prepareStatement(
        "select COLUMN_NAME from information_schema.COLUMNS where TABLE_NAME='pets'");

    return pstmt->executeQuery();
}

} // namespace ashs