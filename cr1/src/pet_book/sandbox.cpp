#include <bits/stdc++.h> // std::sort
#include <vector>
#include <memory> //unique_ptr
#include <iostream>

using namespace std;

// void printVector(vector<int> v);
// vector<int> v = {1, 2, 3};

// bool IsNull(const string& val);

int main()
{
    // create smart ptr + create an instance on stack
    auto p1 = make_unique<int>(3);
    cout << *p1 << endl;

    // create instance on stack and a smart ptr to it
    unique_ptr<int> p2(new int(4));
    cout << *p2 << endl;

    // moving ownership to a new unique ptr
    unique_ptr<int> p3;
    p3 = move(p2);  // no copy operator, p2 is now NULL
    cout << *p3 << endl;

    // moving ownership to ptr ctor
    unique_ptr<int> p4 = move(p3); // no cctor, p3 is now NULL
    cout << *p4 << endl;

    // another optional syntax
    auto p5 = move(p4);
    cout << *p5 << endl;

    auto p6(make_unique<int>(3));



    // sort(v.begin(),
    //     v.end(),
    //     [](const int& a, const int& b) -> bool { return a > b; });
    // printVector(v);

    // cout << IsNull("abc") << endl;
    // cout << IsNull("54") << endl;
    // cout << IsNull("0") << endl;
    // cout << IsNull("00000000000") << endl;
    // cout << IsNull("-7") << endl;

    return 0;
}

// bool IsNull(const string& val)
// {
//     return 0 == strtol(val.c_str(), NULL, 10);
// }

// // Function to print vector 
// void printVector(vector<int> v) 
// { 
//     // lambda expression to print vector 
//     for_each(v.begin(), v.end(), [](int i) 
//     { 
//         std::cout << i << " "; 
//     }); 
//     cout << endl; 
// } 