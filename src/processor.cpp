#include <vector>
#include <string>
#include <sstream>

#include "processor.h"
#include "linux_parser.h"

float Processor::previousActiveJiffies_ = 0.0;
float Processor::previousIdleJiffies_ = 0.0;

float Processor::Utilization() { 
    float currentActiveJiffies = (float)LinuxParser::ActiveJiffies();
    float currentIdleJiffies = (float)LinuxParser::IdleJiffies();
    float ftotalCpuUtilization = 0.0f;
    float delActiveJiffies = currentActiveJiffies - this->previousActiveJiffies_;
    float delIdleJiffies = currentIdleJiffies - this->previousIdleJiffies_;
    ftotalCpuUtilization = delActiveJiffies/(delActiveJiffies + delIdleJiffies);
    this->previousActiveJiffies_ = currentActiveJiffies;
    this->previousIdleJiffies_ = currentIdleJiffies;
    return ftotalCpuUtilization;  
}