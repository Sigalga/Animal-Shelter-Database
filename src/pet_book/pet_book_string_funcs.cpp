
#include "pet_book_string_funcs.hpp"

using namespace std;

namespace ashs
{

static bool isNum(const string& val);

static const string g_findBy("SELECT * FROM pets WHERE ");
static const string g_pk("pet_id");

static string& GetRule(const string& col, const string& val, const string& val2);

// Queries ///////////////////////////////////////////////////////

// string& FindBy(const string& col, const string& val, const string& val2)
// {
//     string* str = new string(g_findBy);

//     if (isNum(val))
//     {
//         if ("" == val2)
//         {
//             // find by number
//             *str += (col + "=" + val);
//         }
//         else
//         {
//             // find by range
//             *str += (col + " BETWEEN " + val + " AND " + val2);
//         }
//     }
//     else
//     {
//         // find by text
//         *str += (col + " LIKE '%" + val + "%'");
//     }
    
//     return *str;
// }

string& FindBy(const string& col, const string& val, const string& val2)
{
    string* str = new string(g_findBy + GetRule(col, val, val2));
    return *str;
}

string& FilterBy(const string& col, const string& val, const string& query)
{
    string* str = new string(query);

    // add rule
    *str += " AND " + GetRule(col, val, "");

    return *str;
}

// string& OrderBy(const string& col, onst string& order)
// {

// }

// // edit operations
// string& AddEntry(const string& val1, const string& val2)
// {
//     // the id of the entry must be the next free id number
//     // example:
//     // "INSERT INTO pets VALUES (val1, val2, val3)"
// }

string& UpdateField(const string& col, const string& val, const string& id)
{
    string* str = new string("UPDATE pets SET ");
    *str += (col + "='" + val + "' WHERE " + g_pk + "=" + id + ";");

    return *str;
}

// string& RemoveEntry(const string& null, const string& id)
// {
//     // example:
//     // "DELETE FROM pets WHERE pk=id"
// }

// Helper functions //////////////////////////////////////////

static string& GetRule(const string& col, const string& val, const string& val2)
{
    string* rule = new string;
    
    if (isNum(val))
    {
        if ("" == val2)
        {
            // find by number
            *rule = (col + "=" + val);
        }
        else
        {
            // find by range
            *rule = (col + " BETWEEN " + val + " AND " + val2);
        }
    }
    else
    {
        // find by text
        *rule = (col + " LIKE '%" + val + "%'");
    }

    return *rule;
}

static bool isNum(const string& val)
{
    char* isText;
    strtol(val.c_str(), &isText, 10);

    return false == *isText;
}

} // namespace ashs