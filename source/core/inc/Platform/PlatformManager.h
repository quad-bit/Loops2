#pragma once

class PlatformManager
{
private:

    PlatformManager();
    PlatformManager(PlatformManager const &) {}
    PlatformManager const & operator= (PlatformManager const &) {}

    static PlatformManager* platformManagerInstance;

public:

    void Init();
    void DeInit();
        
    static PlatformManager* GetSingleton();
    ~PlatformManager();

};
