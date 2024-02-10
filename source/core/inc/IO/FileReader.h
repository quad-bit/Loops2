#pragma once

#include <vector>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

namespace Core
{
    namespace IO
    {
        void GetFilesInFolder(const fs::path& folderPath, std::vector<std::string>& result);
        const char* ReadSpvFile(uint32_t& length, const char* filename);
        const char* ReadFile(size_t& length, const char* filename);
        const char* ReadTxtFile(uint32_t& length, const char* filename);
        unsigned char* ReadKtxImage(uint32_t& width, uint32_t& height, uint32_t& textureSize, const char* filename);
    }
}
