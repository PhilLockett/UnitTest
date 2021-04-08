/**
 * @file    test.cpp
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Unit test code Implementation.
 *
 * Build using:
 *    g++ -std=c++20 -c -o test.o test.cpp
 *    g++ -std=c++20 -c -o unittest.o unittest.cpp
 *    g++ -std=c++20 -o test test.o unittest.o
 *
 * Test using:
 *    ./test
 *
 *  Should generate 1 error.
 */

#include <iostream>
#include <sstream>

#include "unittest.h"



/**
 * @section Multicase test.
 */
UNIT_TEST(test0, "Test verbose output.")

    VERBOSE_ON
    REQUIRE(IS_VERBOSE)

NEXT_CASE(test1, "Test verbose output disabled.")

    VERBOSE_OFF
    REQUIRE(!IS_VERBOSE)

NEXT_CASE(test2, "Test verbose output again.")

    VERBOSE_ON
    REQUIRE(IS_VERBOSE)

END_TEST


int getSomeValue(void)
{
    return 10;
}
/**
 * @section single test case.
 */
UNIT_TEST(test3, "Test REQUIRE macro - first test should pass, second test should fail.")
    // Test "pass" case.
    REQUIRE(getSomeValue() == 10)

    // Test "fail" case.
    REQUIRE(getSomeValue() == 11)

END_TEST


int runTests(void)
{
    std::cout << "Executing all tests.\n";

    RUN_TEST(test0)
    RUN_TEST(test3)

    const int err = ERROR_COUNT;
    if (err)
        std::cerr << err << " ERROR(S) encountered!.\n";
    else
        std::cout << "All tests passed.\n";

    std::cout << "Note: we force an error to tet the fail case, so error count should be 1.\n";

    return err;
}

/**
 * Test system entry point.
 *
 * @param  argc - command line argument count.
 * @param  argv - command line argument vector.
 * @return error value or 0 if no errors.
 */
int main(int argc, char *argv[])
{
    return runTests();
}

