#include <iostream>
#include <pxr/pxr.h>
#include "pxr/base/plug/registry.h"
#include "pxr/base/plug/plugin.h"

PXR_NAMESPACE_USING_DIRECTIVE


int main(){
    PlugRegistry &PlugManager = PlugRegistry::GetInstance();
    PlugPluginPtrVector pluginList = PlugManager.RegisterPlugins("/home/teachmain/usd/share/usd/examples/plugin/hdTiny/resources/plugInfo.json");
    for(auto plug : pluginList){
        std::cout << plug->GetName() <<"\n";
    }
    return 0;
}