#pragma once

template<typename T>
class DeferredRendering
{
private:
    T * apiInterface;

public:
    void Init();
    void SetupRenderer();
    void DislogeRenderer();
    void DeInit();
};
