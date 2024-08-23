#include <cstddef>
#include<iostream>
#include <pxr/base/gf/vec4d.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/token.h>
#include <pxr/imaging/hd/driver.h>
#include <pxr/imaging/hd/mergingSceneIndex.h>
#include <pxr/imaging/hd/renderDelegate.h>
#include <pxr/imaging/hd/retainedSceneIndex.h>
#include <pxr/imaging/hd/sceneIndex.h>
#include <pxr/imaging/hd/sceneIndexObserver.h>
#include <pxr/imaging/hd/tokens.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>
#include <ratio>
#include "TriSceneIndex.h"
#include "pxr/base/plug/plugin.h"
#include "pxr/base/plug/registry.h"
#include "pxr/imaging/hf/pluginRegistry.h"
#include "pxr/imaging/hd/rendererPluginRegistry.h"
#include "pxr/imaging/hd/rendererPlugin.h"
#include "pxr/imaging/hd/renderIndex.h"
#include "pxr/imaging/hgi/hgi.h"
#include "pxr/imaging/hd/renderBufferSchema.h"
#include "pxr/usdImaging/usdImaging/stageSceneIndex.h"

#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/imaging/hd/meshSchema.h"
#include "pxr/imaging/hd/meshTopologySchema.h"
#include "pxr/imaging/hd/dataSourceTypeDefs.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/purposeSchema.h"
#include "pxr/imaging/hd/visibilitySchema.h"
#include "pxr/imaging/hd/xformSchema.h"
#include "pxr/imaging/hd/extentSchema.h"
#include "pxr/imaging/hd/cameraSchema.h"


PXR_NAMESPACE_USING_DIRECTIVE

void viewSubtree(HdSceneIndexBaseRefPtr scene,const SdfPath &primPath,int deepth){
    for(int i=0;i<deepth;i++){
        printf("    ");
    }
    SdfPathVector children = scene->GetChildPrimPaths(primPath);
    std::cout << primPath.GetElementString() << std::endl;
    for(SdfPath child : children){
        viewSubtree(scene,child, deepth+1);
    }

}
void PrintTfTokenList(const std::string title, TfTokenVector list, int elementPerLine=1){

    int titleLen = title.length();
    int remain = (40 - titleLen)/2;
    if(title.length()>39){
        return;
    }
    std::cout << std::string(remain,'-') << title << std::string(40-remain,'-')<<'\n';
    int counter = 0;
    for(TfToken str : list){
        
    }
    
}

void PrintRenderDelegateInfo(HdRenderDelegate *renderer){
    TfTokenVector supportedRprimType = renderer -> GetSupportedRprimTypes();
    std::cout << "Supported Rprim include:\n";
    for(TfToken type : supportedRprimType){
        std::cout << type << '\n';
    }
    std::cout << "--------------------\n";

    std::cout << "Supported Sprim include:\n";
    TfTokenVector supportedSprimType = renderer -> GetSupportedSprimTypes();
    for(TfToken type : supportedSprimType){
        std::cout << type << '\n';
    }
    std::cout << "--------------------\n";

    std::cout << "Supported Bprim include:\n";
    TfTokenVector supportedBprimType = renderer -> GetSupportedBprimTypes();
    for(TfToken type : supportedBprimType){
        std::cout << type << '\n';
    }
    std::cout << "--------------------\n";
}

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
    if(!renderIndex->IsSprimTypeSupported(HdPrimTypeTokens->camera)){
        std::cout << "Camera not support?" << std::endl;
        return -1;
    }

    UsdImagingStageSceneIndexRefPtr usdScene = UsdImagingStageSceneIndex::New();
    UsdStageRefPtr testStage = UsdStage::Open("/home/teachmain/Kitchen_set/Kitchen_set.usd");

    HdMergingSceneIndexRefPtr mergeScene = HdMergingSceneIndex::New();
    mergeScene->AddInputScene(usdScene, SdfPath::AbsoluteRootPath());

    HdRetainedSceneIndexRefPtr retainScene = HdRetainedSceneIndex::New();
    mergeScene->AddInputScene(retainScene, SdfPath::AbsoluteRootPath());

    HdRetainedSceneIndex::AddedPrimEntry tmpEntry;
    tmpEntry.primPath = SdfPath("/camera");
    tmpEntry.primType = HdPrimTypeTokens->camera;
    tmpEntry.dataSource = HdCameraSchema::BuildRetained(
        HdCameraSchema::BuildProjectionDataSource(HdCameraSchemaTokens->perspective),
        HdRetainedTypedSampledDataSource<float>::New(1.0f),
        HdRetainedTypedSampledDataSource<float>::New(1.0f),
        HdRetainedTypedSampledDataSource<float>::New(0.0f),
        HdRetainedTypedSampledDataSource<float>::New(0.0f),
        HdRetainedTypedSampledDataSource<float>::New(50.0f),
        HdRetainedTypedSampledDataSource<GfVec2f>::New({0.001f,1000000.0f}),
        nullptr,
        HdRetainedTypedSampledDataSource<float>::New(21.0f),
        HdRetainedTypedSampledDataSource<float>::New(1000.0f),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    renderIndex->InsertSceneIndex(mergeScene,SdfPath::AbsoluteRootPath());
    retainScene->AddPrims({tmpEntry});

    usdScene->SetStage(testStage);


    return 0; 
}
