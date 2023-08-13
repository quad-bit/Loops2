#include "TextureFactory.h"

TextureFactory* TextureFactory::instance = nullptr;

void TextureFactory::Init()
{

}

void TextureFactory::DeInit()
{

}

void TextureFactory::Update()
{

}

TextureFactory * TextureFactory::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new TextureFactory();
    }
    return instance;
}

TextureFactory::~TextureFactory()
{
}
