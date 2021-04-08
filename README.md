# UnitTest_c

A simple unit test 'framework' implemented as a singleton.

## Overview

A unit test framework is useful for regression testing, providing a level of
confidence in code proportional to the test coverage of code.

### Test progress display
The macros VERBOSE_ON and VERBOSE_OFF control whether output is displayed as
the tests are preformed, the state of which can be tested using IS_VERBOSE.

### Defining test cases
A test case is defined between the macro UNIT_TEST(func, desc) and END_TEST.
The parameter 'func' is a function compatible name for the test case and is
used (by RUN_TEST) to call the test case. The parameter 'desc' is a one line
description of the test case. Both 'func' and 'desc' are displayed when
running the tests if IS_VERBOSE is true.

If a test case follows directly on from a previous test case and requires one
or more test cases to have been run, NEXT_CASE(func, desc) can be used as a
continuation of the test. This serves more as a way to split large tests into
smaller chunks. Again 'func' is a function compatible name and 'desc' is a one
line description and are displayed when running the tests. However 'func' 
should not be called.

### Assertions
The macro REQUIRE(cond) is the whole point of this. The parameter 'cond' is
a compilable condition that must be met by the test. Within each unit test
case, code under test should be executed then all appropriate conditions 
should be tested for. If the condition does not return true, an error message
will be displayed and the error count will be incremented.

### Running test cases
To run the test cases use the RUN_TEST(func) macro on all 'func's defined by
the UNIT_TEST macro only. Do not try to run the 'func's defined by the 
NEXT_CASE macros as this will cause a compile error.

### Error count
Once all tests have been run, the error count should be checked. The number of
failed assertions is given by ERROR_COUNT. This should be 0, if not an error 
has been introduced.

### Note
The timings displayed are a rough indication intended to indicate whether
performance has been adversely affected. **Performance tools should be used if 
precise timing is required.**

## Cloning and Running

To clone, build and run this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/UnitTest.git
    cd UnitTest/
    make
    ./test

This should (intentionally) create 1 error, since this code tests that error 
conditions are caught. However, typically, all test should pass. 

The files inittest.cpp and inittest.h can be used along with unit test code.

## Points of interest

This code has the following points of interest:

  * The unit test code is implemented as a singleton.
  * A simple interface via Macros.
  * Light weight code validation.
