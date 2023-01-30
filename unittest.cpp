/**
 * @file    unittest.cpp
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
 * Simple unit test "framework" Implementation.
 */

#include <iostream>
#include <fstream>

#include "unittest.h"

const std::string timingsFileName{"timings.txt"};
const std::string profileFileName{"profile.txt"};
const std::string resultsFileName{"results.txt"};
const std::string logTestText{"logTest"};

std::string UnitTest_c::testCase{"UNDEFINED"};
std::string UnitTest_c::description{"UNDEFINED"};
std::string UnitTest_c::condition{"UNDEFINED"};
bool UnitTest_c::update{};
bool UnitTest_c::verbose{true};
bool UnitTest_c::profiling{true};
size_t UnitTest_c::errors{};
float UnitTest_c::tolerance{0.25f};

std::chrono::time_point<std::chrono::steady_clock> UnitTest_c::start{};

std::unordered_map<std::string, std::chrono::nanoseconds> UnitTest_c::times{};
std::unordered_map<std::string, size_t> UnitTest_c::counts{};
std::unordered_map<std::string, size_t> UnitTest_c::errorList{};
std::vector<std::pair<std::string, std::string>> UnitTest_c::assertList{};


/**
 * Send the current name-value pairs to the output stream.
 *
 * @param  os - Output stream.
 */
void UnitTest_c::display(std::ostream &os) const
{
    os << "\tTest Case:\t" << testCase << "()\n";
    os << "\tDescription:\t" << description << "\n";
    os << "\tCondition:\t" << condition << "\n";
    os << "\tVerbose:\t" << std::boolalpha << verbose << '\n';
    os << "\tCurrent Errors:\t" << errors << '\n';
    os << "\tTolerance:\t" << (int)(tolerance * 100)<< '\n';
}

void UnitTest_c::progress(const std::string & test, const std::string & desc)
{
    testCase = test;
    description = desc;

    errorList[testCase] = 0;
    assertList.emplace_back(testCase, logTestText);

    if (verbose)
        std::cout << testCase << " - " << description << '\n';

    start = std::chrono::steady_clock::now();
}

bool UnitTest_c::store(void)
{
    if (std::ofstream os{timingsFileName, std::ios::out})
    {
        std::cout << "Generating test timings in text file " << timingsFileName << "\n";

        // Output in assertList order.
        for (auto & [testCase, condition] : assertList)
            if (condition.compare(logTestText) == 0)
                os << times[testCase].count() << ' ' << testCase << '\n';
    }

    return true;
}

bool UnitTest_c::retrieve(void)
{
    std::ifstream infile(timingsFileName, std::ifstream::in);
    if (!infile.is_open())
        return false;

    int64_t time;
    std::string func;

    while (infile >> time >> func)
        if (!infile.eof() && func.length())
            setTime(func, std::chrono::nanoseconds{time});

    infile.close();

    infile.open(resultsFileName, std::ifstream::in);
    if (!infile.is_open())
        return false;

    size_t count;

    while (infile >> count >> func)
        if (!infile.eof() && func.length())
            setCount(func, count);

    infile.close();
    return true;
}
std::chrono::nanoseconds UnitTest_c::getTime(const std::string & key)
{
    auto it = times.find(key);
    if (it == times.end())
        return std::chrono::nanoseconds{0};

    return it->second;
}
bool UnitTest_c::setTime(const std::string & key, std::chrono::nanoseconds value)
{
    auto it = times.find(key);
    if (it == times.end())
    {
        times[key] = value;
        return true;
    }

//    it->second = value;
    return false;
}
bool UnitTest_c::setCount(const std::string & key, size_t count)
{
    auto it = counts.find(key);
    if (it == counts.end())
    {
        counts[key] = count;
        return true;
    }

//    it->second = value;
    return false;
}
void UnitTest_c::complete(void)
{
    if (tolerance <= 0.0f)
        return;

    const auto stop = std::chrono::steady_clock::now();
    const auto elapsed{stop-start};
    const auto nseconds = elapsed.count();
    if (setTime(testCase, elapsed))
    {
        update = true;
        if (verbose)
            std::cout << testCase << " -> " << nseconds << "ns\n";
    }
    else
    {
        const auto previous = getTime(testCase);
        const auto delta{elapsed - previous};
        const auto change = (float)(delta.count()) / previous.count();
        const auto slower = (delta > std::chrono::nanoseconds{0});
        if ((slower) && (change > tolerance))
        {
            std::cerr << '\n';
            std::cerr << "After running test case \"" << testCase << "\" - \"" << description << "\"\n";
            std::cerr << "\tTest was too slow (previous: " << previous.count() << "ns, current: " << nseconds << "ns)\n";
            std::cerr << '\n';
        }
        if (verbose)
        {
            const auto percent = (int)(change * 100);
            if (slower)
                std::cout << testCase << " -> " << nseconds << "ns (" << percent << "% slower than previous)\n";
            else
                std::cout << testCase << " -> " << nseconds << "ns (" << -percent << "% faster than previous)\n";
        }
    }
}
void UnitTest_c::checking(const std::string & cond)
{
    condition = cond;
    if (profiling)
        assertList.push_back({testCase, condition});
}
void UnitTest_c::failure(const char *file, int line)
{
    errors++;
    errorList[testCase]++;

    std::cerr << '\n';
    std::cerr << "While running test case \"" << testCase << "\" - \"" << description << "\"\n";
    std::cerr << "\t(in file: " << file << ", on line: " << line << ")\n";
    std::cerr << "\tRequirement (" << condition << ") failed\n";
    std::cerr << '\n';
}

int UnitTest_c::finished(void)
{
    if (update)
        store();

    if (std::ofstream os{profileFileName, std::ios::out})
    {
        std::cout << "Generating test profile in text file " << profileFileName << "\n";

        for (auto & [testCase, condition] : assertList)
        {
            if (condition.compare(logTestText) == 0)
                os << testCase << '\n';
            else
                os << "  " << condition << '\n';
        }
    }

    if (std::ofstream os{resultsFileName, std::ios::out})
    {
        std::cout << "Generating test results in text file " << resultsFileName << "\n";

        // Output in assertList order.
        for (auto & [testCase, condition] : assertList)
        {
            if (condition.compare(logTestText) == 0)
                os << errorList[testCase] << " " << testCase << "\n";
        }
    }

    return errors;
}

int UnitTest_c::summary(void)
{
    std::cout << "\nTest Result Summary\n";

    bool better{};
    bool worse{};
    for (auto & [testCase, count] : errorList)
    {
        if (count > counts[testCase])
            worse = true;
        else
        if (count < counts[testCase])
            better = true;
    }

    if (worse)
    {
        std::cerr << "\nThe following test cases are worse than in the previous test run:\n";

        for (auto & [testCase, condition] : assertList)
            if (condition.compare(logTestText) == 0)
                if (errorList[testCase] > counts[testCase])
                    std::cerr << "  " << testCase << "\n";
    }

    if (better)
    {
        std::cout << "\nThe following test cases are better than in the previous test run:\n";

        for (auto & [testCase, condition] : assertList)
            if (condition.compare(logTestText) == 0)
                if (errorList[testCase] < counts[testCase])
                    std::cout << " " << testCase << "\n";
    }

    if (errors)
        std::cerr << "\n" << errors << " ERROR(S) encountered!.\n";
    else
        std::cout << "\nAll tests passed.\n";

    return errors;
}
