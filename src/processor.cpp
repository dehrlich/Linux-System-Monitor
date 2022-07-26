#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <vector>

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// Return the aggregate CPU utilization
float Processor::Utilization() {
    vector<string> cpu_jiffies = LinuxParser::CpuUtilization();
    float Idle = stof(cpu_jiffies[3]) + stof(cpu_jiffies[4]);
    float NonIdle = stof(cpu_jiffies[0]) + stof(cpu_jiffies[1]) +
        stof(cpu_jiffies[2]) + stof(cpu_jiffies[5]) +
        stof(cpu_jiffies[6]) + stof(cpu_jiffies[7]);
    float Total = Idle + NonIdle;
    
    // differentiate: actual value minus previous one
    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle;

    //update private members of Processor class
    PrevTotal = Total;
    PrevIdle = Idle;

    //return cpu utilization as a percentage
    return (totald - idled) / totald;
    }