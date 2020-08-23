#ifndef ASHS_PET_BOOK_HPP
#define ASHS_PET_BOOK_HPP

// Standard C++ includes
#include <vector>

// MySQL/C++ Connector includes
#include <cppconn/connection.h>     // sql::Connection
#include <cppconn/resultset.h>		    // sql::ResultSet

// ASHS includes
#include "stmt_string_generator.hpp"

using namespace sql;
using namespace std;

namespace ashs
{

class PetBook
{
public:
    using Key = StmtStringGenerator::Key;
    using StringFunc = StmtStringGenerator::StringFunc;

    PetBook(Connection* con, StmtStringGenerator* stringGen);
    ~PetBook() {}

    void SetDatabase(const string& db) { petBookDB = db; }
    const string& GetCurrPK();

    void Start();
    void Stop() { isRunning = false; }

    

private:
    Connection* con;
    StmtStringGenerator* stringGen;
    string petBookDB;
    bool isRunning;
    string currTable;   // for datatable choice
    string currId;      // for update operations, otherwise "query" or empty
    string currQuery;   // for secondary operations concatenation

    // Main flow functions /////////////////////////////////////////////////////
    // Adds all the StringFuncs to the String Generator
    void InitStringGen();

    // Receives and handles input from user
    void ExecuteInput();

    // Receives user input and produces an SQL statement string
    string& MakeString();

    // Menu Displayers /////////////////////////////////////////////////////////
    // Displays a list of all operations possible via ui
    void DisplayOperMenu();

    // Displays a list of all table fields
    void DisplayFieldMenu();

    // Gets the names of table's fields
    ResultSet* GetFields();

    // Result Displayers ///////////////////////////////////////////////////////
    // Prints a database resultset for a query
    void DisplayResults(ResultSet* res);

    // Gets the Resultset for a recently edited/added entry
    ResultSet* GetEditResult();

    // Prints a Resultset in a table view by <fields>
    void PrintTable(ResultSet* res, ResultSet* fields);

    // StringFuncs /////////////////////////////////////////////////////////////
    string& Exit();

    // forget all initial/concatenated queries
    string& ClearSearch();

    // initial queries
    string& FindBy();
    string& GetAll();

    // secondary queries
    // may be concatenated to initial/secondary queries
    string& FilterBy();
    string& OrderBy();

    // editorial operations
    // clear the search when done, modified item is displayed
    string& UpdateField();
    string& AddEntry();
    string& RemoveEntry(); // TO DO

    // helper operations
    string& FindByCurrId();
    string& FindByMaxId();
    const string& GetData() { return *(new string("SELECT * FROM " + currTable)); }
    // const string& GetDataWhere() { return GetData() + " WHERE "; }
    const string& GetDataAsc() { return *(new string(GetData() + " ORDER BY " + GetCurrPK() + " ASC")); }
};


} // namespace ashs

#endif // ASHS_PET_BOOK_HPP