#pragma once

class TextureFactory
{

private:
    TextureFactory(){}
    TextureFactory(TextureFactory const &) {}
    TextureFactory const & operator= (TextureFactory const &) {}

    static TextureFactory* instance;

public:
    void Init();
    void DeInit();
    void Update();
    static TextureFactory* GetInstance();
    ~TextureFactory();
};
