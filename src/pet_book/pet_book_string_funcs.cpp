
#include "pet_book_string_funcs.hpp"

using namespace std;

namespace ashs
{

static bool isNum(const string& val);

static const string findBy("SELECT * FROM pets WHERE ");

// Queries ///////////////////////////////////////////////////////

string& FindBy(const string& col, const string& val)
{
    string* str = new string(findBy);

    // check if the value is a number
    if (isNum(val))
    {
        // find by number
        *str += (col + "=" + val);
    }
    else
    {
        // find by text
        *str += (col + " LIKE '%" + val + "%'");
    }
    
    return *str;
}

static bool isNum(const string& val)
{
    char* isText;
    strtol(val.c_str(), &isText, 10);

    return false == *isText;
}

// string& OrderBy(const string& col, const string& order)
// {

// }

// // edit operations
// string& AddEntry(const string& val1, const string& val2)
// {
//     // the id of the entry must be the next free id number
//     // example:
//     // "INSERT INTO pets VALUES (val1, val2, val3)"
// }

// string& UpdateField(const string& col, const string& val)
// {
//     // example:
//     // "UPDATE pets SET col=val WHERE pk=id"
// }

// string& RemoveEntry(const string& null, const string& id)
// {
//     // example:
//     // "DELETE FROM pets WHERE pk=id"
// }

} // namespace ashs