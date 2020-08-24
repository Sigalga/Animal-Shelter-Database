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
#include <boost/bind.hpp>			    // boost::bind
#include <boost/function.hpp>		    // boost::function

#include "pet_book.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

// helper funcs
static bool isNum(const string& val);
static void GetSearchVals(string& val, string& val2);

// statement strings
static const string g_petBookDB("pet_book");
static const string g_petsTable("pets");
static const string g_adopTable("adopters");
static const string g_defaultTable(g_petsTable);

static const string g_pk("pet_id");
// SELECT COL
static const string g_getAll("SELECT * FROM pets");
static const string g_findBy(g_getAll + " WHERE ");
static const string g_getAllAsc(g_getAll + " ORDER BY " + g_pk + " ASC");

// other consts
static const string g_welcome("\nWelcome to the PetBook");

static const size_t g_nInitOpers = 2;
static const size_t g_nSecOpers = 3;
static const size_t g_nInitSecOpers = 4;

// names of all the ui operations
static const size_t g_nOpers =  g_nInitOpers +
                                g_nSecOpers +
                                g_nInitSecOpers;
static const PetBook::Key g_operNames[g_nOpers] =
{
    "exit",         // 0 initial  secondary
    "show_all",     // 1 initial
    "find",         // 2 initial
    "update",       // 3 initial  secondary
    "add_new",      // 4 initial  secondary
    "delete",       // 5 initial  secondary
    "filter",       // 6          secondary
    "order",        // 7          secondary
    "clear"         // 8          secondary
};

// names of ui operations to appear in a new search
static const size_t g_nInitMenuOpers =  g_nInitOpers +
                                        g_nInitSecOpers;
static const PetBook::Key g_initOperNames[g_nInitMenuOpers] =
{
    g_operNames[0],
    g_operNames[1],
    g_operNames[2],
    g_operNames[3],
    g_operNames[4],
    g_operNames[5]
};

// names of ui operations to appear after an initial/secondary query
static const size_t g_nSecMenuOpers =   g_nSecOpers +
                                        g_nInitSecOpers;
static const PetBook::Key g_secOperNames[g_nSecMenuOpers] =
{
    g_operNames[0],
    g_operNames[3],
    g_operNames[4],
    g_operNames[5],
    g_operNames[6],
    g_operNames[7],
    g_operNames[8]
};

/////////////////////////////////////////////////////////////////////

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    :   con(con),
        stringGen(stringGen),
        petBookDB(g_petBookDB),
        isRunning(false),
        currTable(g_defaultTable),
        currPK(""),
        currId(""),
        currQuery(""),
        stmtString(""),
        fields(NULL)
{
	con->setSchema(petBookDB);
    InitStringGen();
    GetFields();
}

