#include <string>
#include <ios>
#include <iomanip>
#include <sstream>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    std::ostringstream out;
    int hrs = (seconds / (60 * 60));
    seconds -= hrs * (60 * 60);
    int min = (seconds / 60);
    seconds -= min * 60; 

    out << std::internal << std::setfill('0') << std::setw(2) <<
    hrs << ':' << std::setw(2) << min << ':' << std::setw(2) << seconds;

    return out.str();
}