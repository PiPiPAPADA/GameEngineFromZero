#pragma once
#include "Interface.hpp"
#include "Image.hpp"
#include "Buffer.hpp"

namespace My
{
    Interface ImageParser{
        public:
            virtual Image& Parser(const Buffer& buf) = 0;
    };
    
} // namespace My
