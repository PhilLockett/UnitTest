/**
 * @file    unittest.h
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
 * A simple, lightweight unit test 'framework' implemented as a singleton.
 */
/*
Overview
A unit test framework is useful for regression testing, providing a level of
confidence in code proportional to the test coverage of code. This lightweight
implementation is intended for easy use with test code for quick confirmation
that bugs have not been introduced.

Generated files
When the test cases are run, three files are generated: "timings.txt", 
"results.txt" and "profile.txt".

"timings.txt" lists the duration for each test case executed. If the file does
not exist it is generated. If it does exist, it is read and used to compare
with the latest test run. This helps indicate if code changes have introduced
a performance hit.

"results.txt" lists the number of errors each test case has found. This file is
read on start up and is generated with the latest results every time the tests
are run. If OUTPUT_SUMMARY is selected, the previous and latest results are
compared and any changes are displayed indicating whether any test cases have
better or worse results. This helps when there are a large number of tests with
a significant percentage failing. This is useful for Test Driven Developement.

"profile.txt" lists the assertions made by each test case. This file is
generated every time the tests are run.

Test progress display
The macros VERBOSE_ON and VERBOSE_OFF control whether output is displayed as
the tests are performed. The verbose state can be tested using the macro
IS_VERBOSE which returns a boolean. Calling VERBOSE_OFF makes the tests run
quietly, only displaying errors and timing warnings. Verbose in on by default.

Disabling profiling
By default, profiling is enabled. However, for test cases that iterate over
large collections making many assertions to "profile.txt" can be very large.
The macros PROFILE_OFF and PROFILE_ON can be used to control this.

Defining test cases
A test case is defined between the macro UNIT_TEST(func, desc) and END_TEST.
The parameter 'func' is a function compatible name for the test case and is
used (by RUN_TEST) to call the test case. The parameter 'desc' is a one line
description of the test case. Both 'func' and 'desc' are displayed when
running the tests if IS_VERBOSE is true.

If a test case follows directly on from a previous test case and requires one
or more test cases to have been run, NEXT_CASE(func, desc) can be used as a
continuation of the test. This serves more as a way to split large tests into
smaller chunks. Again 'func' is a function compatible name and 'desc' is a one
line description and are displayed when running the tests. However, 'func' 
should not be called.

Assertions
The macro REQUIRE(cond) is the whole point of this. The parameter 'cond' is
a compilable condition that must be met by the test. Within each unit test
case, code under test should be executed then all appropriate conditions 
should be tested for. If the condition does not return true, an error message
will be displayed and the error count will be incremented.

Running test cases
To run the test cases use the RUN_TEST(func) macro on all 'func's defined by
the UNIT_TEST macro only. Do not try to run the 'func's defined by the 
NEXT_CASE macros as this will cause a compile error.

Error count
The current error count can be obtained with the ERROR_COUNT macro.

Test completion
Once all tests have been run, the FINISHED macro must be called. FINISHED also
returns the error count, which is the number of failed assertions. The error
count should be 0, if not, an error has been introduced.

Calling OUTPUT_SUMMARY displays any progress by comparing the previous and 
latest results and indicating whether any test cases now have better or worse
results as well as displaying the error count.

Note on timings
The timings displayed are a rough indication to whether performance has been
adversely affected by a code change. The code uses the file "timings.txt" to
track previous test case timings. If the duration of a test case has increased
by more than 25% (default) a message is displayed, but it is not counted as an
error. Deleting "timings.txt" will cause this file to be regenerated when the
tests are next run.

The default 25% tolerance can be adjusted by calling the SET_TOLERANCE(value)
macro. The 'value' parameter is specified as a decimal, so for example, an
acceptable deviation of 25% would be set by calling SET_TOLERANCE(0.25).

Time checking can be disabled with the macro TIMINGS_OFF or by setting the
tolerance to 0.0. Time checking can be enabled with the macro TIMINGS_ON or by
setting the tolerance to a positive non zero value.

Also note that any change to the test names, i.e. the value of 'func' used in
the call to UNIT_TEST, "timings.txt" will need to be deleted so that the
timings for the new 'func' names can be calculated.

**If precise timing is required, performance tools should be used.**

Cloning
To clone this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/UnitTest.git
*/

