#include "Utility/Timer.h"
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <ctime>   // localtime
#include <plog/Log.h> 

Core::Utility::Timer* Core::Utility::Timer::instance = nullptr;

void Core::Utility::Timer::Init()
{
    currentTimeStamp = std::chrono::high_resolution_clock::now();
    previousTimeStamp = std::chrono::high_resolution_clock::now();
}

void Core::Utility::Timer::DeInit()
{

}

void Core::Utility::Timer::Update()
{

}

Core::Utility::Timer * Core::Utility::Timer::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Core::Utility::Timer();
    }
    return instance;
}

Core::Utility::Timer::~Timer()
{
}

void Core::Utility::Timer::Tick()
{
    currentTimeStamp = std::chrono::high_resolution_clock::now();
}

void Core::Utility::Timer::Reset()
{
    currentTimeStamp = std::chrono::high_resolution_clock::now();
    previousTimeStamp = std::chrono::high_resolution_clock::now();
}

uint32_t Core::Utility::Timer::GetSeconds()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    auto duration = now.time_since_epoch();

    typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>
    >::type> Days; /* UTC: +8:00 */

    Days days = std::chrono::duration_cast<Days>(duration);
    duration -= days;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    duration -= milliseconds;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    duration -= microseconds;
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    std::stringstream ss;

    return (uint32_t)seconds.count();
}

