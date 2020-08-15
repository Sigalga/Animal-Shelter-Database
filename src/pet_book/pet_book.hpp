#ifndef ASHS_PET_BOOK_HPP
#define ASHS_PET_BOOK_HPP

// MySQL/C++ Connector includes
#include <cppconn/connection.h>     // sql::Connection

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
    void SetDatabase(const string& db) { petBookDB = db; }

    void Start();

private:
    Connection* con;
    StmtStringGenerator* stringGen;
    string petBookDB;

    // Displays welcome menu for the interactive ui
    void DisplayMenu();

    // Receives and handles input from user
    void ExecuteRequests();

    // Prints a database resultset for a query
    void DisplayResults();

    // Displays a list of all operations possible via ui
    void DisplayStringFuncs();

    // Receives user input and produces an SQL statement string
    string& MakeString();
    
};

} // namespace ashs

#endif // ASHS_PET_BOOK_HPP