#include<iostream>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/token.h>
#include <pxr/imaging/hd/driver.h>
#include <pxr/imaging/hd/renderDelegate.h>
#include <pxr/imaging/hd/sceneIndex.h>
#include <pxr/imaging/hd/sceneIndexObserver.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include "TriSceneIndex.h"
#include "pxr/base/plug/plugin.h"
#include "pxr/base/plug/registry.h"
#include "pxr/imaging/hf/pluginRegistry.h"
#include "pxr/imaging/hd/rendererPluginRegistry.h"
#include "pxr/imaging/hd/rendererPlugin.h"
#include "pxr/imaging/hd/renderIndex.h"
#include "pxr/imaging/hgi/hgi.h"
#include "pxr/imaging/hd/renderBufferSchema.h"


PXR_NAMESPACE_USING_DIRECTIVE
int main(){
    PlugRegistry &PlugManager = PlugRegistry::GetInstance();
    PlugManager.RegisterPlugins("/home/teachmain/usd/share/usd/examples/plugin/hdTiny/resources/plugInfo.json");

    HfPluginDescVector hfplugin;
    HdRendererPluginRegistry &HdRenderPluginRE  = HdRendererPluginRegistry::GetInstance();
    HdRenderPluginRE.GetPluginDescs(&hfplugin);
    printf("Found Renderer: \n");
    for(int index=0;index<hfplugin.size();index++){
        std::cout << '['<<index<<']'<<'\t'<<hfplugin[index].id<<"\tDisplayName: "<<hfplugin[index].displayName<<std::endl;
    }
    int selectIndex=0;
    std::cin >> selectIndex;

    const TfToken RendererID(hfplugin[selectIndex].id);

    HdRendererPlugin *rendererPlugin = HdRendererPluginRegistry::GetInstance()
        .GetRendererPlugin(RendererID);
    TF_VERIFY(rendererPlugin != nullptr);

    HdRenderDelegate *renderDelegate = rendererPlugin->CreateRenderDelegate();
    TF_VERIFY(renderDelegate != nullptr);
    HdRenderSettingDescriptorList settings = renderDelegate->GetRenderSettingDescriptors();
    
    std::cout << "------Render Settings------"<<std::endl;
    for (HdRenderSettingDescriptor setting : settings){
        std::cout <<setting.key << '\t'<<setting.defaultValue <<'\t'<<setting.name << std::endl;
    }
    std::cout << "---------------------------"<<std::endl;


    HdRenderIndex *renderIndex = HdRenderIndex::New(renderDelegate, HdDriverVector(),"test");
    TF_VERIFY(renderIndex != nullptr);
    std::cout << "Create renderIndex  for: "<<RendererID << std::endl;
    TriSceneIndexRefPtr testScene = TfCreateRefPtr(new TriSceneIndex());
    renderIndex->InsertSceneIndex(testScene,SdfPath::AbsoluteRootPath());
    testScene->Populate(false);
    testScene->Populate(true);

    return 0; 
}
