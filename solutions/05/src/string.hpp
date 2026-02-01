/**
 * @file string.hpp
 * @brief Custom string implementation.
 */

#ifndef STRING_HPP
#define STRING_HPP


#include <ostream>


/**
 * @brief Custom string class implementation as a RAII example.
 */
class String {
public:
    /**
     * @brief Construct String from str literal.
     *
     * @see https://en.cppreference.com/w/cpp/language/string_literal.html
     */
    String(const char *str);

    ~String();

private:
    // We will discuss operator overloading in detail. Here is a quick
    // reference if you want to be familiar with it beforehand:
    // https://www.geeksforgeeks.org/cpp/operator-overloading-cpp/
    //
    // In the readme, we defined a function called c_str that returned a
    // private `data` field, and we only used it to print underlying `char *`
    // to cout. Instead of leaking private field, now we define a function only
    // for printing String via output stream (e.g. cout).
    friend std::ostream& operator<<(std::ostream& stream,
                                    const String& matrix);

    char *data;
};


#endif
