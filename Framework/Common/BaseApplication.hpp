#pragma once
#include "IApplication.hpp"
#include "GfxConfiguration.h"

namespace My{
    class BaseApplication: implements IApplication{
        public:
            BaseApplication(GfxConfiguration &cfg);
            virtual int Initialize();
            virtual void Finalize();
            virtual void Tick();
            virtual bool IsQuit();
        protected:
            // flag for quit from main loop
            static bool m_bQuit;
            GfxConfiguration m_Config;
        private:
            //hide default construct to enforce a configuration
            BaseApplication() {};
    };
}