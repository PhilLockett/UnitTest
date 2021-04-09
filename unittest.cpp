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
std::chrono::duration<float> UnitTest_c::elapsed_seconds;

std::unordered_map<std::string, float> UnitTest_c::times;

/**
 * Send the current name-value pairs to the output stream.
 *
 * @param  os - Output stream.
 */
void UnitTest_c::display(std::ostream &os) const
{
     os << "\tTest Case:\t" << testCase << "()\n";
     os << "\tDescription:\t" << description << "()\n";
     os << "\tVerbose:\t" << verbose << '\n';
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
        outfile << it.second << ' ' << it.first << '\n';
    }
    outfile.close();

    return true;
}

bool UnitTest_c::retrieve(void)
{
    std::ifstream infile(fileName, std::ifstream::in);
    if (!infile.is_open())
        return false;

    float time;
    std::string func;

    while (infile >> time >> func)
    {
        if (!infile.eof() && func.length())
            setTime(func, time);
    }

    infile.close();

    return true;
}
float UnitTest_c::getTime(const std::string & key)
{
    auto it = times.find(key);
    if (it == times.end())
    {
        return 0.0f;
    }

    return it->second;
}
bool UnitTest_c::setTime(const std::string & key, float value)
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
    elapsed_seconds = std::chrono::steady_clock::now() - start;
    float seconds = elapsed_seconds.count();
    if (setTime(testCase, seconds))
    {
        float prev = getTime(testCase);
        float delta = seconds - prev;
        float change = delta / prev;
        if ((delta > 0) && (change > tolerance))
        {
            std::cerr << '\n';
            std::cerr << "After running " << testCase << " (" << description << "):\n";
            std::cerr << "\ttest was too slow (previous: " << prev << "s, current: " << seconds << "s)\n";
            std::cerr << '\n';
        }
        if (verbose)
        {
            if (delta > 0)
                std::cout << testCase << " -> " << seconds << "s (slower than previous: " << prev << "s)" << std::endl;
            else
                std::cout << testCase << " -> " << seconds << "s (faster than previous: " << prev << "s)" << std::endl;
        }
    }
    else
    if (verbose)
    {
        std::cout << testCase << " -> " << seconds << 's' << std::endl;
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

