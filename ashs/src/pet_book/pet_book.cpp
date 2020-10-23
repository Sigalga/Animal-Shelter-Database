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

// helper funcs
static bool isNum(const string& val);
static bool isNull(const string& val);
static void GetSearchVals(string& val, string& val2);

// constants
static const vector<string> DATA_TABLES = { "pets", "adopters" };
static const size_t NO_TABLE_IDX = DATA_TABLES.size() + 1;
static const string FK("adopter_id");   // foreign key field of the main table
static const string WELCOME_MSG("\nWelcome to the PetBook");

// a list of all operation keys to be displayed in ui menus,
// and to serve as keys for calling the respective functions
static const vector<PetBook::Key> OPER_NAMES =
{
    "exit",         // 0  initial  secondary
    "show_all",     // 1  initial
    "find",         // 2  initial
    "update",       // 3  initial  secondary    choice
    "add_new",      // 4  initial  secondary
    "delete",       // 5  initial  secondary    choice
    "filter",       // 6           secondary
    "order",        // 7           secondary
    "clear",        // 8  initial  secondary
    "choose",       // 9  initial  secondary
    "display",                      // 10 choice
    "display owner / owned pets"    // 11 choice
};

// operation keys to display in the 1st query menu
static const vector<PetBook::Key> INIT_OPER_NAMES =
{
    OPER_NAMES[1],  // show_all
    OPER_NAMES[2],  // find
    OPER_NAMES[3],  // update
    OPER_NAMES[4],  // add_new
    OPER_NAMES[5],  // delete
    OPER_NAMES[9],  // choose
    OPER_NAMES[8],  // clear
    OPER_NAMES[0]   // exit
};

// operation keys to display in the 2nd query menu -
// after an initial operation has been executed
static const vector<PetBook::Key> SEC_OPER_NAMES =
{
    OPER_NAMES[3],  // update
    OPER_NAMES[4],  // add_new
    OPER_NAMES[5],  // delete
    OPER_NAMES[6],  // filter
    OPER_NAMES[7],  // order
    OPER_NAMES[9],  // choose
    OPER_NAMES[8],  // clear
    OPER_NAMES[0]   // exit
};

// operation keys to display in the internal menu of ChooseEntry()
static const vector<PetBook::Key> CHOICE_OPER_NAMES =
{
    OPER_NAMES[10], // display (find by curr id)
    OPER_NAMES[11], // display owner / owned pets (find joined)
    OPER_NAMES[3],  // update
    OPER_NAMES[5]   // delete
};

///////////////////////////////////////////////////////////////////

PetBook::PetBook(std::shared_ptr<sql::Connection> con)
    :   con(con)
{
	con->setSchema(petBookDB);
    SetCurrPK();
    InitStringGen();
}

