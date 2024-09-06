#include <cstddef>
#include <cstdio>
#include<iostream>
#include <memory>
#include <ostream>
#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/gf/vec3d.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec3i.h>
#include <pxr/base/gf/vec4d.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/token.h>
#include <pxr/base/vt/dictionary.h>
#include <pxr/imaging/hd/aov.h>
#include <pxr/imaging/hd/renderBuffer.h>
#include <pxr/imaging/hdx/renderTask.h>
#include <pxr/imaging/hd/driver.h>
#include <pxr/imaging/hd/mergingSceneIndex.h>
#include <pxr/imaging/hd/renderDelegate.h>
#include <pxr/imaging/hd/retainedSceneIndex.h>
#include <pxr/imaging/hd/sceneIndex.h>
#include <pxr/imaging/hd/sceneIndexObserver.h>
#include <pxr/imaging/hd/tokens.h>
#include <pxr/imaging/hd/types.h>
#include <pxr/pxr.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usd/primTypeInfo.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xformOp.h>
#include <ratio>
#include "TriSceneIndex.h"
#include "pxr/base/plug/plugin.h"
#include "pxr/base/plug/registry.h"
#include "pxr/base/vt/value.h"
#include "pxr/imaging/hd/changeTracker.h"
#include "pxr/imaging/hd/dataSource.h"
#include "pxr/imaging/hd/repr.h"
#include "pxr/imaging/hd/rprimCollection.h"
#include "pxr/imaging/hdx/renderSetupTask.h"
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
#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"
#include "pxr/imaging/hd/sceneDelegate.h"
#include "pxr/imaging/hd/engine.h"
#include "pxr/imaging/hd/renderPass.h"
#include "pxr/imaging/hd/camera.h"
#include "pxr/imaging/hio/image.h"
#include "pxr/imaging/hio/types.h"


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
    std::cout << std::string(remain,'-') << title << std::string(40-remain-titleLen,'-')<<'\n';
    int counter = 0;
    for(TfToken str : list){
        std::cout << str << '\n';
    }
    std::cout << "----------------------------------------\n";
}

void PrintRenderDelegateInfo(HdRenderDelegate *renderer){
    PrintTfTokenList("Supported Rprim",renderer->GetSupportedRprimTypes());
    PrintTfTokenList("Supported Bprim",renderer->GetSupportedBprimTypes());
    PrintTfTokenList("Supported Sprim",renderer->GetSupportedSprimTypes());
    PrintTfTokenList("Render Settings", renderer->GetRenderSettingsNamespaces());
}
void CameraTest(){
}

