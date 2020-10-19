#ifndef ASHS_STMT_STRING_GENERATOR_HPP
#define ASHS_STMT_STRING_GENERATOR_HPP

// Standard C++ includes
#include <string>
#include <map>
#include <vector>

#include <functional>		    // std::function

namespace ashs
{

class StmtStringGenerator
{
public:
    using Key =         std::string;
    using StringFunc =  std::function<const std::string&()>;
    using FuncMap =     std::map<Key, StringFunc>;

    // Adds a new StringFunc to the map with a unique key.
    // Passing an occupied key will result in no action.
    void AddStringFunc(const Key key, StringFunc func);
    void AddStringFunc(const char* cStr, StringFunc func);

    // Removes a StringFunc selected by key.
    // passing a non existing key will result in no action.
    void RemoveStringFunc(const Key key);

    // Generates a string by executing a StringFunc, selected by key.
    // Passing an non-existing key will result in no action.
    void GenerateString(const Key key);

    // Returns a vector of all stringFuncs keys
    std::vector<Key> GetKeys();

private:
    FuncMap stringFuncs;
};

} // namespace ashs

#endif // ASHS_STMT_STRING_GENERATOR_HPP