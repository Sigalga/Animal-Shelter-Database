#ifndef ASHS_STMT_STRING_GENERATOR_HPP
#define ASHS_STMT_STRING_GENERATOR_HPP

// Standard C++ includes
#include <string>
#include <map>
#include <utility>  // std::pair

using namespace std;

namespace ashs
{

class StmtStringGenerator
{
public:
    typedef int Key;
    typedef string&(*StringFunc)(const string&, const string&);

    void AddStringFunc(const Key key, const StringFunc func);
    void RemoveStringFunc(const Key key);

    string& GenerateString(const Key key, const string& col, const string& val);

private:
    map<Key, StringFunc> stringFuncs;
};

void StmtStringGenerator::AddStringFunc(const Key key, const StringFunc func)
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

#endif // ASHS_STMT_STRING_GENERATOR_HPP