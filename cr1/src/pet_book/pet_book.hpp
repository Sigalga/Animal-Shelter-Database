#ifndef ASHS_PET_BOOK_HPP
#define ASHS_PET_BOOK_HPP

// Standard C++ includes
#include <vector>

// MySQL/C++ Connector includes
#include <cppconn/connection.h>     // sql::Connection
#include <cppconn/resultset.h>		// sql::ResultSet

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
    ~PetBook() { delete fields; }

    void SetDatabase(const string& db) { petBookDB = db; }

    void Start();
    void Stop() { isRunning = false; }

private:
    Connection* con;        // connection to an SQL server database
    StmtStringGenerator* stringGen;
    string petBookDB;       // name of the SQL database
    bool isRunning;         // interactive ui switch
    string currTable;       // name of the current datatable choice
    string currPK;          // name of primary key field for current datatable
    string currId;          // pk value for entry-specific operations, otherwise "query"/"new_id"/empty
    string currQuery;       // for secondary operations concatenation
    string stmtString;      // for stringFuncs
    ResultSet* fields;      // current datatable's field names

    // Setters /////////////////////////////////////////////////////////////////
    void SetDataTable(const string& table);

    // Extracts the primary key field name from current datatable choice
    void SetCurrPK();

    // Resets autoincrementation of the pk to start from current max
    void ResetAutoinc();

    // Main flow functions /////////////////////////////////////////////////////
    // Adds all the StringFuncs to the String Generator
    void InitStringGen();

    // Receives and handles input from user
    void ExecuteInput();

    // Receives user input and produces an SQL statement string
    const string& MakeString();

    // Menu Displayers /////////////////////////////////////////////////////////
    // Displays a list of datatables and enables to choose one
    void DisplayTableMenu();

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
    void PrintTable(ResultSet* res);

    // StringFuncs /////////////////////////////////////////////////////////////
    // Exists PetBook
    const string& Exit();             
    
    // Starts a new search in the database
    const string& ClearSearch();
    
    // Initial queries ///////////////////////////////////////
    //
    // Searches table by a field and a value, or a range of values
    const string& FindBy();
    //
    // Fetches all table entries in an ascending pk order
    const string& GetAll();
    //
    //////////////////////////////////////////////////////////

    // Secondary queries /////////////////////////////////////
    // May be concatenated to initial/secondary queries
    //
    // Filters an existing serach result set by a field and a value,
    // or a range of values
    const string& FilterBy();
    //
    // Oraders an existing serach result set by a field's value,
    // either in an ascending or descending order
    const string& OrderBy();
    //
    const string& ChooseEntry();
    //
    // Receives an adopter id to show all of its pets
    // or: a pet id to show its adopter
    const string& FindJoined();
    //
    //////////////////////////////////////////////////////////

    // Editorial operations //////////////////////////////////
    // Clear the search when done, and display modified item
    //
    // Replaces current field value with a new input value
    const string& UpdateField();
    //
    // Adds a new table entry with input values
    const string& AddEntry();
    //
    // Removes a table entry by its identifier
    const string& RemoveEntry();
    //
    //////////////////////////////////////////////////////////

    // Helper operations /////////////////////////////////////
    //
    // Rereives the entry most recently edited by PetBook
    const string& FindByCurrId();
    //
    // Rereives the entry most recently added to the table
    const string& FindByMaxId();
    //
    // A query start phrase, retreives all table data
    string& SelectData();
    //
    // A query start phrase, to be followed by a rule
    string& SelectDataWhere();
    //
    // A query start phrase, retreives all table data
    // ordered by its pk in an ascending order
    string& SelectDataAsc();
    //
    // A rule phrase for a query, to follow a start phrase
    string& GetRule(const string& col, const string& val, const string& val2);
    //
    // Sets currId to a user input
    void SetCurrId();
    //
    // Gets the foreign key value of an entry identified by currId
    string& GetCurrFKVal();
    //
    //////////////////////////////////////////////////////////

    friend class PbTestClass;
};

} // namespace ashs

#endif // ASHS_PET_BOOK_HPP