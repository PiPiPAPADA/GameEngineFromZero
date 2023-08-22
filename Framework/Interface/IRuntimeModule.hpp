#pragma once

#include "Interface.hpp"

namespace My{
    Interface IRuntimeModule{
        public:
            virtual ~IRuntimeModule() {}
            // create init/final func bcz, static value has not fixed order for init
            // some module only for load into memory, then do init 
            // module not used only release memory
            virtual int Initialize() =0;  // initialize module
            virtual void Finalize() =0;  //  After module finished
            virtual void Tick() =0;      // run once
    };
}
