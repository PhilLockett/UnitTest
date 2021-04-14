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

std::string UnitTest_c::fileName = "timings.txt";
std::string UnitTest_c::testCase = "UNDEFINED";
std::string UnitTest_c::description = "UNDEFINED";
std::string UnitTest_c::condition = "UNDEFINED";
bool UnitTest_c::verbose = true;
int UnitTest_c::errors = 0;
float UnitTest_c::tolerance = 0.25f;

std::chrono::time_point<std::chrono::steady_clock> UnitTest_c::start;

std::unordered_map<std::string, std::chrono::nanoseconds> UnitTest_c::times;

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

    if (verbose)
    {
        std::cout << testCase << " - " << description << std::endl;
    }
    start = std::chrono::steady_clock::now();
}

bool UnitTest_c::store(void)
{
    std::ofstream outfile(fileName, std::ifstream::out);
    if (!outfile.is_open())
        return false;

    for (auto & it : times)
    {
        outfile << it.second.count() << ' ' << it.first << '\n';
    }
    outfile.close();

    return true;
}

bool UnitTest_c::retrieve(void)
{
    std::ifstream infile(fileName, std::ifstream::in);
    if (!infile.is_open())
        return false;

    int64_t time;
    std::string func;

    while (infile >> time >> func)
    {
        if (!infile.eof() && func.length())
            setTime(func, std::chrono::nanoseconds{time});
    }

    infile.close();

    return true;
}
std::chrono::nanoseconds UnitTest_c::getTime(const std::string & key)
{
    auto it = times.find(key);
    if (it == times.end())
    {
        return std::chrono::nanoseconds{0};
    }

    return it->second;
}
bool UnitTest_c::setTime(const std::string & key, std::chrono::nanoseconds value)
{
    auto it = times.find(key);
    if (it == times.end())
    {
        times[key] = value;
        return false;
    }

//    it->second = value;
    return true;
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
        const auto prev = getTime(testCase);
        const auto delta{elapsed - prev};
        const auto change = (float)(delta.count()) / prev.count();
        const auto slower = (delta > std::chrono::nanoseconds{0});
        if ((slower) && (change > tolerance))
        {
            std::cerr << '\n';
            std::cerr << "After running " << testCase << " (" << description << "):\n";
            std::cerr << "\ttest was too slow (previous: " << prev.count() << "ns, current: " << nseconds << "ns)\n";
            std::cerr << '\n';
        }
        if (verbose)
        {
            const auto percent = (int)(change * 100);
            if (slower)
                std::cout << testCase << " -> " << nseconds << "ns (" << percent << "% slower than previous)" << std::endl;
            else
                std::cout << testCase << " -> " << nseconds << "ns (" << -percent << "% faster than previous)" << std::endl;
        }
    }
    else
    if (verbose)
    {
        std::cout << testCase << " -> " << nseconds << "ns" << std::endl;
    }
}
void UnitTest_c::failure(const std::string & cond)
{
    errors++;
    condition = cond;
    if (verbose)
        std::cerr << '\n';

    std::cerr << '\n';
    std::cerr << "While running " << testCase << " (" << description << "):\n";
    std::cerr << "\tRequirement (" << condition << ") failed\n";
    std::cerr << '\n';
}

