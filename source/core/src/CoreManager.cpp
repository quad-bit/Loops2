#include "CoreManager.h"
#include "Settings.h"

#include "CorePrecompiled.h"
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "Utility/HashManager.h"

Core::CoreManager* Core::CoreManager::instance = nullptr;

void Core::CoreManager::Init()
{
    //plog::init(plog::debug, "Hello.txt"); // Step2: initialize the logger.
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);
    PLOGD << "Core manager Init";
    
    HashManager::GetInstance()->Init();
    /*
    // Step3: write log messages using a special macro. There are several log macros, use the macro you liked the most.
    PLOGV << "asdfasdfas";
    PLOGD << "Hello log!"; // short macro
    PLOG_DEBUG << "Hello log!"; // long macro
    PLOG(plog::debug) << "Hello log!"; // function-style macro

    // Log severity levels are printed in different colors.
    PLOG_VERBOSE << "This is a VERBOSE message";
    PLOG_DEBUG << "This is a DEBUG message";
    PLOG_INFO << "This is an INFO message";
    PLOG_WARNING << "This is a WARNING message";
    PLOG_ERROR << "This is an ERROR message";
    PLOG_FATAL << "This is a FATAL message";
    */
}

void Core::CoreManager::DeInit()
{
    PLOGD << "Core manager DeInit";

    HashManager::GetInstance()->DeInit();
    delete HashManager::GetInstance();
}

void Core::CoreManager::Update()
{

}

Core::CoreManager * Core::CoreManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new Core::CoreManager();
    }
    return instance;
}

Core::CoreManager::~CoreManager()
{
}
