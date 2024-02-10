#include "IO/FileReader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ktx.h>

void Core::IO::GetFilesInFolder(const fs::path & folderPath, std::vector<std::string> & result)
{
    for (const auto & entry : fs::directory_iterator(folderPath))
    {
        //if( entry.path().string().find(".spv") != std::string::npos)
        result.push_back(entry.path().string());
        //std::cout << entry.path() << std::endl;
    }

}

const char * Core::IO::ReadTxtFile(uint32_t& length, const char* filename)
{
    FILE* inFile;
    fopen_s(&inFile, filename, "r");

    if (!inFile)
    {
        assert(0 && "File not found");
    }
    //checkf(inFile, "Trying to load text file: %s, file not found", filepath);

    fseek(inFile, 0, SEEK_END);
    long size = ftell(inFile);
    rewind(inFile);

    char* outString = (char *)calloc(1, size + 1); // Enough memory for file + \0

    fread(outString, size, 1, inFile); // Read in the entire file
    fclose(inFile); // Close the file
    length = size;
    return outString;
}

unsigned char* Core::IO::ReadKtxImage(uint32_t& width, uint32_t& height, uint32_t& textureSize, const char* filename)
{
    ktxResult result;
    ktxTexture* ktxTexture;

    result = ktxTexture_CreateFromNamedFile(filename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);

    assert(result == KTX_SUCCESS);

    // Get properties required for using and upload texture data from the ktx texture object
    width = ktxTexture->baseWidth;
    height = ktxTexture->baseHeight;
    //mipLevels = ktxTexture->numLevels;
    unsigned char* ktxTextureData = ktxTexture_GetData(ktxTexture);
    ktx_size_t components = ktxTexture_GetElementSize(ktxTexture);
    textureSize = ktxTexture->dataSize;
    return ktxTextureData;
}

const char* Core::IO::ReadSpvFile(uint32_t& length, const char* filename)
{
    FILE* fp ;// = fopen(filename, "rb");
    fopen_s(&fp, filename, "rb");
    if (fp == NULL) {
        printf("Could not find or open file: %s\n", filename);
        assert(0 && "wrong path");
    }

    // get file size.
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    long filesizepadded = long(ceil(filesize / 4.0)) * 4;

    // read file contents.
    char *str = new char[filesizepadded];
    fread(str, filesize, sizeof(char), fp);
    fclose(fp);

    // data padding. 
    for (int i = filesize; i < filesizepadded; i++) {
        str[i] = 0;
    }

    length = filesizepadded;
    return str;
}

//loadFile(const char *const name, std::string) 
const char* Core::IO::ReadFile(size_t& length, const char* filename)
{
    fs::path filepath(fs::absolute(fs::path(filename)));

    std::uintmax_t fsize;
    long filesizepadded;
    if (fs::exists(filepath)) {
        fsize = fs::file_size(filepath);
        filesizepadded = long(ceil(fsize / 4.0)) * 4;
        length = (size_t)filesizepadded;
    }
    else {
        throw(std::invalid_argument("File not found: " + filepath.string()));
    }

    

    std::ifstream infile;
    infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        infile.open(filepath.c_str(), std::ios::in | std::ifstream::binary);
    }
    catch (...) {
        std::throw_with_nested(std::runtime_error("Can't open input file " + filepath.string()));
    }

    std::string fileStr;

    try {
        fileStr.resize(filesizepadded);
    }
    catch (...) 
    {
        std::stringstream err;
        err << "Can't resize to " << fsize << " bytes";
        std::throw_with_nested(std::runtime_error(err.str()));
    }

    infile.read(const_cast<char *>(fileStr.c_str()), fsize);
    infile.close();

    // data padding. 
    /*for (int i = fsize; i < filesizepadded; i++) {
        fileStr[i] = 0;
    }*/

    return fileStr.c_str();
}


