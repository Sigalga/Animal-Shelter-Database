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
static const string PET_BOOK_DB("pet_book");
static const string DATA_TABLES[] = { "pets", "adopters" };
static const string DEF_TABLE(DATA_TABLES[0]);

// other consts
static const string WELCOME_MSG("\nWelcome to the PetBook");

static const PetBook::Key OPER_NAMES[] =
{
    "exit",         // 0 initial  secondary
    "show_all",     // 1 initial
    "find",         // 2 initial
    "update",       // 3 initial  secondary
    "add_new",      // 4 initial  secondary
    "delete",       // 5 initial  secondary
    "filter",       // 6          secondary
    "order",        // 7          secondary
    "clear"         // 8 initial  secondary
};

static const PetBook::Key INIT_OPER_NAMES[] =
{
    OPER_NAMES[1],
    OPER_NAMES[2],
    OPER_NAMES[3],
    OPER_NAMES[4],
    OPER_NAMES[5],
    OPER_NAMES[8],  // clear
    OPER_NAMES[0]   // exit
};

static const PetBook::Key SEC_OPER_NAMES[] =
{
    OPER_NAMES[3],
    OPER_NAMES[4],
    OPER_NAMES[5],
    OPER_NAMES[6],
    OPER_NAMES[7],
    OPER_NAMES[8],  // clear
    OPER_NAMES[0]   // exit
};
static const size_t N_TABLES = (sizeof(DATA_TABLES) / sizeof(string));
static const size_t N_OPERS = sizeof(OPER_NAMES) / sizeof(PetBook::Key);
static const size_t N_INIT_OPERS = sizeof(INIT_OPER_NAMES) / sizeof(PetBook::Key);
static const size_t N_SEC_OPERS =  sizeof(SEC_OPER_NAMES) / sizeof(PetBook::Key);

/////////////////////////////////////////////////////////////////////

PetBook::PetBook(Connection* con, StmtStringGenerator* stringGen)
    :   con(con),
        stringGen(stringGen),
        petBookDB(PET_BOOK_DB),
        isRunning(false),
        currTable(DEF_TABLE),
        currPK(""),
        currId(""),
        currQuery(""),
        stmtString(""),
        m_fields(GetFields())
{
	con->setSchema(petBookDB);
    SetCurrPK();
    InitStringGen();
    cout << currTable << endl;
}

void PetBook::InitStringGen()
{
    static const PetBook::StringFunc stringFuncs[N_OPERS] =
    {
        boost::bind(&PetBook::Exit,         this),
        boost::bind(&PetBook::GetAll,       this),
        boost::bind(&PetBook::FindBy,       this),
        boost::bind(&PetBook::UpdateField,  this),
        boost::bind(&PetBook::AddEntry,     this),
        boost::bind(&PetBook::RemoveEntry,  this),
        boost::bind(&PetBook::FilterBy,     this),
        boost::bind(&PetBook::OrderBy,      this),
        boost::bind(&PetBook::ClearSearch,  this)
    };

    // Add operations
    for (size_t i = 0; i < N_OPERS; i++)
    {
        stringGen->AddStringFunc(OPER_NAMES[i], stringFuncs[i]);
    }
}

void PetBook::Start()
{
    isRunning = true;
    
    cout << WELCOME_MSG << endl;
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
    if (0 == currId.compare("")) // new search
    {
        // choose table
        DisplayTableMenu();
        size_t i;
        cin >> i;
        while (cin.fail() || !(1 <= i && i<= N_TABLES + 1))
        {
            cin.clear();
            cin.ignore(256,'\n');
            cin >> i;
        }

        switch (i)
        {
        case N_TABLES + 1:
            return Exit();
        
        default:
            SetDataTable(DATA_TABLES[i-1]);
            break;
        }
    }

    // choose operation
    DisplayOperMenu();
    string key;
    cin >> key;

    return stringGen->GenerateString(key);
}

