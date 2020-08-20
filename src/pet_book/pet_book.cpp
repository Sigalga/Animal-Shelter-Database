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

using namespace sql;
using namespace std;

namespace ashs
{

// helper funcs
static bool isNum(const string& val);
static string& GetRule(const string& col, const string& val, const string& val2);

// statement strings
static const string g_petBookDB("pet_book");
static const string g_pk("pet_id");
static const string g_getAll("SELECT * FROM pets");
static const string g_findBy(g_getAll + " WHERE ");
static const string g_getAllAsc(g_getAll + " ORDER BY " + g_pk + " ASC");

// other consts
static const string g_welcome("\nWelcome to the PetBook");

static const size_t g_nOpers = 7;
static const PetBook::Key g_operNames[g_nOpers] =
{
    "exit",         // initial  secondary
    "show_all",     // initial
    "find",         // initial
    "update",       // initial  secondary
    "filter",       //          secondary
    "order",        //          secondary
    "new_search"    //          secondary
};

static const size_t g_nInitOpers = 4;
static const PetBook::Key g_initOperNames[g_nInitOpers] =
{
    "exit",         // initial  secondary
    "show_all",     // initial
    "find",         // initial
    "update",       // initial  secondary
};

static const size_t g_nSecOpers = 5;
static const PetBook::Key g_secOperNames[g_nSecOpers] =
{
    "exit",         // initial  secondary
    "update",       // initial  secondary
    "filter",       //          secondary
    "order",        //          secondary
    "new_search"    //          secondary
};

////////////////////////////////////s////////////////////////////////

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    :   con(con),
        stringGen(stringGen),
        petBookDB(g_petBookDB),
        isRunning(false),
        currId(""),
        currQuery("")   
{
	con->setSchema(petBookDB);
    InitStringGen();
}

void PetBook::InitStringGen()
{
    static const PetBook::StringFunc stringFuncs[g_nOpers] =
    {
        boost::bind(&PetBook::Exit, this),
        boost::bind(&PetBook::GetAll, this),
        boost::bind(&PetBook::FindBy, this),
        boost::bind(&PetBook::UpdateField, this),
        boost::bind(&PetBook::FilterBy, this),
        boost::bind(&PetBook::OrderBy, this),
        boost::bind(&PetBook::ClearSearch, this)
    };

    // Add operations
    for (size_t i = 0; i < g_nOpers; i++)
    {
        stringGen->AddStringFunc(g_operNames[i], stringFuncs[i]);
    }
    
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
        if (0 != currQuery.compare(""))
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

    if (0 != currId.compare("query"))   // editorial
    {
        try
        {
            PreparedStatement* pstmt;
            pstmt = con->prepareStatement(FindByCurrId());
            res = pstmt->executeQuery();

            ClearSearch();
        }
        catch (sql::SQLException &e)
        {
            cout << "# ERR: " << e.what() << endl;
        }
    }

    // print table headers
    for (fields->beforeFirst(); fields->next(); )
    {
        cout.width(10);
        cout << fields->getString("COLUMN_NAME") << "\t" << left;
    }
    cout << endl;

    // print result set
    while (res->next())
    {
        for (fields->beforeFirst(); fields->next(); )
        {
            cout.width(10);
            cout << res->getString(fields->getString("COLUMN_NAME")) << "\t" << left;
        }
        cout << endl;
    }
}

// Menu Displayers /////////////////////////////////////////////////////////
void PetBook::DisplayOperMenu()
{
    cout << "Select an operation by typing it.\n"
    << "Available operations are:" << endl;

    if (0 == currId.compare("")) // new search
    {
        for (size_t i = 0; i < g_nInitOpers; i++)
        {
            cout << "- " << g_initOperNames[i]<< endl;
        }
    }
    else    // continuous search
    {
        for (size_t i = 0; i < g_nSecOpers; i++)
        {
            cout << "- " << g_secOperNames[i]<< endl;
        }
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

// initial queries
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

string& PetBook::GetAll()
{
    currId = "query";
    
    string* str = new string(g_getAllAsc);
    return *str;
}

// secondary queries
string& PetBook::FilterBy()
{
    // enter column to filter by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter value/s to filter by
    cout << "Enter a value for the parameter" << endl;
    string val(""); //val2(""); // TO DO
    cin >> val;

    string* str = new string(currQuery);

    // add rule
    if (0 == str->compare(g_getAllAsc))
    {
        *str = (g_findBy + GetRule(col, val, ""));
    }
    else
    {
        *str += " AND " + GetRule(col, val, "");
    }

    return *str;
}

string& PetBook::OrderBy()
{
    // enter column to order by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter ASC or DESC to order by
    cout << "Enter the order (ASC / DESC)" << endl;
    string order("");
    cin >> order;

    string* str = new string(currQuery);

    // add rule
    if (0 == str->compare(g_getAllAsc))
    {
        *str = g_getAll;
    }
    *str += " ORDER BY " + col + " " + order;

    return *str;
}

// editorial operations
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

string& PetBook::ClearSearch()
{
    currId = "";
    currQuery = "";
    return *(new string(""));
}




// string& AddEntry()
// {
//     // the id of the entry must be the next free id number
//     // get an SQL statement which provides it(?)
//      // perhaps manage a list of free id's smaller than the highest occupied?
//     // example:
//     // "INSERT INTO pets VALUES (val1, val2, val3)"
// }

// string& RemoveEntry()
// {
//     // example:
//     // "DELETE FROM pets WHERE pk=id"
// }

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