void PetBook::InitStringGen()
{
    static const std::vector<StringFunc> stringFuncs =
    {
        [this](){ return Exit();            },
        [this](){ return GetAll();          },
        [this](){ return FindBy();          },
        [this](){ return UpdateField();     },
        [this](){ return AddEntry();        },
        [this](){ return RemoveEntry();     },
        [this](){ return FilterBy();        },
        [this](){ return OrderBy();         },
        [this](){ return ClearSearch();     },
        [this](){ return ChooseEntry();     },
        [this](){ return FindCurrChoice();  },
        [this](){ return FindJoined();      }
    };

    for (size_t i = 0; i < OPER_NAMES.size(); i++)
    {
        stringGen.AddStringFunc(OPER_NAMES[i], stringFuncs[i]);
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
    MakeString();

    while (isRunning)
    {
        if ("" != currQuery)
        {
            PreparedStatement* pstmt = NULL;

            try
            {
                pstmt = con->prepareStatement(currQuery);
                DisplayResults(pstmt->executeQuery());
            }
            catch (sql::SQLException &e)
            {
                cout << "# ERR: " << e.what() << endl;
            }
            
            delete pstmt;
        }

        MakeString();
    }
}

void PetBook::MakeString()
{
    // choose table
    if ("" == currId) // new search
    {
        DisplayTableMenu();

        size_t i;
        cin >> i;

        // handle bad input
        while (cin.fail() || !(1 <= i && i <= NO_TABLE_IDX))
        {
            cin.clear();
            cin.ignore(256,'\n');
            cin >> i;
        }

        if (NO_TABLE_IDX == i)
        {
            Exit();
            return ;
        }
        else
        {
            SetDataTable(DATA_TABLES[i-1]);
        }
    }

    // choose operation
    DisplayOperMenu();
    string key;
    cin >> key;

    // generates a string into currQuery
    stringGen.GenerateString(key);
}

// Result Displayers
void PetBook::DisplayResults(ResultSet* res)
{   
    if ("query" != currId)       // editorial
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
    ResultSet* res = NULL;
    PreparedStatement* pstmt = NULL;

    try
    {
        if ("new_id" == currId)  // new entry added
        {
            pstmt = con->prepareStatement(FindByMaxId());
        }
        else                                // entry edited
        {
            pstmt = con->prepareStatement(FindByCurrId());
        }

        res = pstmt->executeQuery();
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }
    
    delete pstmt;
    return res;
}

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
void PetBook::DisplayTableMenu()
{
    cout << "Search in... (type the number)" << endl;

    size_t i = 0;
    for (auto table : DATA_TABLES)
    {
        cout << ++i << ". " << table << endl;
    }
    cout << ++i << ". exit" << endl;
}

void PetBook::DisplayOperMenu()
{
    cout << "Select an operation by typing it.\n"
    << "Available operations in " << currTable << " are:" << endl;

    // if ("" == currId ||         // entirely new search
    //     "exit" == currQuery)    // new primary oper in same datatable
    if ("" == currId)         // new search
    {
        for (auto name : INIT_OPER_NAMES)
        {
            cout << "- " << name << endl;
        }
    }
    else                // concat queries
    {
        for (auto name : SEC_OPER_NAMES)
        {
            cout << "- " << name << endl;
        }
    }
}

void PetBook::DisplayFieldMenu()
{
    cout << "Enter the parameter to search by or modify. options are:" << endl;

    fields->first();
    cout << "- " << fields->getString("COLUMN_NAME") << endl;
    while (fields->next())
    {
        cout << "- " << fields->getString("COLUMN_NAME") << endl;
    }
}

ResultSet* PetBook::GetFields()
{
    PreparedStatement* pstmt = NULL;
    ResultSet* fields = NULL;
    
    try
    {
        pstmt = con->prepareStatement(
            "select COLUMN_NAME from information_schema.COLUMNS where TABLE_NAME='" + currTable + "'");
        
        fields = pstmt->executeQuery();
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }

    delete pstmt;
    return fields;
}

// StringFuncs ////////////////////////////////////////////////////
const string& PetBook::Exit()
{
    isRunning = false;
    currPK = "";
    stmtString = "";
    return ClearSearch();
}

const string& PetBook::ClearSearch()
{
    currId = "";
    currQuery = "";
    return currQuery;
}

// initial queries
const string& PetBook::FindBy()
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

const string& PetBook::GetAll()
{
    currId = "query";

    currQuery = SelectDataAsc();
    return currQuery;
}

// secondary queries
const string& PetBook::FilterBy()
{
    // enter column to filter by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter value/s to filter by
    string val(""), val2("");
    GetSearchVals(val, val2);

    // add rule to the current query
    const string rule(GetRule(col, val, val2));
    const string getAll(SelectDataAsc());
    if (getAll == currQuery)
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

const string& PetBook::OrderBy()
{
    // enter column to order by
    DisplayFieldMenu();
    string col("");
    cin >> col;

    // enter ASC or DESC to order by
    cout << "Enter the order (ASC / DESC)" << endl;
    string order("");
    cin >> order;

    // add rule to the current query
    if (SelectDataAsc() == currQuery)
    {
        currQuery = SelectData();
    }
    currQuery += " ORDER BY " + col + " " + order;

    return currQuery;
}

const string& PetBook::ChooseEntry()
{
    // choose a single entry and notify future calls
    currId = "";
    while ("" == currId)
    {
        ChooseId();
    }

    if ("" == currId)   // no valid id is chosen
    {
        currQuery = "";
    }
    else
    {
        currQuery = "choose";

        // display menu
        cout << "choose an operation: (type the number)" << endl;
        size_t i = 0;
        for (auto name : CHOICE_OPER_NAMES)
        {
            cout << ++i << ". " << name << endl;
        }

        // receive choice and execute
        size_t key = 0;
        cin >> key;
        stringGen.GenerateString(CHOICE_OPER_NAMES[key - 1]);
    }

    return currQuery;
}

const string& PetBook::FindJoined()
{
    bool abortQuery = false;

    // choose a single entry, if not already chosen
    if ("choose" != currQuery)
    {
        ChooseId();
    }

    if ("" == currId)   // no valid id is chosen
    {
        abortQuery = true;
    }
    else
    {
        // extract the foreign key value of the chosen entry
        const string foreignKey = GetCurrFKVal();

        if ("" == foreignKey)   // entry has no foreign key value
        {
            cout << "no owner or owned pets" << endl;
            currId = "";
            abortQuery = true;
        }
        else
        {
            // identify as a query for result display
            currId = "query";

            // choose an adopter's adopter_id and show all its pets
            if ("adopters" == currTable)
            {
                SetDataTable("pets");
                currQuery = "SELECT * FROM pets WHERE " + FK + "=" + foreignKey;
            }
            // choose a pet's adopter_id and show that adopter
            else if ("pets" == currTable)
            {
                SetDataTable("adopters");
                currQuery = "SELECT * FROM adopters WHERE " + FK + "=" + foreignKey;
            }
            // default option
            else
            {
                abortQuery = true;
            }
        }
    }

    // cleanup
    if (abortQuery)
    {
        currQuery = "";
    }

    return currQuery;
}

const string& PetBook::FindCurrChoice()
{
    currQuery = FindByCurrId();
    return currQuery;
}

// editorial operations
const string& PetBook::UpdateField()
{
    // choose a single entry, if not chosen
    if ("choose" != currQuery)
    {
        ChooseId();
    }

    if ("" == currId)   // no valid id is chosen
    {
        currQuery = "";
    }
    else
    {
        // enter column to update
        DisplayFieldMenu();
        string col("");
        cin >> col;

        // enter new value
        cout << "Enter a new value for the parameter" << endl;
        string val("");
        cin >> val;
        
        currQuery = ("UPDATE " + currTable + " SET " + col + "='" + val + "' WHERE " + currPK + "=" + currId + ";");
    }
    
    return currQuery;
}

const string& PetBook::AddEntry()
{
    currId = "new_id";
    ResetAutoinc();

    cout << "Enter the values for each parameter:" << endl;

    // add columns to statement string
    currQuery = "INSERT INTO " + currTable + " (";
    fields->first();
    if ("pets" == currTable)
    {
        fields->next();    // skip pk column
    }
    currQuery += fields->getString("COLUMN_NAME");
    while (fields->next())
    {
        currQuery += (", " + fields->getString("COLUMN_NAME"));
    }
    currQuery += ") VALUES (";

    // add values from input to statement string
    fields->first();
    if ("pets" == currTable)
    {
        fields->next();    // skip pk column
    }
    cout << fields->getString("COLUMN_NAME") << ": " << flush;
    string val("");
    cin >> val;

    if (isNum(val))
    {
        currQuery += (isNull(val) ? "NULL" : val);
    }
    else
    {
        currQuery += ("'" + val + "'");
    }
    while (fields->next())
    {
        cout << fields->getString("COLUMN_NAME") << ": " << flush;
        cin >> val;
        currQuery += (", ");

        if (isNum(val))
        {
            currQuery += (isNull(val) ? "NULL" : val);
        }
        else
        {
            currQuery += ("'" + val + "'");
        }
    }
    currQuery += ")";

    return currQuery;
}

const string& PetBook::RemoveEntry()
{
    // choose a single entry, if not chosen
    if ("choose" != currQuery)
    {
        ChooseId();
    }

    if ("" == currId)   // no valid id is chosen
    {
        currQuery = "";
    }
    else
    {
        currQuery = ("DELETE FROM " + currTable + " WHERE " +
                        currPK + "=" + currId + ";");        
    }
    
    return currQuery;
}

// helper operations
const string& PetBook::FindByCurrId()
{
    const string select(SelectDataWhere());
    const string rule(GetRule(currPK, currId, ""));
    stmtString = (select + rule);
    return stmtString;
}

const string& PetBook::FindByMaxId()
{
    const string select(SelectDataWhere());
    stmtString = (select + currPK + " = (SELECT MAX(" + currPK + ") FROM " + currTable + ")");
    return stmtString; 
}

// Helper functions ///////////////////////////////////////////////
const string& PetBook::GetRule(const string& col, const string& val, const string& val2)
{
    if (isNum(val))
    {
        if ("" == val2)
        {
            // find by number
            stmtString = (col + (isNull(val) ? " IS NULL" : "=" + val));
        }
        else
        {
            // find by range
            stmtString = (col + " BETWEEN " + val + " AND " + val2);
        }
    }
    else
    {
        if ("" == val2)
        {
            // find by text
            stmtString = (col + " LIKE '%" + val + "%'");
        }
        else
        {
            // find by text range
            stmtString = (col + " BETWEEN '" + val + "' AND '" + val2 +
                                "' OR " + col + " LIKE '%" + val2 + "%'");
        }
    }

    return stmtString;
}

const string& PetBook::SelectData()
{
    stmtString = ("SELECT * FROM " + currTable);
    return stmtString;
}

const string& PetBook::SelectDataWhere()
{
    SelectData();
    stmtString += " WHERE ";
    return stmtString;
}

const string& PetBook::SelectDataAsc()
{
    SelectData();
    stmtString += (" ORDER BY " + currPK + " ASC");
    return stmtString;
}

void PetBook::ChooseId()
{
    // receive an id for a single entry
    cout << "choose an entry and enter its " << currPK << endl;

    currId = "";
    SetCurrId();
    while ("" == currId)
    {
        cout << "wrong id number. to try again, press r.\n"
            << "to go back, press any key."
            << endl;
            
        string key;
        cin >> key;
        if ("r" == key)
        {
            cout << "enter a valid " << currPK << endl;
            SetCurrId();
        }
        else
        {
            return ;
        }
    }
}

void PetBook::SetCurrId()
{
    string id;
    cin >> id;

    if (isNull(id) || !(isNum(id)))
    {
        currId = "";
    }
    else
    {
        currId = id;
    }
}

static bool isNum(const string& val)
{
    char* isText;
    strtol(val.c_str(), &isText, 10);

    return false == *isText;
}

static bool isNull(const string& val)
{
    return 0 == strtol(val.c_str(), NULL, 10);
}

static void GetSearchVals(string& val, string& val2)
{
    cout << "Enter a value for the parameter.\n"
    << "To enter a number range, type the numbers like so:\n"
    << "-r <min> <max>"
    << endl;
    cin >> val;
    if ("-r" == val)
    {
        cin >> val;
        cin >> val2;
    }
}

// Setters ////////////////////////////////////////////////////////
void PetBook::SetDataTable(const string& table)
{
    currTable = table;
    fields = GetFields();
    SetCurrPK();;
}

void PetBook::SetCurrPK()
{
    PreparedStatement* pstmt = NULL;
    ResultSet* res = NULL;

    try
    {
        pstmt = con->prepareStatement(
            "SHOW KEYS FROM " + currTable + " WHERE Key_name = 'PRIMARY';");
        res = pstmt->executeQuery();
        res->next();
        currPK = (res->getString("COLUMN_NAME"));
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }

    delete res;
    delete pstmt;

}

const string& PetBook::GetCurrFKVal()
{
    PreparedStatement* pstmt = NULL;
    ResultSet* res = NULL;

    try
    {
        pstmt = con->prepareStatement(
            "SELECT " + FK + " FROM " + currTable + " WHERE " + currPK + "=" + currId);
        res = pstmt->executeQuery();
        res->next();
        stmtString = (res->getString(FK));
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
        stmtString = "";
    }

    delete res;
    delete pstmt;

    return stmtString;
}

void PetBook::ResetAutoinc()
{
    PreparedStatement* pstmt = NULL;
    ResultSet* res = NULL;

    try
    {
        string stmtStr("SELECT " + currPK + " FROM " + currTable + " WHERE " + currPK + " = (SELECT MAX(" + currPK + ") FROM " + currTable + ")");
        pstmt = con->prepareStatement(stmtStr);
        res = pstmt->executeQuery();

        res->first();
        string maxId = res->getString(currPK);

        stmtStr = ("ALTER TABLE pets AUTO_INCREMENT=" + maxId + ";");
        pstmt = con->prepareStatement(stmtStr);
        res = pstmt->executeQuery();
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }

    delete res;
    delete pstmt;
}

} // namespace ashs