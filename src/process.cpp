#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
    float total_time = (float)LinuxParser::ActiveJiffies(pid); // in units jiffies
    float elapsed_time = (float)LinuxParser::UpTime(pid); // in units sec
    return (total_time / (float)sysconf(_SC_CLK_TCK)) / elapsed_time;
    }

// Return the command that generated this process
string Process::Command() {
  if (LinuxParser::Command(pid).size() > 40) {
  	return LinuxParser::Command(pid).substr(0,40) + "...";
  }
  else {
  	return LinuxParser::Command(pid);
  } 
  return "";
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return (this->CpuUtilization() < a.CpuUtilization());
    }