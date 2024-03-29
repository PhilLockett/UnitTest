# UnitTest_c
A simple, lightweight unit test 'framework' implemented as a singleton.

## Overview
A unit test framework is useful for regression testing, providing a level of
confidence in code proportional to the test coverage of code. This lightweight
implementation is intended for easy use with test code for quick confirmation
that bugs have not been introduced.

### Generated files
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

### Test progress display
The macros VERBOSE_ON and VERBOSE_OFF control whether output is displayed by
UnitTest as the tests are performed. The verbose state can be checked using
the macro IS_VERBOSE, which returns a boolean. Calling VERBOSE_OFF makes the
tests run quietly, only displaying errors and timing warnings. Verbose is on
by default.

### Disabling profiling
By default, profiling is enabled. However, for test cases that make many
assertions, "profile.txt" can become very large. The macros PROFILE_OFF and
PROFILE_ON can be used to control this.

### Defining test cases
A test case is defined between the macro UNIT_TEST(func, desc) and the macro
END_TEST. The parameter `func` is a function compatible name for the test case
and is used (by RUN_TEST) to call the test case. The parameter `desc` is a one
line description of the test case. Both `func` and `desc` are displayed when
running the tests if IS_VERBOSE is true.

If a test case is directly dependent on a previous test case and requires one
or more test cases to have been run, NEXT_CASE(func, desc) can be used as a
continuation of the test. This serves more as a way to split large tests into
smaller chunks. Again `func` is a function compatible name and `desc` is a one
line description and are displayed when running the tests. However, `func` 
should not be called by the RUN_TEST macro or directly.

### Assertions
The macro REQUIRE(cond) is the whole point of this. The parameter `cond` is
a compilable condition that must be met by the test. Within each unit test
case, code under test should be executed then all appropriate conditions 
should be tested for. If the condition does not return true, an error message
will be displayed and the error count will be incremented.

### Running test cases
To run the test cases use the RUN_TEST(func) macro on all `func`s defined by
the UNIT_TEST macro only. Do not try to run the `func`s defined by the 
NEXT_CASE macros as this will cause a compilation error.

### Error count
The current error count can be obtained at any time with the ERROR_COUNT macro.

### Test completion
Once all tests have been run, the FINISHED macro must be called. FINISHED also
returns the error count, which is the number of failed assertions. The error
count should be 0, if not, an error has been introduced.

Calling OUTPUT_SUMMARY displays any progress by comparing the previous and 
latest results and indicating whether any test cases now have better or worse
results as well as displaying the error count.

### Note on timings
The timings displayed are a rough indication to whether performance has been
adversely affected by a code change. The code uses the file "timings.txt" to
track previous test case timings. If the duration of a test case has increased
by more than 25% (default) a message is displayed, but it is not counted as an
error. Deleting "timings.txt" will cause this file to be regenerated when the
tests are next run.

The default 25% tolerance can be adjusted by calling the SET_TOLERANCE(value)
macro. The `value` parameter is specified as a decimal, so for example, an
acceptable deviation of 25% would be set by calling SET_TOLERANCE(0.25).

Time checking can be disabled with the macro TIMINGS_OFF or by setting the
tolerance to 0.0. Time checking can be enabled with the macro TIMINGS_ON or by
setting the tolerance to a positive non zero value.

Also note that any change to the test names, i.e. the value of `func` used in
the call to UNIT_TEST, "timings.txt" will need to be deleted so that the
timings for the new `func` names can be calculated.

**If precise timing is required, performance tools should be used.**

## Cloning and Running
To clone, build and run this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/UnitTest.git
    cd UnitTest/
    make
    ./test

This test code (test.cpp) should (intentionally) create 1 error, since this
code tests that error conditions are caught. However, typically, all test
should pass and ERROR_COUNT should be 0.

The files unittest.cpp and unittest.h can be used along with unit test code.

## Points of interest
This code has the following points of interest:

  * The unit test code is implemented as a singleton.
  * A simple interface via Macros.
  * Light weight code validation.
