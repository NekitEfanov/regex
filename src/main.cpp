#include "regex.hpp"

#include <iostream>

using namespace std;


int main() {

    string pattern_str;
    string str(256, '\0');

    cout << "Input string: ";
    cin.getline(const_cast<char*>(str.data()), 256);

    cout << "Pattern: ";
    cin >> pattern_str;

    cout << endl;

    shared_ptr<regex> pattern;
    try {
        pattern.reset(new regex(pattern_str));
    }
    catch (const exception &ex) {
        cout << "Error: " << ex.what() << endl;
        return -1;
    }

    int i = 0;
    regex_result result;

    while (regex_search(str, result, *pattern)) {
        std::cout << "Match " << i++ << ": " << result.str() << std::endl;
        str = result.suffix();
    }

    return 0;
}