#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
#include "system.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() {
     return pid_; 
}

void Process::setPid(int pid) {
    pid_ = pid;
}

float Process::CpuUtilization() {
    return LinuxParser::PidCpuUtilization(Process::Pid()); 
}

string Process::Command() { 
    return LinuxParser::Command(Process::Pid()); 
}

string Process::Ram() { 
    return LinuxParser::Ram(Process::Pid());
}

string Process::User() { 
    return LinuxParser::User(Process::Pid()); 
}

long int Process::UpTime() { 
    return LinuxParser::UpTime(Process::Pid());
}

bool Process::operator<(Process & a) { 
    return a.CpuUtilization() < this->CpuUtilization();
}