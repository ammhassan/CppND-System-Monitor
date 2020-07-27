#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <numeric>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
// not sure if this will enter an infinite loop, check later
float LinuxParser::MemoryUtilization() { 
  int memTotal, memFree, memTotalUsed;
  float memUtilization;
  string line, key, value, unit;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal") {
          memTotal = stoi(value);
        }
        if (key == "MemFree"){
          memFree = stoi(value);
        }
      }
    }
  }
  memTotalUsed = memTotal - memFree;
  memUtilization = memTotalUsed / memTotal;
  return memUtilization; 
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
   string upTime, idleTime, line;
   std::ifstream filestream(kProcDirectory + kUptimeFilename);
   if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
   }
   return stoi(upTime); 
   }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> sysJiffiesString(10);
  string line, key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key>> sysJiffiesString[0] >> sysJiffiesString[1] >> sysJiffiesString[2] >> sysJiffiesString[3] >> sysJiffiesString[4] >> sysJiffiesString[5] >> sysJiffiesString[6] >> sysJiffiesString[7] >> sysJiffiesString[8] >> sysJiffiesString[9];
   }
   
  vector<long> sysJiffies;
  long num;
  for (int i=0; i < 10; i++) {
    if (sysJiffiesString[i] == "0") { 
      num = 0;
    }
    else {
      num = stoi(sysJiffiesString[i]);
    }
    sysJiffies.push_back(num);
  }

  return std::accumulate(sysJiffies.begin(),sysJiffies.end(),0);
  }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> sysJiffiesString(10);
  string line, key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key>> sysJiffiesString[0] >> sysJiffiesString[1] >> sysJiffiesString[2] >> sysJiffiesString[3] >> sysJiffiesString[4] >> sysJiffiesString[5] >> sysJiffiesString[6] >> sysJiffiesString[7] >> sysJiffiesString[8] >> sysJiffiesString[9];
   }
   
  vector<long> sysJiffies;
  long num;
  for (int i=0; i < 10; i++) {
    if (sysJiffiesString[i] == "0") { 
      num = 0;
    }
    else {
      num = stoi(sysJiffiesString[i]);
    }
    sysJiffies.push_back(num);
  }

  long totalJiffies = std::accumulate(sysJiffies.begin(),sysJiffies.end(),0);
  long activeJiffies = totalJiffies - sysJiffies[3] - sysJiffies[4];
  return activeJiffies; 
  }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::ActiveJiffies(); 
  }

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() {
   return static_cast<float>(LinuxParser::ActiveJiffies()) / LinuxParser::Jiffies(); 
   }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }