#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long times) { 
    string elapsedTime{};
    long seconds = times%60;
    long minutes = times/60;
    long hours = minutes/60;
    minutes = minutes%60;
    elapsedTime = formatTime(hours) + ":" + formatTime(minutes) + ":" + formatTime(seconds);
    return elapsedTime; 
}

std::string Format::formatTime(long time){
    std::string result{};
    if (time < 10){
        result = "0";
    }
    result+= std::to_string(time);
    return result;
}