int main(){
    PlugRegistry &PlugManager = PlugRegistry::GetInstance();
    PlugManager.RegisterPlugins("/home/teachmain/usd/share/usd/examples/plugin/hdTiny/resources/plugInfo.json");
    PlugManager.RegisterPlugins("/home/teachmain/usd/lib/usd/plugInfo.json");
    PlugManager.RegisterPlugins("/home/teachmain/usd/plugin/usd/plugInfo.json");

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
    PrintRenderDelegateInfo(renderDelegate);

    HdRenderSettingDescriptorList settings = renderDelegate->GetRenderSettingDescriptors();
    
    std::cout << "------Render Settings------"<<std::endl;
    for (HdRenderSettingDescriptor setting : settings){
        std::cout <<setting.key << '\t'<<setting.defaultValue <<'\t'<<setting.name << std::endl;
    }
    std::cout << "---------------------------"<<std::endl;

    VtDictionary status = renderDelegate->GetRenderStats();
    for(auto [key,value] : status){
        std:: cout << key << '\t' << value << std::endl;
    }


    HdRenderIndex *renderIndex = HdRenderIndex::New(renderDelegate, HdDriverVector(),"test");
    TF_VERIFY(renderIndex != nullptr);

    UsdImagingStageSceneIndexRefPtr usdScene = UsdImagingStageSceneIndex::New();
    UsdStageRefPtr testStage = UsdStage::Open("/home/teachmain/Kitchen_set/Kitchen_set.usd");
    usdScene->SetStage(testStage);
    HdMergingSceneIndexRefPtr mergeIndex = HdMergingSceneIndex::New();
    mergeIndex->AddInputScene(usdScene, SdfPath::AbsoluteRootPath());




    GfMatrix4d cameraXform;
    cameraXform.SetLookAt(GfVec3d(0,0,10),GfVec3d(0,0,-1),GfVec3d(0,1,0));

    HdRetainedSceneIndex::AddedPrimEntry cameraEntry;
    cameraEntry.primPath = SdfPath("/camera");
    cameraEntry.primType = HdPrimTypeTokens->camera;
    cameraEntry.dataSource = HdRetainedContainerDataSource::New(
        HdCameraSchemaTokens->camera,
        HdCameraSchema::BuildRetained(
            HdCameraSchema::BuildProjectionDataSource(HdCameraSchemaTokens->perspective),
            HdRetainedTypedSampledDataSource<float>::New(100.0f),
            HdRetainedTypedSampledDataSource<float>::New(100.0f),
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
            nullptr,
            nullptr
        ),
        HdXformSchemaTokens->xform,
        HdXformSchema::BuildRetained(
            HdRetainedTypedSampledDataSource<GfMatrix4d>::New(cameraXform),
            HdRetainedTypedSampledDataSource<bool>::New(true)
        )
    );
    HdSceneIndexPrim prim = {
        HdPrimTypeTokens->mesh,
    };
    HdRetainedSceneIndex::AddedPrimEntry primEntry;
    primEntry.primPath = SdfPath("/quad");
    primEntry.primType = HdPrimTypeTokens->mesh; 
    primEntry.dataSource = HdRetainedContainerDataSource::New(

        HdMeshSchemaTokens->mesh,
        HdMeshSchema::BuildRetained(
            HdMeshTopologySchema::BuildRetained(
                HdRetainedTypedSampledDataSource<VtArray<int>>::New({4}),
                HdRetainedTypedSampledDataSource<VtArray<int>>::New({0,1,2,3}),
                nullptr,
                HdRetainedTypedSampledDataSource<TfToken>::New(HdTokens->rightHanded)),
            HdRetainedTypedSampledDataSource<TfToken>::New(HdPrimvarRoleTokens->none),
            nullptr,
            HdRetainedTypedSampledDataSource<bool>::New(true)),

        HdPrimvarsSchemaTokens->primvars,
        HdRetainedContainerDataSource::New(
            HdTokens->points,
            HdPrimvarSchema::BuildRetained(
                HdRetainedTypedSampledDataSource<VtArray<GfVec3f>>::New({{-1,-1,0},{1,-1,0},{1,1,0},{-1,1,0}}),
                nullptr,
                nullptr,
                HdRetainedTypedSampledDataSource<TfToken>::New(HdPrimvarSchemaTokens->vertex),
                HdRetainedTypedSampledDataSource<TfToken>::New(HdPrimvarSchemaTokens->point))),

        HdPurposeSchemaTokens->purpose,
        HdPurposeSchema::BuildRetained(
            HdRetainedTypedSampledDataSource<TfToken>::New(HdRenderTagTokens->geometry)),

        HdVisibilitySchemaTokens->visibility,
        HdVisibilitySchema::BuildRetained(
                HdRetainedTypedSampledDataSource<bool>::New(true)),

        HdXformSchemaTokens->xform,
        HdXformSchema::BuildRetained(
            HdRetainedTypedSampledDataSource<GfMatrix4d>::New(GfMatrix4d(1)),
            HdRetainedTypedSampledDataSource<bool>::New(false)),
            HdExtentSchemaTokens->extent,
            HdExtentSchema::BuildRetained(
                HdRetainedTypedSampledDataSource<GfVec3d>::New(GfVec3d(-1,-1,0)),
                HdRetainedTypedSampledDataSource<GfVec3d>::New(GfVec3d(1,1,0)))
    );

    HdRetainedSceneIndexRefPtr retainScene = HdRetainedSceneIndex::New();
    mergeIndex->AddInputScene(retainScene,SdfPath::AbsoluteRootPath());

    renderIndex->InsertSceneIndex(mergeIndex,SdfPath::AbsoluteRootPath());

    HdContainerDataSourceHandle data = 
    HdRetainedContainerDataSource::New(
        HdPrimTypeTokens->renderBuffer,
        HdRenderBufferSchema::BuildRetained(
            HdRetainedTypedSampledDataSource<GfVec3i>::New({1920,1080,1}),
            HdRetainedTypedSampledDataSource<HdFormat>::New(HdFormat::HdFormatUNorm8Vec4),
            HdRetainedTypedSampledDataSource<bool>::New(false)));
    
    
    HdRetainedSceneIndex::AddedPrimEntry bufferEntry;
    bufferEntry.primPath = SdfPath("/buffer");
    bufferEntry.primType = HdPrimTypeTokens->renderBuffer;
    bufferEntry.dataSource = data;

    retainScene->AddPrims({cameraEntry,primEntry,bufferEntry});


    HdRenderBuffer *rb = static_cast<HdRenderBuffer*>(
        renderIndex->GetBprim(HdPrimTypeTokens->renderBuffer,SdfPath("/buffer")));


    HdSceneDelegate *scene_delegate = renderIndex->GetSceneDelegateForRprim(SdfPath("/quad"));
    HdRenderParam *param = renderDelegate->GetRenderParam();

    HdRenderPassAovBinding aovbinding;
    aovbinding.aovName = HdAovTokens->color;
    aovbinding.clearValue = VtValue(GfVec4f(1.0f, 0.0f, 1.0f, 1.0f));
    aovbinding.renderBufferId = SdfPath("/buffer");
    aovbinding.renderBuffer = rb;

    HdRenderPassSharedPtr renderpass = renderDelegate->CreateRenderPass(renderIndex, HdRprimCollection(HdTokens->geometry,HdReprSelector(HdReprTokens->hull)));
    HdRenderPassStateSharedPtr render_pass_state = renderDelegate->CreateRenderPassState();
    render_pass_state->SetAovBindings({aovbinding});
    render_pass_state->SetViewport(GfVec4d(0,0,1920,1080));

    HdCamera *cameraOBJ =static_cast<HdCamera *>( renderIndex->GetSprim(HdPrimTypeTokens->camera, SdfPath("/camera")));

    render_pass_state->SetCamera(cameraOBJ);

    HdxRenderTaskParams renderparam;
    renderparam.aovBindings.push_back(aovbinding);
    renderparam.camera = SdfPath("/camera");
    renderparam.viewport = GfVec4d(0,0,1920,1080);

    auto rendertask = 
    std::make_shared<HdxRenderTask>(scene_delegate,SdfPath("/rendertask"));
    HdTaskContext ct;
    ct[HdTokens->params] = renderparam;


    HdTaskSharedPtrVector tasks = {rendertask};
    HdEngine engine;


    int times = 0;
    do{
        times++;
        engine.Execute(renderIndex,&tasks);
    }while(!rendertask->IsConverged());

    std::cout << times << std::endl;
    
    rb->Resolve();
    HioImage::StorageSpec storage;
    storage.width = rb->GetWidth();
    storage.height = rb->GetHeight();
    storage.format = HioFormat::HioFormatUNorm8Vec4;
    storage.flipped = true;
    storage.data = rb->Map();
    VtDictionary metadata;
    HioImageSharedPtr image = HioImage::OpenForWriting("test.png");
    if (image) {
        image->Write(storage, metadata);
    }

    rb->Unmap();

    return 0; 
}
