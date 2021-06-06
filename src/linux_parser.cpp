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

template <typename T>
T findValueByKey(std::string const &keyFilter, string const &filename) {
  string line;
  string key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
    stream.close();
  }
  return value;
};

template <typename T>
T getValueOfFile(string const &filename) {
  string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};


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
    filestream.close();
  }
  return value;
}

string LinuxParser::Kernel() {
  string os;
  string kernel;
  string version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
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
  float memTotal = findValueByKey<float>("MemTotal:", kMeminfoFilename);// "/proc/memInfo";
  float memFree = findValueByKey<float>("MemFree:", kMeminfoFilename);;
  return ((memTotal - memFree)/memTotal); 
}


long LinuxParser::UpTime() { 
  long upTime = getValueOfFile<long>(kUptimeFilename);
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
    string temp;
    string utime;
    string stime;
    string cutime;
    string cstime;
    for(int i=1; i<14; i++){linestream >> temp;}
    linestream >> utime >> stime >> cutime >> cstime;
    activeJiffies = std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime);
    filestream.close();
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
    std::string description;
    linestream >> description;
    std::string stats[10];
    for (int i = 0; i<10; i++) {linestream >> stats[i];}
    activeJiffies = std::stol(stats[kUser_]) + std::stol(stats[kNice_]) + std::stol(stats[kSystem_]) + std::stol(stats[kIRQ_]) + std::stol(stats[kSoftIRQ_]) + std::stol(stats[kSteal_]);
    filestream.close();
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
    std::string description;
    std::string stats[10];
    for (int i = 0; i<10; i++) {linestream >> stats[i];}
    idleJiffies = std::stol(stats[kIdle_]) + std::stol(stats[kIOwait_]);
    filestream.close();
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
        cpuUtilization.emplace_back(line);
      }
    }
    filestream.close();
  }
  return cpuUtilization; 
}

int LinuxParser::TotalProcesses() { 
   int totalProcesses = findValueByKey<int>("processes", kStatFilename);
   return totalProcesses;
}

int LinuxParser::RunningProcesses() { 
  int runningProcesses = findValueByKey<int>("procs_running", kStatFilename);
  return runningProcesses; 
}

string LinuxParser::Command(int pid) { 
  string line = getValueOfFile<string>(std::to_string(pid) + kCmdlineFilename);
  return line; 
}

string LinuxParser::Ram(int pid) { 
  string ram = findValueByKey<string>(filterProcMem, std::to_string(pid) + kStatusFilename);
  int iRam = std::stoi(ram)/1024;
  ram = std::to_string(iRam);
  return ram; 
}

string LinuxParser::Uid(int pid) { 
  string uid = findValueByKey<string>("Uid:", std::to_string(pid) + kStatusFilename);
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
    filestream.close();
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
    filestream.close();
  }
  upTime/=sysconf(_SC_CLK_TCK);
  return upTime; 
}
