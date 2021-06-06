#include <vector>
#include <string>
#include <sstream>

#include "processor.h"
#include "linux_parser.h"

double Processor::previousActiveJiffies_ = 0.0;
double Processor::previousIdleJiffies_ = 0.0;

double Processor::Utilization() { 
    double currentActiveJiffies = static_cast<double>(LinuxParser::ActiveJiffies());
    double currentIdleJiffies = static_cast<double>(LinuxParser::IdleJiffies());
    double totalCpuUtilization = 0.0f;
    double delActiveJiffies = currentActiveJiffies - this->previousActiveJiffies_;
    double delIdleJiffies = currentIdleJiffies - this->previousIdleJiffies_;
    totalCpuUtilization = delActiveJiffies/(delActiveJiffies + delIdleJiffies);
    this->previousActiveJiffies_ = currentActiveJiffies;
    this->previousIdleJiffies_ = currentIdleJiffies;
    return totalCpuUtilization;  
}