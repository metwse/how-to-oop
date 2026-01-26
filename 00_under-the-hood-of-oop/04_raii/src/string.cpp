#include "string.hpp"

#include <cstring>
#include <iostream>

using std::ostream, std::cout, std::endl;


String::String(const char *str) {
    data = new char[strlen(str) + 1];
    strcpy(data, str);

    cout << "String created: " << data << endl;
}

String::~String() {
    cout << "String destroyed: " << data << endl;

    delete[] data;
}

ostream &operator<<(ostream &os, const String &string) {
    os << string.data;

    return os;
}
