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
    ~PetBook();

    void SetDatabase(const string& db) { petBookDB = db; }

    void Start();
    void Stop() { isRunning = false; }

private:
    Connection* con;
    StmtStringGenerator* stringGen;
    string petBookDB;
    bool isRunning;
    string currId;
    string currQuery;

    void InitStringGen();

    // Displays a list of all operations possible via ui
    void DisplayOperMenu();

    // Displays a list of all table fields
    void DisplayFieldMenu();

    // Receives and handles input from user
    void ExecuteInput();

    // Prints a database resultset for a query
    void DisplayResults(ResultSet* res);

    // Prints the entire pets table ordered by ascending pet id
    void DisplayAll();


    ResultSet* GetFields();

    // Receives user input and produces an SQL statement string
    string& MakeString();

        // StringFuncs
    string& Exit(const string& a="", const string& b="", const string& c="")
    {
        isRunning = false;
        return const_cast<string&>(a);
    }
};


} // namespace ashs

#endif // ASHS_PET_BOOK_HPP