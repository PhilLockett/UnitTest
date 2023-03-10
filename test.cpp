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
 * Test the unit test code Implementation.
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

#include "unittest.h"


/**
 * @section Multicase test.
 */
UNIT_TEST(test0, "Test verbose output.")

    VERBOSE_ON
    REQUIRE(IS_VERBOSE)

NEXT_CASE(test1, "Test disabling verbose output.")

    VERBOSE_OFF
    REQUIRE(!IS_VERBOSE)

NEXT_CASE(test2, "Test enabling verbose output again.")

    VERBOSE_ON
    REQUIRE(IS_VERBOSE)

END_TEST


/**
 * @section single test case.
 */
static const int SOMEVALUE{10};
int getSomeValue(void) { return SOMEVALUE; }
int dummyValues[] { 0, 1, 2, 3, 4, 5, 6 };

UNIT_TEST(test3, "Test REQUIRE macro - test should pass.")
    // Test "pass" case.
    REQUIRE(getSomeValue() == SOMEVALUE)
    REQUIRE(ERROR_COUNT == 0)

NEXT_CASE(test4, "Test REQUIRE macro - test should fail.")
    // Test "fail" case.
    REQUIRE(getSomeValue() == SOMEVALUE+1)
    REQUIRE(ERROR_COUNT == 1)

END_TEST

UNIT_TEST(test5, "Test setting tolerance low - may generate a 'too slow' message.")

    SET_TOLERANCE(0.01)
    REQUIRE(getSomeValue() == SOMEVALUE)

END_TEST

UNIT_TEST(test6, "Test disabling timing by setting tolerance to zero.")

    TIMINGS_OFF
    REQUIRE(getSomeValue() == SOMEVALUE)

END_TEST

UNIT_TEST(test7, "Test enabling timing by setting tolerance back to non-zero.")

    TIMINGS_ON
    REQUIRE(getSomeValue() == SOMEVALUE)

END_TEST

UNIT_TEST(test8, "Test displaying test case state.")

    std::cout << UnitTest_c::getInstance();

END_TEST

UNIT_TEST(test9, "Dummy Test 1 affected by argv[1].")

    TIMINGS_OFF
    REQUIRE(dummyValues[1] == 1)
    REQUIRE(dummyValues[2] == 2)

NEXT_CASE(test10, "Dummy Test 1 affected by argv[2].")

    REQUIRE(dummyValues[2] == 2)
    REQUIRE(dummyValues[3] == 3)

END_TEST

UNIT_TEST(test11, "Dummy Test 2 affected by argv[3].")

    REQUIRE(dummyValues[3] == 3)

END_TEST

int runTests(void)
{
    std::cout << "Executing all tests.\n";

    RUN_TEST(test0)
    RUN_TEST(test3)
    RUN_TEST(test5)
    RUN_TEST(test6)
    RUN_TEST(test7)
    RUN_TEST(test8)
    RUN_TEST(test9)
    RUN_TEST(test11)

    std::cout << "\n";
    const int err = FINISHED;
    OUTPUT_SUMMARY;

    std::cout << "\nNote: we force an error in Test4 to get the fail case, so error count should be 1.\n\n";

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
    for (int a{1}; a < argc; ++a)
        dummyValues[a] = atoi(argv[a]);

    return (runTests()==1 ? 0 : 1);
}

