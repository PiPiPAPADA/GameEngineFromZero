#pragma once
#include "IApplication.hpp"
#include "GrahphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"

namespace My{
    class BaseApplication: implements IApplication{
        public:
            BaseApplication(GfxConfiguration &cfg);
            virtual int Initialize();
            virtual void Finalize();
            virtual void Tick();
            virtual bool IsQuit();

            inline GfxConfiguration& GetConfiguration() {return m_Config;};
        protected:
            virtual void OnDraw(){};
            // flag for quit from main loop
            static bool m_bQuit;
            GfxConfiguration m_Config;
        private:
            //hide default construct to enforce a configuration
            BaseApplication() {};
    };
}