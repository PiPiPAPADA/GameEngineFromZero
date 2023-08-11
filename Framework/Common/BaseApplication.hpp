#pragma once
#include "IApplication.hpp"

namespace My{
    class BaseApplication : implements IApplication{
        public:
            virtual int Initialize();
            virtual void Finalize();
            virtual void Tick();
            virtual bool IsQuit();
        protected:
            // flag for quit from main loop
            bool m_bQuit;
    };
}