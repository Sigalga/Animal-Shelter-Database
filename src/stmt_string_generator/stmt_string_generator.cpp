#include "stmt_string_generator.hpp"

using namespace std;

namespace ashs
{

void StmtStringGenerator::AddStringFunc(const Key key, StringFunc func)
{ 
   stringFuncs.insert(pair<Key, StringFunc>(key, func));
}

void StmtStringGenerator::RemoveStringFunc(const Key key)
{
    try
    {
        stringFuncs.erase(key);
    }
    catch(const std::out_of_range& e)
    {
    }
}

string& StmtStringGenerator::GenerateString(const Key key, const string& col, const string& val)
{
    try
    {
        return stringFuncs.at(key)(col, val);
    }
    catch(const std::out_of_range& e)
    {
        return *(new string(""));
    }
}

} // namespace ashs