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
  memUtilization = static_cast<float>(memTotalUsed) / memTotal;
  return memUtilization; 
  }

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

float LinuxParser::PidCpuUtilization(int pid) {
  string line, placeHolder;
  long utime, stime, starttime, totalTime, elapsedTime_sec;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + "stat");
  if (filestream.is_open()) {
  std::getline(filestream, line);
  std::istringstream linestream(line);
  for (int i = 0; i < 22; i++) {
    if (i == 13) {
      linestream >> utime;
    }
    else if (i == 14)
    {
      linestream >> stime;
    }
    else if (i == 21)
    {
      linestream >> starttime;
    }
    else 
    {
      linestream >> placeHolder;
    }
  }
  }
  totalTime = utime + stime;
  elapsedTime_sec = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
  float cpuUsage;
  if (elapsedTime_sec == 0) {
    cpuUsage = 0.0;
  }
  else {
    cpuUsage = static_cast<float>(totalTime / sysconf(_SC_CLK_TCK)) / elapsedTime_sec;
  }
  return cpuUsage;
}

long LinuxParser::ActiveJiffies() { 
  vector<string> sysJiffiesString(10);
  string line, key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key>> sysJiffiesString[0] >> sysJiffiesString[1] >>
                        sysJiffiesString[2] >> sysJiffiesString[3] >> 
                        sysJiffiesString[4] >> sysJiffiesString[5] >>
                        sysJiffiesString[6] >> sysJiffiesString[7] >>
                        sysJiffiesString[8] >> sysJiffiesString[9];
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

long LinuxParser::IdleJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::ActiveJiffies(); 
  }

float LinuxParser::CpuUtilization() {
   return static_cast<float>(LinuxParser::ActiveJiffies()) / LinuxParser::Jiffies(); 
   }

int LinuxParser::TotalProcesses() {
  int totalProcesses;
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          totalProcesses = stoi(value);
        }
      }
    }
  }
  return totalProcesses; 
  }

int LinuxParser::RunningProcesses() {
  int runningProcesses;
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          runningProcesses = stoi(value);
        }
      }
    }
  }
  return runningProcesses;  
  }

string LinuxParser::Command(int pid) {
  string line;
   std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + "cmdline");
   if (filestream.is_open()) {
    std::getline(filestream, line);
   }
   return line; 
  }

string LinuxParser::Ram(int pid) {
  string line, key, value, unit;
  long ram_Mb;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + "status");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "VmSize:") {
          ram_Mb = stoi(value) / 1000;
        }
      }
    }
  }
  return to_string(ram_Mb);
  }

string LinuxParser::Uid(int pid) {
  string line, key, value1, value2, value3, value4;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + "status");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value1 >> value2 >> value3 >> value4) {
        if (key == "Uid:") {
          return value1;
        }
      }
    }
  }
   return value1; 
  }

string LinuxParser::User(int pid) {
  string userID = LinuxParser::Uid(pid);
  string line, key, value1, value2, value3;
  std::ifstream filestream("/etc/passwd");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value1 >> value2 >> value3) {
        if (value2 == userID) {
          return key;
        }
      }
    }
  } 
  return key;
}

long LinuxParser::UpTime(int pid) {
  string line, placeHolder;
  long uptime, starttime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" + "stat");
  if (filestream.is_open()) {
  std::getline(filestream, line);
  std::istringstream linestream(line);
  for (int i = 0; i < 22; i++) {
    if (i == 21) {
      linestream >> starttime;
    }
    else
    {
      linestream >> placeHolder;
    }
  }
  }
  uptime = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
  return uptime; 
}