#if !defined(_UNITTEST_H__20210324_0940__INCLUDED_)
#define _UNITTEST_H__20210324_0940__INCLUDED_

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <tuple>

/**
 * @section unit test macro definitions.
 *
 * Implementation of the unit test macros.
 */

#define VERBOSE_ON UnitTest_c::getInstance().setVerbose(true);
#define VERBOSE_OFF UnitTest_c::getInstance().setVerbose(false);
#define IS_VERBOSE (UnitTest_c::getInstance().isVerbose())

#define PROFILE_ON UnitTest_c::getInstance().setProfiling(true);
#define PROFILE_OFF UnitTest_c::getInstance().setProfiling(false);

#define DEFAULT_TOLERANCE (0.25f)
#define SET_TOLERANCE(value) UnitTest_c::getInstance().setTolerance(value);

#define TIMINGS_OFF     SET_TOLERANCE(0.0)
#define TIMINGS_ON      SET_TOLERANCE(DEFAULT_TOLERANCE)

#define UNIT_TEST(func, desc) void func(void) {\
    UnitTest_c::getInstance().progress(#func, desc);

#define NEXT_CASE(func, desc) \
    UnitTest_c::getInstance().complete();\
    UnitTest_c::getInstance().progress(#func, desc);

#define END_TEST \
    UnitTest_c::getInstance().complete();\
}

#define REQUIRE(cond) { UnitTest_c::getInstance().checking(#cond); \
    if (!(cond)) UnitTest_c::getInstance().failure(__FILE__, __LINE__); }

#define RUN_TEST(func)    func();

#define ERROR_COUNT UnitTest_c::getInstance().getErrorCount()

#define FINISHED UnitTest_c::getInstance().finished()

#define OUTPUT_SUMMARY UnitTest_c::getInstance().summary();

/**
 * @section unit test context data.
 *
 * Implementation of the unit test context Singleton.
 */

class UnitTest_c
{
private:
//- Hide the default constructor and destructor.
    UnitTest_c(void) { retrieve(); }
    virtual ~UnitTest_c(void) {}

    void display(std::ostream &os) const;

    static std::string testCase;
    static std::string description;
    static std::string condition;
    static bool update;
    static bool verbose;
    static bool profiling;
    static size_t errors;
    static float tolerance;

    static std::chrono::time_point<std::chrono::steady_clock> start;

    static std::unordered_map<std::string, std::chrono::nanoseconds> times;
    static std::unordered_map<std::string, size_t> counts;
    static std::unordered_map<std::string, size_t> errorList;
    static std::vector<std::pair<std::string, std::string>> assertList;

    static bool store(void);
    static bool retrieve(void);
    static std::chrono::nanoseconds getTime(const std::string & key);
    static bool setTime(const std::string & key, std::chrono::nanoseconds value);
    static bool setCount(const std::string & key, size_t count);

public:
//- Delete the copy constructor and assignement operator.
    UnitTest_c(const UnitTest_c &) = delete;
    void operator=(const UnitTest_c &) = delete;

    friend std::ostream & operator<<(std::ostream &os, const UnitTest_c &A) { A.display(os); return os; }

    static UnitTest_c & getInstance() { static UnitTest_c instance; return instance; }

    static void setVerbose(bool state = true) { verbose = state; }
    static bool isVerbose(void) { return verbose; }
    static void setProfiling(bool state = true) { profiling = state; }
    static bool isProfiling(void) { return profiling; }
    static void setTolerance(float value) { tolerance = value; }
    static void progress(const std::string & test, const std::string & desc);
    static void complete(void);
    static void checking(const std::string & cond);
    static void failure(const char *file, int line);
    static int getErrorCount(void) { return errors; }
    static int finished(void);
    static int summary(void);

};

#endif // !defined(_UNITTEST_H__20210324_0940__INCLUDED_)

