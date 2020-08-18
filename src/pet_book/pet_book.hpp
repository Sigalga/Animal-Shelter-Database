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

    void Start();
    void Stop() { isRunning = false; }

    // StringFuncs
    string& Exit();
    string& FindBy();
    string& FilterBy();
    string& UpdateField();
    string& FindByCurrId();
    string& GetAll();

private:
    Connection* con;
    StmtStringGenerator* stringGen;
    string petBookDB;
    bool isRunning;
    string currId;
    string currQuery;

    // General flow functions //////////////////////////////////////////////////
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
};


} // namespace ashs

#endif // ASHS_PET_BOOK_HPP