#include <bits/stdc++.h> // std::sort
#include <vector>

using namespace std;

// void printVector(vector<int> v);
// vector<int> v = {1, 2, 3};

bool IsNull(const string& val);

int main()
{
    // sort(v.begin(),
    //     v.end(),
    //     [](const int& a, const int& b) -> bool { return a > b; });
    // printVector(v);

    cout << IsNull("abc") << endl;
    cout << IsNull("54") << endl;
    cout << IsNull("0") << endl;
    cout << IsNull("00000000000") << endl;
    cout << IsNull("-7") << endl;

    return 0;
}

bool IsNull(const string& val)
{
    return 0 == strtol(val.c_str(), NULL, 10);
}


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