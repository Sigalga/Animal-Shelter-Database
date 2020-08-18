// Standard C++ includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <vector>  

// MySQL/C++ Connector includes
#include <cppconn/prepared_statement.h>	// sql::PreparedStatement
#include <cppconn/exception.h>          // sql::SQLException

// Boost includes
#include <boost/bind.hpp>			// boost::bind
#include <boost/function.hpp>		// boost::function

#include "pet_book.hpp"
#include "pet_book_string_funcs.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

static const string g_welcome("\nWelcome to the PetBook");
static const size_t g_nOpers = 4;
static const PetBook::Key g_operNames[g_nOpers] =
{
    "exit",
    "find",
    "update",
    "filter"
};

static const PetBook::StringFunc g_StringFuncs[g_nOpers] =
{
    NULL,
    &FindBy,
    &UpdateField,
    &FilterBy
};

////////////////////////////////////////////////////////////////////

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    :   con(con),
        stringGen(stringGen),
        petBookDB("pet_book"),
        isRunning(false),
        currId(""),
        currQuery("")
{
	con->setSchema(petBookDB);
    InitStringGen();
}

void PetBook::InitStringGen()
{
    // Add the exit operation
    stringGen->AddStringFunc(g_operNames[0], boost::bind(&PetBook::Exit, this, _1, _2, _3));

    // Add all other operations
    for (size_t i = 1; i < g_nOpers; i++)
    {
        stringGen->AddStringFunc(g_operNames[i], g_StringFuncs[i]);
    }
}

void PetBook::Start()
{
    isRunning = true;
    
    cout << g_welcome << endl;
    ExecuteInput();
}

// Input loop
void PetBook::ExecuteInput()
{
    PreparedStatement* pstmt;
    
    currQuery = MakeString();
    while (isRunning)
    {
        try
        {
            pstmt = con->prepareStatement(currQuery);
            DisplayResults(pstmt->executeQuery());    
        }
        catch (sql::SQLException &e)
        {
            cout << "# ERR: " << e.what() << endl;
        }

        currQuery = MakeString();
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
        currId = "query";

        // enter column
        DisplayFieldMenu();
        cin >> col;

        // enter value/s
        cout << "Enter a value for the parameter.\n"
        << "To enter a number range, type the numbers like so:\n"
        << "-r <min> <max>"
        << endl;
        cin >> val;
        if (0 == val.compare("-r"))
        {
            cin >> val;
            cin >> val2;
        }
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
    else if (0 == key.compare("filter"))
    {
        val2 = currQuery;

        // enter column
        DisplayFieldMenu();
        cin >> col;

        // enter value/s
        cout << "Enter a value for the parameter" << endl;
        cin >> val;
    }

    return stringGen->GenerateString(key, col, val, val2);
}

// Result Displayers ///////////////////////////////////////////////////////
void PetBook::DisplayResults(ResultSet* res)
{   
    ResultSet* fields = GetFields();

    if (0 != currId.compare("query"))
    {
        try
        {
            PreparedStatement* pstmt;
            pstmt = con->prepareStatement(FindBy("pet_id", currId));
            res = pstmt->executeQuery();    
        }
        catch (sql::SQLException &e)
        {
            cout << "# ERR: " << e.what() << endl;
        }
    }

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

    for (size_t i = 0; i < g_nOpers; i++)
    {
        cout << "- " << g_operNames[i]<< endl;
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

PetBook::~PetBook()
{
}

} // namespace ashs