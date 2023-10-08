#pragma once

#include <cstdio>
#include <string>
#include <utility>
#include <iostream>
#include <vector>
#include "IRuntimeModule.hpp"
#include "Buffer.hpp"

namespace My {
    class AssetLoader : public IRuntimeModule{
        public:
            virtual ~AssetLoader() {};
            virtual int Initialize();
            virtual void Finalize();
            virtual void Tick();

            typedef void* AssetFilePtr;

            enum AssetOpenMode{
                MY_OPEN_TEXT = 0, //open in text mode
                MY_OPEN_BINARY = 1, //open in binary mode
            };

            enum AssetSeekBase{
                MY_SEEK_SET = 0, //seek set
                MY_SEEK_CUR = 1, //seek cur
                MY_SEEK_END = 2  //seek_end  
            };

            bool AddSearchPath(const char* path);
            bool RemoveSearchPath(const char* path);
            bool FileExists(const char* filePath);

            AssetFilePtr OpenFile(const char* name, AssetOpenMode mode);

            Buffer SyncOpenAndReadText(const char* filePath);
            Buffer SyncOpenAndReadBinary(const char* filePath);

            size_t SyncRead(const AssetFilePtr& fp, Buffer& buf);

            void CloseFile(AssetFilePtr& fp);

            size_t GetSize(const AssetFilePtr& fp);
            int32_t Seek(AssetFilePtr fp, long offset, AssetSeekBase where);

            inline std::string SyncOpenAndReadTextFileToString(const char* filename){
                std::string result;
                Buffer buffer = SyncOpenAndReadText(filename);
                char* content = reinterpret_cast<char*>(buffer.m_pData);

                if(content){
                    result = std::string(std::move(content));
                }
                return result;
            }
        private:
            std::vector<std::string> m_strSearchPath;

    };
    extern AssetLoader* g_pAssetLoader;
}