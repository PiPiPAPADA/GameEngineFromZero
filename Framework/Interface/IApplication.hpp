#pragma once
#include "Interface.hpp"
#include "IRuntimeModule.hpp"
#include "GfxConfiguration.h"

namespace My{
    Interface IApplication : implements IRuntimeModule{
        public:
            virtual int Initialize() =0;
            virtual void Finalize() = 0;
            virtual void Tick() =0;
            // check application whether quit, quit was trick by os
            virtual bool IsQuit() =0;
            virtual GfxConfiguration& GetConfiguration() = 0;
    };
}