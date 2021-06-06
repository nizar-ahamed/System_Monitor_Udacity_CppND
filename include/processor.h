#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  double Utilization();  

 private:
 static double previousActiveJiffies_;
 static double previousIdleJiffies_;
};

#endif