// Result Displayers
void PetBook::DisplayResults(ResultSet* res)
{   
    if (0 != currId.compare("query"))       // editorial
    {
        delete res;
        res = GetEditResult();
        ClearSearch();
    }

    PrintTable(res);

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

void PetBook::PrintTable(ResultSet* res)
{
    // print table headers
    for (m_fields->beforeFirst(); m_fields->next(); )
    {
        cout.width(10);
        cout << m_fields->getString("COLUMN_NAME") << "\t" << left;
    }
    cout << endl;

    // print result set
    while (res->next())
    {
        for (m_fields->beforeFirst(); m_fields->next(); )
        {
            cout.width(10);
            cout << res->getString(m_fields->getString("COLUMN_NAME")) << "\t" << left;
        }
        cout << endl;
    }
}

// Menu Displayers
void PetBook::DisplayTableMenu()
{
    cout << "Search in... (type the number)" << endl;

    size_t i = 0;
    for (i = 0; i < N_TABLES; i++)
    {
        cout << i+1 << ". " << DATA_TABLES[i] << endl;
    }
    cout << i+1 << ". exit" << endl;
}

void PetBook::DisplayOperMenu()
{
    cout << "Select an operation by typing it.\n"
    << "Available operations in " << currTable << " are:" << endl;

    if (0 == currId.compare("")) // new search
    {
        for (size_t i = 0; i < N_INIT_OPERS; i++)
        {
            cout << "- " << INIT_OPER_NAMES[i]<< endl;
        }
    }
    else                        // concat queries
    {
        for (size_t i = 0; i < N_SEC_OPERS; i++)
        {
            cout << "- " << SEC_OPER_NAMES[i]<< endl;
        }
    }
}

void PetBook::DisplayFieldMenu()
{
    cout << "Enter the parameter to search by or modify. options are:" << endl;

    m_fields->first();
    while (m_fields->next())
    {
        cout << "- " << m_fields->getString("COLUMN_NAME") << endl;
    }
}

ResultSet* PetBook::GetFields()
{
    PreparedStatement* pstmt = con->prepareStatement(
        "select COLUMN_NAME from information_schema.COLUMNS where TABLE_NAME='" + currTable + "'");
    ResultSet* fields = pstmt->executeQuery();

    delete pstmt;

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
    const string getAllAsc(SelectDataAsc());
    if (0 == currQuery.compare(getAllAsc))
    {
        currQuery = SelectData();
    }
    currQuery += " ORDER BY " + col + " " + order;

    return currQuery;
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
    
    currQuery = ("UPDATE " + currTable + " SET " + col + "='" + val + "' WHERE " + currPK + "=" + currId + ";");
    return currQuery;
}

string& PetBook::AddEntry()
{
    currId = "new_id";

    cout << "Enter the values for each parameter:" << endl;

    // add columns to statement string
    currQuery = "INSERT INTO " + currTable + " (";
    m_fields->first();
    m_fields->next();    // skip pk column
    currQuery += m_fields->getString("COLUMN_NAME");
    while (m_fields->next())
    {
        currQuery += (", " + m_fields->getString("COLUMN_NAME"));
    }
    currQuery += ") VALUES (";

    // add values from input to statement string
    m_fields->first();
    m_fields->next();    // skip pk column
    cout << m_fields->getString("COLUMN_NAME") << ": " << flush;
    string val("");
    cin >> val;
    currQuery += (isNum(val) ? val : ("'" + val + "'"));
    while (m_fields->next())
    {
        cout << m_fields->getString("COLUMN_NAME") << ": " << flush;
        cin >> val;
        currQuery += (", " + (isNum(val) ? val : ("'" + val + "'")));
    }
    currQuery += ")";

    return currQuery;
}

string& PetBook::RemoveEntry()
{
    // choose a single entry
    cout << "choose an entry and enter its pet_id" << endl;
    cin >> currId;
    
    // const string pk(currPK);
    currQuery = ("DELETE FROM " + currTable + " WHERE " + currPK + "=" + currId + ";");
    return currQuery;
}

// helper operations
string& PetBook::FindByCurrId()
{
    const string select(SelectDataWhere());
    const string rule(GetRule(currPK, currId, ""));
    stmtString = (select + rule);
    return stmtString;
}

string& PetBook::FindByMaxId()
{
    const string select(SelectDataWhere());
    stmtString = (select + currPK + " = (SELECT MAX(" + currPK + ") FROM " + currTable + ")");
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
    stmtString += (" ORDER BY " + currPK + " ASC");
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

// Setters ////////////////////////////////////////////////////////
void PetBook::SetDataTable(const string& table)
{
    currTable = table;
    m_fields = GetFields();
    SetCurrPK();;
}

void PetBook::SetCurrPK()
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
}

} // namespace ashs