#pragma once
#include <chrono>
#include "Platform/Assertion.h"

namespace Core
{
    namespace Utility
    {
        class Timer
        {
        private:
            Timer() {}
            Timer(Timer const&) {}
            Timer const& operator= (Timer const&) {}

            static Timer* instance;
            std::chrono::high_resolution_clock::time_point currentTimeStamp, previousTimeStamp;

        public:
            void Init();
            void DeInit();
            void Update();
            static Timer* GetInstance();
            ~Timer();

            void Tick();
            void Reset();
            template <typename T>
            T GetDeltaTime();

            uint32_t GetSeconds();
        };
    }
}

template<typename T>
inline T Core::Utility::Timer::GetDeltaTime()
{
    ASSERT_MSG_DEBUG(std::chrono::high_resolution_clock::is_steady, "Not a steady clock");
    //Tick();
    currentTimeStamp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<T, std::milli> fs = currentTimeStamp - previousTimeStamp;
    //auto fs = std::chrono::duration_cast<T, std::chrono::milliseconds>(currentTimeStamp - previousTimeStamp);
    previousTimeStamp = currentTimeStamp;
        
    return fs.count();
}


/*
class Timer {
public:
    Timer() {
        reset();
    }
    void reset() {
        m_timestamp = std::chrono::high_resolution_clock::now();
    }
    float diff() {
        std::chrono::duration<float> fs = std::chrono::high_resolution_clock::now() - m_timestamp;
        return fs.count();
    }
private:
    std::chrono::high_resolution_clock::time_point m_timestamp;
};
*/

