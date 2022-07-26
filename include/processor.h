#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  Processor() {
    PrevTotal = 0.0;
    PrevIdle = 0.0;
  }

 private:

 float PrevTotal; //prev total cpu utilization
 float PrevIdle; // prev idle cpu utilization
};

#endif