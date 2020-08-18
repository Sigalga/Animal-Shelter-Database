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
// #include "pet_book_string_funcs.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

static const string g_findBy("SELECT * FROM pets WHERE ");
static const string g_pk("pet_id");
static bool isNum(const string& val);
static string& GetRule(const string& col, const string& val, const string& val2);

static const string g_welcome("\nWelcome to the PetBook");
static const size_t g_nOpers = 5;
static const PetBook::Key g_operNames[g_nOpers] =
{
    "exit",
    "find",
    "filter",
    "update",
    "show_all"
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
    // Add operations
    stringGen->AddStringFunc(g_operNames[0], boost::bind(&PetBook::Exit, this));
    stringGen->AddStringFunc(g_operNames[1], boost::bind(&PetBook::FindBy, this));
    stringGen->AddStringFunc(g_operNames[2], boost::bind(&PetBook::FilterBy, this));
    stringGen->AddStringFunc(g_operNames[3], boost::bind(&PetBook::UpdateField, this));
    stringGen->AddStringFunc(g_operNames[4], boost::bind(&PetBook::GetAll, this));
}

void PetBook::Start()
{
    isRunning = true;
    
    cout << g_welcome << endl;
    ExecuteInput();
}

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

string& PetBook::MakeString()
{
    // choose operation
    DisplayOperMenu();
    string key;
    cin >> key;

    return stringGen->GenerateString(key);
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
            pstmt = con->prepareStatement(FindByCurrId());
            res = pstmt->executeQuery();
            currId = "query";
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

// StringFuncs /////////////////////////////////////////////////////////////
string& PetBook::Exit()
{
    isRunning = false;
    return *(new string(""));
}

string& PetBook::FindBy()
{
    currId = "query";

    // enter column to search by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter value/s to search by
    cout << "Enter a value for the parameter.\n"
    << "To enter a number range, type the numbers like so:\n"
    << "-r <min> <max>"
    << endl;
    string val(""), val2("");
    cin >> val;
    if (0 == val.compare("-r"))
    {
        cin >> val;
        cin >> val2;
    }

    string* str = new string(g_findBy + GetRule(col, val, val2));
    return *str;
}

string& PetBook::FilterBy()
{
    // enter column to filter by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter value/s to filter by
    string val(""); //val2(""); // TO DO
    cout << "Enter a value for the parameter" << endl;
    cin >> val;

    string* str = new string(currQuery);

    // add rule
    *str += " AND " + GetRule(col, val, "");

    return *str;
}

string& PetBook::UpdateField()
{
    // choose a single entry
    cout << "choose an entry and enter its pet_id" << endl;
    cin >> currId;

    // enter column to update
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter new value
    cout << "Enter a new value for the parameter" << endl;
    string val("");
    cin >> val;
    
    string* str = new string("UPDATE pets SET ");
    *str += (col + "='" + val + "' WHERE " + g_pk + "=" + currId + ";");

    return *str;
}

string& PetBook::FindByCurrId()
{
    string* str = new string(g_findBy + GetRule(g_pk, currId, ""));
    return *str;
}

string& PetBook::GetAll()
{
    string* str = new string("SELECT * FROM pets ORDER BY " + g_pk + " ASC");
    return *str;
}

// Helper functions ////////////////////////////////////////////////////////
static string& GetRule(const string& col, const string& val, const string& val2)
{
    string* rule = new string;
    
    if (isNum(val))
    {
        if ("" == val2)
        {
            // find by number
            *rule = (col + "=" + val);
        }
        else
        {
            // find by range
            *rule = (col + " BETWEEN " + val + " AND " + val2);
        }
    }
    else
    {
        // find by text
        *rule = (col + " LIKE '%" + val + "%'");
    }

    return *rule;
}

static bool isNum(const string& val)
{
    char* isText;
    strtol(val.c_str(), &isText, 10);

    return false == *isText;
}

} // namespace ashs