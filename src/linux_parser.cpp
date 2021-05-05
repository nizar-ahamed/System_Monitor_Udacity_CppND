#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
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
          return value;
        }
      }
    }
  }
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
  return kernel;
}

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

float LinuxParser::MemoryUtilization() { 
  float memTotal, memFree;
  string line;
  string description, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> description >> value;
      if (description == "MemTotal:") { memTotal = std::stof(value);}
      else if (description == "MemFree:") { memFree = std::stof(value);}
    }
  }
  return ((memTotal - memFree)/memTotal); 
}

long LinuxParser::UpTime() { 
  long upTime = 0;
  string line, value;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    upTime = std::stol(value);
  }
  return upTime; 
}

long LinuxParser::Jiffies() { 
  return (LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies());
}

long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long activeJiffies = 0;
  if (filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    string temp, utime, stime, cutime, cstime;
    for(int i=1; i<14; i++){linestream >> temp;}
    linestream >> utime >> stime >> cutime >> cstime;
    activeJiffies = std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime);
  }
  return activeJiffies; 
}

long LinuxParser::ActiveJiffies() { 
  string line;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  long activeJiffies = 0;
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string description, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    linestream >> description >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    activeJiffies = std::stol(user) + std::stol(nice) + std::stol(system) + std::stol(irq) + std::stol(softirq) + std::stol(steal);
   }
  return activeJiffies; 
}

long LinuxParser::IdleJiffies() { 
  string line;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  long idleJiffies = 0;
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string description, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    linestream >> description >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    idleJiffies = std::stol(idle) + std::stol(iowait);
  }
  return idleJiffies; 
}

vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuUtilization;
  string line;
  string description;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> description;
      if(description == "intr"){break;}
      else {
        cpuUtilization.push_back(line);
      }
    }
  }
  return cpuUtilization; 
}

int LinuxParser::TotalProcesses() { 
   int totalProcesses = 0;
   string description, value;
   string line;
   std::ifstream filestream(kProcDirectory + kStatFilename);
   if (filestream.is_open()) {
     while (std::getline(filestream, line)){
       std::istringstream linestream(line);
       linestream >> description;
       if (description == "processes")
       {
         linestream >> value;
         totalProcesses = std::stoi(value);
       }
     }
   }
   return totalProcesses;
}

int LinuxParser::RunningProcesses() { 
  int runningProcesses = 0;
  string description, value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> description;
      if (description == "procs_running"){
        linestream >> value;
        runningProcesses = std::stoi(value);
      }
    }
  }
  return runningProcesses; 
}

string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
  }
  return line; 
}

string LinuxParser::Ram(int pid) { 
  string description, ram, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> description;
      if (description == "VmSize:"){
        linestream >> ram;
        break;
      }
    }
  }
  return ram; 

}

string LinuxParser::Uid(int pid) { 
  string description, uid, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> description;
      if (description == "Uid:"){
        linestream >> uid;
        break;
      }
    }
  }
  return uid; 
}

string LinuxParser::User(int pid) { 
  string line;
  string user, pwd;
  string value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> pwd >> value;
      if (value == LinuxParser::Uid(pid)) {
        std::replace(user.begin(), user.end(), '_', ' ');
        return user;
      }
    }
  }
  return user; 
}

long LinuxParser::UpTime(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long upTime = 0;
  if (filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    string temp, uptime;
    for(int i=1; i<22; i++){linestream >> temp;}
    linestream >> uptime;
    upTime = std::stol(uptime);
  }
  upTime/=sysconf(_SC_CLK_TCK);
  return upTime; 
}
