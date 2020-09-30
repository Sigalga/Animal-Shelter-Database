#ifndef ASHS_PB_TEST_CLASS_HPP
#define ASHS_PB_TEST_CLASS_HPP

#include "pet_book.hpp"

namespace ashs
{

class PbTestClass
{
public:
    PbTestClass(PetBook* petBook) : instance(petBook) {}

    void StringFuncsTest();
    void PrivateMethodsTest();
    void PublicMethodsTest();

private:
    PetBook* instance;

    // Public Methods Tests //////////
    void StartTest();
    void StopTest();
    void SetDatabaseTest();

    // Private Methods Tests //////////
    size_t ExecutInputTest();
    size_t MakeStringTest();

    // MakeStringTest Internal Tests
    size_t MakeFindTest();
    size_t MakeFilterTest();
    size_t MakeOrderTest();
    size_t MakeUpdateTest();
    size_t MakeAddTest();
    size_t MakeRemoveTest();
    size_t MakeFindJoinedTest();
    size_t MakeFindCurrIdTest();

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