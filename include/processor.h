#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  

 private:
 static float previousActiveJiffies_;
 static float previousIdleJiffies_;
};

#endif