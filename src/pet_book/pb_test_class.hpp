#ifndef ASHS_PB_TEST_CLASS_HPP
#define ASHS_PB_TEST_CLASS_HPP

#include "pet_book.hpp"

namespace ashs
{

class PbTestClass
{
public:
    PbTestClass(PetBook* petBook) : instance(petBook) {}

    void StartTest();
    void PrivateMethodsTest();
    size_t ExecutInputTest();
    size_t MakeStringTest();
    size_t StringFuncsTest();

private:
    PetBook* instance;

    // StringFunc Tests //////////////
    size_t ExitTest();
    size_t ClearSearchTest();

    // Initial queries
    size_t FindByTest();
    size_t GetAllTest();

    // Secondary queries
    size_t FilterByTest();
    size_t OrderByTest();
    size_t ChooseEntryTest();
    size_t FindJoinedTest();

    // Editorial operations
    size_t UpdateFieldTest();
    size_t AddEntryTest();
    size_t RemoveEntryTest();

    // Helper operations
    size_t FindByCurrIdTest();
    size_t FindByMaxIdTest();
    size_t SelectDataTest();
    size_t SelectDataWhereTest();
    size_t SelectDataAscTest();
    size_t GetRuleTest();
};

} // namespace ashs

#endif // ASHS_PB_TEST_CLASS_HPP