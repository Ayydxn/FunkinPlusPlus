#include "FunkinPCH.h"
#include "FunkinEngine.h"

#include <assert.h>

CFunkinEngine* GFunkinEngine = nullptr;

CFunkinEngine::CFunkinEngine()
{
    assert(GFunkinEngine == nullptr);
    
    GFunkinEngine = this;
}

CFunkinEngine::~CFunkinEngine()
{
    GFunkinEngine = nullptr;
}
