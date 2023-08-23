#pragma once
#include "GrahphicsManager.hpp"

namespace My {
    class OpenGLGraphicsManager : public GraphicsManager{
        public:
            virtual int Initialiaze();
            virtual void Finalize();
            virtual void Tick();
        private:
    };
}