#include "pxr/pxr.h"

#include "pxr/base/tf/errorMark.h"
#include "pxr/base/tf/token.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/vec3f.h"

#include "pxr/imaging/hd/engine.h"
#include "pxr/imaging/hd/sceneIndex.h"
#include "pxr/imaging/hd/rendererPlugin.h"
#include "pxr/imaging/hd/rendererPluginRegistry.h"
#include "pxr/imaging/hd/unitTestDelegate.h"
#include "pxr/imaging/hdx/renderTask.h"
#include "pxr/base/plug/registry.h"
#include "pxr/base/plug/plugin.h"

#include "pxr/imaging/hd/renderIndex.h"
#include "pxr/imaging/hf/pluginDesc.h"
#include "pxr/imaging/hd/meshTopologySchema.h"
#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/base/vt/array.h"
#include "pxr/imaging/hd/visibilitySchema.h"
#include "pxr/imaging/hd/dataSource.h"
#include "pxr/imaging/hd/dataSourceTypeDefs.h"
#include "pxr/imaging/hd/xformSchema.h"
#include "pxr/imaging/hd/meshSchema.h"
#include "pxr/imaging/hd/integratorSchema.h"
#include "pxr/imaging/hd/extentSchema.h"
#include "pxr/imaging/hd/purposeSchema.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/tokens.h"
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

void RunHydra()
{
    // Get the renderer plugin and create a new render delegate and index.

    PlugRegistry &PlugManager = PlugRegistry::GetInstance();
    PlugManager.RegisterPlugins("/home/teachmain/usd/share/usd/examples/plugin/hdTiny/resources/plugInfo.json");

    HfPluginDescVector hfplugin;
    HdRendererPluginRegistry &HdRenderPluginRE  = HdRendererPluginRegistry::GetInstance();
    HdRenderPluginRE.GetPluginDescs(&hfplugin);
    printf("Found HfPlugins: \n");
    for(auto plugin : hfplugin){
        std::cout << plugin.id << "\n";
    }
    
    const TfToken tinyRendererPluginId("HdTinyRendererPlugin");
    if(HdRenderIndex::IsSceneIndexEmulationEnabled()){
        printf("RenderIndex SceneIndex Emulation is enable\n");
    }

    HdRendererPlugin *rendererPlugin = HdRendererPluginRegistry::GetInstance()
        .GetRendererPlugin(tinyRendererPluginId);
    TF_VERIFY(rendererPlugin != nullptr);

    HdRenderDelegate *renderDelegate = rendererPlugin->CreateRenderDelegate();
    TF_VERIFY(renderDelegate != nullptr);

    HdRenderIndex *renderIndex = HdRenderIndex::New(renderDelegate, {});
    TF_VERIFY(renderIndex != nullptr);

    // Construct a new scene delegate to populate the render index.
    HdUnitTestDelegate *sceneDelegate = new HdUnitTestDelegate(renderIndex, 
        SdfPath::AbsoluteRootPath());
    TF_VERIFY(sceneDelegate != nullptr);

    // Create a cube.
    sceneDelegate->AddCube(SdfPath("/MyCube1"), GfMatrix4f(1));

    // Let's use the HdxRenderTask as an example, and configure it with
    // basic parameters.
    //
    // Another option here could be to create your own task which would
    // look like this :
    //
    // class MyDrawTask final : public HdTask
    // {
    // public:
    //     MyDrawTask(HdRenderPassSharedPtr const &renderPass,
    //                HdRenderPassStateSharedPtr const &renderPassState,
    //                TfTokenVector const &renderTags)
    //     : HdTask(SdfPath::EmptyPath()) { }
    // 
    //     void Sync(HdSceneDelegate* delegate,
    //         HdTaskContext* ctx,
    //         HdDirtyBits* dirtyBits) override { }
    //
    //     void Prepare(HdTaskContext* ctx,
    //         HdRenderIndex* renderIndex) override { }
    //
    //     void Execute(HdTaskContext* ctx) override { }
    // };
    SdfPath renderTask("/renderTask");
    sceneDelegate->AddTask<HdxRenderTask>(renderTask);
    sceneDelegate->UpdateTask(renderTask, HdTokens->params, 
        VtValue(HdxRenderTaskParams()));
    sceneDelegate->UpdateTask(renderTask, 
        HdTokens->collection,
        VtValue(HdRprimCollection(HdTokens->geometry, 
        HdReprSelector(HdReprTokens->refined))));

    // Ask Hydra to execute our render task.
    HdEngine engine;
    HdTaskSharedPtrVector tasks = { renderIndex->GetTask(renderTask) };
    engine.Execute(renderIndex, &tasks);

    // Destroy the data structures
    delete renderIndex;
    delete renderDelegate;
    delete sceneDelegate;
}

int main(int argc, char *argv[])
{
    TfErrorMark mark;
    RunHydra();

    // If no error messages were logged, return success.
    if (mark.IsClean()) {
        std::cout << "OK" << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << "FAILED" << std::endl;
        return EXIT_FAILURE;
    }
}
