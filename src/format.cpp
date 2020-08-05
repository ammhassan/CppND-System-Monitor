#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
     int hh, mm, ss;
     hh = seconds / (60 * 60);
     mm = seconds % (60 * 60) / 60;
     ss = seconds % (60 * 60) % 60;
     string hhString, mmString, ssString, formattedTime;
     if (hh < 10) {
          hhString = "0" + to_string(hh);
     } else {
          hhString = to_string(hh);
     }
     if (mm < 10) {
          mmString = "0" + to_string(mm);
     } else {
          mmString = to_string(mm);
     }
     if (ss < 10) {
          ssString = "0" + to_string(ss);
     } else {
          ssString = to_string(ss);
     }
     formattedTime = hhString + ":" + mmString + ":" + ssString;
     return formattedTime; 
    }