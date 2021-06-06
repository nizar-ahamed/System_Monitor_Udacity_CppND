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

// Return this process's ID
int Process::Pid() { return this->pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    float curActiveJiffies = (float)LinuxParser::ActiveJiffies(this->pid_);
    float upTime = (float)LinuxParser::UpTime();
    float cpuUtilization = curActiveJiffies/(float)sysconf(_SC_CLK_TCK)/upTime;  
    return cpuUtilization;
}

// Return the command that generated this process
string Process::Command() { 
    string command = LinuxParser::Command(this->pid_);
    if (command.length() > 40)
    {
        command = command.substr(0,40) + "...";
    }
    return  command;
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(this->pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return (LinuxParser::UpTime() - LinuxParser::UpTime(this->pid_)); }

// Overload the "less than" comparison operator for Process objects.
bool Process::operator<(Process& a) { return (this->CpuUtilization() < a.CpuUtilization());}