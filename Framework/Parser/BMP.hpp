#pragma once

#include <iostream>
#include "ImageParser.hpp"

namespace My{
#pragma pack(push, 1)
     typedef struct _BITMAP_FILEHEADER {
        uint16_t Signature;
        uint32_t Size;
        uint32_t Reserved;
        uint32_t BitsOffset;
    } BITMAP_FILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

    typedef struct _BITMAP_HEADER {
        uint32_t HeaderSize;
        int32_t Width;
        int32_t Height;
        uint16_t Planes;
        uint16_t BitCount;
        uint32_t Compression;
        uint32_t SizeImage;
        int32_t PelsPerMeterX;
        int32_t PelsPerMeterY;
        uint32_t ClrUsed;
        uint32_t ClrImportant;
    } BITMAP_HEADER;

    typedef struct _RGBA {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Alpha;
    } RGBA;

    typedef struct _BGRA {
        uint8_t Blue;
        uint8_t Green;
        uint8_t Red;
        uint8_t Alpha;
    } BGRA;
#pragma pack(pop)

    class BmpParser : implements ImageParser
    {
    public:
        virtual Image& Parser(const Buffer& buf)
        {
            Image img;
            BITMAP_FILEHEADER* pFileHeader = reinterpret_cast<BITMAP_FILEHEADER*>(buf.m_pData);
            // std::cout<<"Signature "<<pFileHeader->Signature<<"\t"<<"Size "<<pFileHeader->Size<<std::endl;
            BITMAP_HEADER* pBmpHeader = reinterpret_cast<BITMAP_HEADER*>(buf.m_pData + BITMAP_FILEHEADER_SIZE);
            if (pFileHeader->Signature == 0x4D42 /* 'B''M' */) {
                std::cout << "Asset is Windows BMP file" << std::endl;
                std::cout << "BMP Header" << std::endl;
                std::cout << "----------------------------" << std::endl;
                std::cout << "File Size: " << pFileHeader->Size << std::endl;
                std::cout << "Data Offset: " << pFileHeader->BitsOffset << std::endl;
                std::cout << "Image Width: " << pBmpHeader->Width << std::endl;
                std::cout << "Image Height: " << pBmpHeader->Height << std::endl;
                std::cout << "Image Planes: " << pBmpHeader->Planes << std::endl;
                std::cout << "Image BitCount: " << pBmpHeader->BitCount << std::endl;
                std::cout << "Image Compression: " << pBmpHeader->Compression << std::endl;
                std::cout << "Image Size: " << pBmpHeader->SizeImage << std::endl;

                img.Width = pBmpHeader->Width;
                img.Height = pBmpHeader->Height;
                img.bitcount = pBmpHeader->BitCount;
                img.pitch = ((img.Width * img.bitcount >> 3) + 3) & ~3;
                img.data_size = img.pitch * img.Height;
                img.data = reinterpret_cast<uint8_t*>(g_pMemoryManager->Allocate(img.data_size));

                if (img.bitcount < 24) {
                    std::cout << "Sorry, only true color BMP is supported at now." << std::endl;
                } else {
                    uint8_t* pSourceData = buf.m_pData + pFileHeader->BitsOffset;
                    BGRA* pDestData = reinterpret_cast<BGRA*>(img.data);
                    for (int32_t y = img.Height - 1; y >= 0; y--) {
                        for (uint32_t x = 0; x < img.Width; x++) {
                            *(pDestData + img.Width * (img.Height - y - 1) + x) = *reinterpret_cast<BGRA*>(pSourceData + img.pitch * y + x * (img.bitcount >> 3));
                        }
                    }
                }
            }

            return img;
        }
    };
}