void PetBook::InitStringGen()
{
    static const PetBook::StringFunc stringFuncs[g_nOpers] =
    {
        boost::bind(&PetBook::Exit, this),
        boost::bind(&PetBook::GetAll, this),
        boost::bind(&PetBook::FindBy, this),
        boost::bind(&PetBook::UpdateField, this),
        boost::bind(&PetBook::AddEntry, this),
        boost::bind(&PetBook::RemoveEntry, this),
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
    currQuery = MakeString();
    while (isRunning)
    {
        if (0 != currQuery.compare(""))
        {
            try
            {
                PreparedStatement* pstmt = con->prepareStatement(currQuery);
                DisplayResults(pstmt->executeQuery());

                delete pstmt;
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

// Result Displayers    // TO DO: make "fields" a member, setupon init and delete in dtor.
void PetBook::DisplayResults(ResultSet* res)
{   
    if (0 != currId.compare("query"))       // editorial
    {
        delete res;
        res = GetEditResult();
        ClearSearch();
    }

    // PrintTable(res, fields);
    // PrintTable(res);

    delete res;
}

ResultSet* PetBook::GetEditResult()
{
    ResultSet* res;

    try
    {
        PreparedStatement* pstmt;
        if (0 == currId.compare("new_id"))  // new entry added
        {
            pstmt = con->prepareStatement(FindByMaxId());
        }
        else                                // entry edited
        {
            pstmt = con->prepareStatement(FindByCurrId());
        }

        res = pstmt->executeQuery();

        delete pstmt;
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }
    
    return res;
}

// void PetBook::PrintTable(ResultSet* res, ResultSet* fields) // TO DO: remove fields param
void PetBook::PrintTable(ResultSet* res)
{
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

// Menu Displayers
void PetBook::DisplayOperMenu()
{
    cout << "Select an operation by typing it.\n"
    << "Available operations are:" << endl;

    if (0 == currId.compare("")) // new search
    {
        for (size_t i = 0; i < g_nInitMenuOpers; i++)
        {
            cout << "- " << g_initOperNames[i]<< endl;
        }
    }
    else    // continuous search
    {
        for (size_t i = 0; i < g_nSecMenuOpers; i++)
        {
            cout << "- " << g_secOperNames[i]<< endl;
        }
    }
}

void PetBook::DisplayFieldMenu() // TO DO: make "fields" a member, setupon init and delete in dtor.
{
    cout << "Enter the parameter to search by or modify. options are:" << endl;

    for (fields->beforeFirst(); fields->next(); )
    {
        cout << "- " << fields->getString("COLUMN_NAME") << endl;
    }
}

ResultSet* PetBook::GetFields()
{
    try
    {
        PreparedStatement* pstmt = con->prepareStatement(
            "select COLUMN_NAME from information_schema.COLUMNS where TABLE_NAME='" + currTable + "'");
        // ResultSet* fields = pstmt->executeQuery();
        fields = pstmt->executeQuery();

        delete pstmt;
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }

    return fields;
}

// StringFuncs /////////////////////////////////////////////////////////////
string& PetBook::Exit()
{
    isRunning = false;
    return stmtString;
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
    string val(""), val2("");
    GetSearchVals(val, val2);

    const string select(SelectDataWhere());
    const string rule(GetRule(col, val, val2));

    currQuery = (select + rule);
    return currQuery;
}

string& PetBook::GetAll()
{
    currId = "query";

    currQuery = SelectDataAsc();
    return currQuery;
}

// secondary queries
string& PetBook::FilterBy()
{
    // enter column to filter by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter value/s to filter by
    string val(""), val2("");
    GetSearchVals(val, val2);

    // add rule
    const string rule(GetRule(col, val, ""));
    const string getAll(SelectDataAsc());

    if (0 == currQuery.compare(getAll))
    {
        const string select(SelectDataWhere());
        currQuery = (select + rule);
    }
    else
    {
        currQuery += (" AND " + rule);
    }

    return currQuery;
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

    // add rule
    const string getAll(SelectDataAsc());
    if (0 == currQuery.compare(getAll))
    {
        currQuery = g_getAll;
    }
    currQuery += " ORDER BY " + col + " " + order;

    return currQuery;
}

// editorial operations // TO DO: fix memory leakage
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
    
    const string pk(GetCurrPK());
    currQuery = ("UPDATE pets SET " + col + "='" + val + "' WHERE " + pk + "=" + currId + ";");
    return currQuery;
}

string& PetBook::AddEntry()
{
    currId = "new_id";

    cout << "Enter the values for each parameter:" << endl;

    // add columns to statement string
    currQuery = "INSERT INTO pets (";

    // ResultSet* res = GetFields();
    // res->next();
    // res->next();    // skip pk column
    // currQuery += res->getString("COLUMN_NAME");
    // while (res->next())
    // {
    //     currQuery += (", " + res->getString("COLUMN_NAME"));
    // }

    fields->first();
    fields->next();    // skip pk column
    currQuery += fields->getString("COLUMN_NAME");
    while (fields->next())
    {
        currQuery += (", " + fields->getString("COLUMN_NAME"));
    }
    ResultSet* res = NULL;


    // add values from input to statement string
    currQuery += ") VALUES (";
    
    res->first();
    res->next();    // skip pk column
    cout << res->getString("COLUMN_NAME") << ": " << flush;
    string val("");
    cin >> val;
    currQuery += (isNum(val) ? val : ("'" + val + "'"));
    while (res->next())
    {
        cout << res->getString("COLUMN_NAME") << ": " << flush;
        cin >> val;
        currQuery += (", " + (isNum(val) ? val : ("'" + val + "'")));
    }
    currQuery += ")";

    delete res;

    return currQuery;
}

string& PetBook::RemoveEntry()
{
    // choose a single entry
    cout << "choose an entry and enter its pet_id" << endl;
    cin >> currId;
    
    const string pk(GetCurrPK());
    currQuery = ("DELETE FROM " + currTable + " WHERE " + pk + "=" + currId + ";");
    return currQuery;
}

// helper operations
string& PetBook::FindByCurrId()
{
    const string pk(GetCurrPK());
    const string select(SelectDataWhere());
    const string rule(GetRule(pk, currId, ""));
    stmtString = (select + rule);
    return stmtString;
}

string& PetBook::FindByMaxId()
{
    const string select(SelectDataWhere());
    const string pk(GetCurrPK());
    stmtString = (select + pk + " = (SELECT MAX(" + pk + ") FROM pets)");
    return stmtString; 
}

string& PetBook::ClearSearch()
{
    currId = "";
    currQuery = "";
    return currQuery;
}

// Helper functions ////////////////////////////////////////////////////////
string& PetBook::GetRule(const string& col, const string& val, const string& val2)
{
    if (isNum(val))
    {
        if ("" == val2)
        {
            // find by number
            stmtString = (col + "=" + val);
        }
        else
        {
            // find by range
            stmtString = (col + " BETWEEN " + val + " AND " + val2);
        }
    }
    else
    {
        // fid by text
        stmtString = (col + " LIKE '%" + val + "%'");
    }

    return stmtString;
}

static bool isNum(const string& val)
{
    char* isText;
    strtol(val.c_str(), &isText, 10);

    return false == *isText;
}

static void GetSearchVals(string& val, string& val2)
{
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

string& PetBook::GetCurrPK()
{
    try
    {
        PreparedStatement* pstmt = con->prepareStatement(
            "SHOW KEYS FROM " + currTable + " WHERE Key_name = 'PRIMARY';");
        ResultSet* res = pstmt->executeQuery();
        res->next();
        currPK = (res->getString("COLUMN_NAME"));

        delete res;
        delete pstmt;
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }

    return currPK;
}

string& PetBook::SelectData()
{
    stmtString = ("SELECT * FROM " + currTable);
    return stmtString;
}

string& PetBook::SelectDataWhere()
{
    SelectData();
    stmtString += " WHERE ";
    return stmtString;
}

string& PetBook::SelectDataAsc()
{
    SelectData();
    stmtString += (" ORDER BY " + GetCurrPK() + " ASC");
    return stmtString;
}

} // namespace ashs