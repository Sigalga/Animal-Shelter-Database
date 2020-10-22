#ifndef ASHS_PET_BOOK_HPP
#define ASHS_PET_BOOK_HPP

// Standard C++ includes
#include <vector>
#include <memory>	// smart ptrs

// MySQL/C++ Connector includes
#include <cppconn/connection.h>     // sql::Connection
#include <cppconn/resultset.h>		// sql::ResultSet

// ASHS includes
#include "stmt_string_generator.hpp"

namespace ashs
{

static const std::string PET_BOOK_DB("pet_book");
static const std::string DEFAULT_TABLE("pets");

class PetBook
{
public:
    using Key =         StmtStringGenerator::Key;
    using StringFunc =  StmtStringGenerator::StringFunc;

    PetBook(std::shared_ptr<sql::Connection> con);

    ~PetBook() { delete fields; }

    PetBook(const PetBook&) = delete;
    PetBook& operator=(const PetBook&) = delete;

    // Sets an SQL database
    void SetDatabase(const std::string& db) { petBookDB = db; }

    // Starts the interactive io loop, which allowes the user
    // to send queries and actions to the SQL database
    void Start();

    // Stops the interactive io loop
    void Stop() { isRunning = false; }

private:
    // animal shelter system resources
    std::shared_ptr<sql::Connection> con;   // ashs connection to an SQL server database
    // std::shared_ptr<StmtStringGenerator> stringGen; // statement string generator
    StmtStringGenerator stringGen; // statement string generator
    
    // petbook resources
    std::string petBookDB = PET_BOOK_DB;    // name of the SQL database
    bool isRunning = false;                 // interactive ui switch
    std::string currTable = DEFAULT_TABLE;  // name of the current datatable choice
    std::string currPK = "";                // name of primary key field for current datatable
    std::string currId = "";                // pk value for entry-specific operations, otherwise "query"/"new_id"/empty
    std::string currQuery = "";             // for secondary operations concatenation
    std::string stmtString = "";            // for stringFuncs
    sql::ResultSet* fields = GetFields();   // current datatable's field names

    // Setters /////////////////////////////////////////////////////////////////
    void SetDataTable(const std::string& table);

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
    void MakeString();

    // Menu Displayers /////////////////////////////////////////////////////////
    // Displays a list of datatables and enables to choose one
    void DisplayTableMenu();

    // Displays a list of all operations possible via ui
    void DisplayOperMenu();

    // Displays a list of all table fields
    void DisplayFieldMenu();

    // Gets the names of table's fields
    sql::ResultSet* GetFields();

    // Result Displayers ///////////////////////////////////////////////////////
    // Prints a database ResultSet for a query
    void DisplayResults(sql::ResultSet* res);

    // Gets the Resultset for a recently edited/added entry
    sql::ResultSet* GetEditResult();

    // Prints a Resultset in a table view by <fields>
    void PrintTable(sql::ResultSet* res);

    // StringFuncs /////////////////////////////////////////////////////////////
    // Exists PetBook
    const std::string& Exit();             
    
    // Starts a new search in the database
    const std::string& ClearSearch();
    
    // Initial queries ///////////////////////////////////////
    //
    // Searches table by a field and a value, or a range of values
    const std::string& FindBy();
    //
    // Fetches all table entries in an ascending pk order
    const std::string& GetAll();
    //
    //////////////////////////////////////////////////////////

    // Secondary queries /////////////////////////////////////
    // May be concatenated to initial/secondary queries
    //
    // Filters an existing serach result set by a field and a value,
    // or a range of values
    const std::string& FilterBy();
    //
    // Orders an existing serach result set by a field's value,
    // either in an ascending or descending order
    const std::string& OrderBy();
    //
    // Receives a primary key of an entry in the table
    // to perform further operations on it
    const std::string& ChooseEntry();
    //
    // Receives an adopter id to show all of its pets,
    // or a pet id to show its adopter
    const std::string& FindJoined();
    //
    // Presents a single, currently chosen entry,
    // which had been chosen with ChooseEntry().
    const std::string& FindCurrChoice();
    //
    //////////////////////////////////////////////////////////

    // Editorial operations //////////////////////////////////
    // Clear the search when done, and display modified item
    //
    // Replaces current field value with a new input value
    const std::string& UpdateField();
    //
    // Adds a new table entry with input values
    const std::string& AddEntry();
    //
    // Removes a table entry by its identifier
    const std::string& RemoveEntry();
    //
    //////////////////////////////////////////////////////////
    
    // Helper operations /////////////////////////////////////
    //
    // Rereives the entry most recently edited by PetBook
    const std::string& FindByCurrId();
    //
    // Rereives the entry most recently added to the table
    const std::string& FindByMaxId();
    //
    // A query start phrase, retreives all table data
    const std::string& SelectData();
    //
    // A query start phrase, to be followed by a rule
    const std::string& SelectDataWhere();
    //
    // A query start phrase, retreives all table data
    // ordered by its pk in an ascending order
    const std::string& SelectDataAsc();
    //
    // A rule phrase for a query, to follow a start phrase
    const std::string& GetRule(const std::string& col, const std::string& val, const std::string& val2);
    //
    // Calls SetCurrId() and displays an option menu in case of wrong input 
    void ChooseId();
    //
    // Sets currId to a user input. If input is invalid, sets to "".
    void SetCurrId();
    //
    // Gets the foreign key value of an entry identified by currId
    const std::string& GetCurrFKVal();
    //
    //////////////////////////////////////////////////////////

    friend class PbTestClass;
};

} // namespace ashs

#endif // ASHS_PET_BOOK_HPP