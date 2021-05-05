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

Process::Process(int pid):pid_(pid){}

int Process::Pid() { return this->pid_; }

float Process::CpuUtilization() { 
    float curActiveJiffies = (float)LinuxParser::ActiveJiffies(this->pid_);
    float upTime = (float)LinuxParser::UpTime();
    float cpuUtilization = curActiveJiffies/(float)sysconf(_SC_CLK_TCK)/upTime;  
    return cpuUtilization;
}

string Process::Command() { return LinuxParser::Command(this->pid_); }

string Process::Ram() { return LinuxParser::Ram(this->pid_); }

string Process::User() { return LinuxParser::User(this->pid_); }

long int Process::UpTime() { return (LinuxParser::UpTime() - LinuxParser::UpTime(this->pid_)); }

bool Process::operator<(Process& a) { return (this->CpuUtilization() > a.CpuUtilization());}