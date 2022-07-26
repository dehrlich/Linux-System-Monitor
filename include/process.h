#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization() const;                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  // Future Work: use sort function
  										   // and overloaded operator to sort
  										   // processes by Cpu Utilization

  //constructor with initializer list
  Process(int p) : pid(p) {}

 private:
 int pid;
};

#endif