#include "../src/string.hpp"

#include <iostream>

using std::cout, std::endl;


int main() {
    String str1 = String("Hello,");
    String str2 = String("World!");

    cout << "Values of strings:\n1. " << str1 << "\n" << "2. " << str2 << endl;
}
