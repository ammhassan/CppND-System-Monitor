#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
     int hh, mm, ss;
     hh = seconds / (60 * 60);
     mm = seconds % (60 * 60) / 60;
     ss = seconds % (60 * 60) % 60;
     string formattedTime;
     formattedTime = std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss);
     return formattedTime; 
    }