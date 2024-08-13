#include <cstdio>
#include <iostream>
#include <pxr/pxr.h>
#include "pxr/base/plug/registry.h"
#include "pxr/base/plug/plugin.h"

PXR_NAMESPACE_USING_DIRECTIVE


int main(){
    PlugRegistry &PlugManager = PlugRegistry::GetInstance();
    PlugPluginPtrVector pluginList = PlugManager.GetAllPlugins();
    for(auto plug : pluginList){
        std::cout << plug->GetName() <<"\n";
    }

    int a=0;
    scanf("%d",&a);

    PlugManager.RegisterPlugins("/home/teachmain/usd/share/usd/examples/plugin/hdTiny/resources/plugInfo.json");

    scanf("%d",&a);

    pluginList = PlugManager.GetAllPlugins();

    scanf("%d",&a);
    for(auto plug : pluginList){
        std::cout << plug->GetName() <<"\n";
    }
    return 0;
}