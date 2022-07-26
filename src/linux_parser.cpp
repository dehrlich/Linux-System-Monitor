#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          filestream.close();
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

// Future Work: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float MemTotal = -1.0;
  float MemFree = -1.0;
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "MemTotal:") {
          MemTotal = stof(value);
        }
        if (key == "MemFree:") {
          MemFree = stof(value);
        }
        if (MemTotal >= 0.0 && MemFree >= 0.0) {
          stream.close();
          return (MemTotal - MemFree) / MemTotal;
        }
    }
  }
  stream.close();
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  stream.close();
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long uptime = UpTime();
  return uptime * sysconf(_SC_CLK_TCK);
  }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> temp;
  string line;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) +kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      temp.emplace_back(value); // collect all tokens from extracted line
                                // from /proc/[pid]/stat to access 14th-17th
                                // tokens which collectively account for active jiffies
    }
    stream.close();
    return stol(temp[13]) + stol(temp[14]) + stol(temp[15]) + stol(temp[16]);
  }
  stream.close();
  return 0;
  }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpuUtilization;
  string value;
  string line;
  long activeJiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // discared the first token from the line
                          // as it is just the word "cpu"
    while (linestream >> value) {
      cpuUtilization.emplace_back(value);
    }
    for (string x : cpuUtilization) {
      activeJiffies += stol(x);
      }
  }
  stream.close();
  return activeJiffies;
  }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilization;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // discared the first token from the line
                          // as it is just the word "cpu"
    while (linestream >> value) {
      cpuUtilization.emplace_back(value);
    }
  }
  stream.close();
  return stol(cpuUtilization[3]) + stol(cpuUtilization[4]);
  }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtilization;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;  // discared the first token from the line
                          // as it is just the word "cpu"
    while (linestream >> value) {
      cpuUtilization.emplace_back(value);
    }
  }
  stream.close();
  return cpuUtilization;
  }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int totalProcesses;
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "processes") {
          totalProcesses = stoi(value);
          stream.close();
          return totalProcesses;
        }
    }
  }
  stream.close();
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningProcesses;
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
        if (key == "procs_running") {
          runningProcesses = stoi(value);
          stream.close();
          return runningProcesses;
        }
    }
  }
  stream.close();
  return 0;
  }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    stream.close();
    return line;
    }
  stream.close();
  return "";
  }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string process_mem;
  string line;
  string key;
  string value;
  int temp;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // return physical RAM being used by process
      if (key == "VmRSS:") {
        temp = stoi(value);
        temp = temp / 1000; // convert from KB to MB
        process_mem = to_string(temp);
        stream.close();
        return process_mem;
      }
    }
  }
  stream.close();
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string uid;
  string line;
  string key;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
        stream.close();
        return uid;
      }
    }
  }
  stream.close();
  return "0";
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string user;
  string line;
  string key;
  string value;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user; // first token in the line is user
      while (linestream >> value) {
        if (value == uid) {
          stream.close();
          return user;
        }
      }
    }
  }
  stream.close();
  return "0";
}
// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  vector<string> temp;
  string line;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) +kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      temp.emplace_back(value); // collect all tokens from extracted line
                                // from /proc/[pid]/stat to access 22nd
                                // value which holds process uptime
    }
    stream.close();
    return UpTime() - (stol(temp[21]) / sysconf(_SC_CLK_TCK)); // convert from clk ticks to seconds
  }
  stream.close();
  return 0;
  }