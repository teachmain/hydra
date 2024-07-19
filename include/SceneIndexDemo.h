#include "pxr/imaging/hd/sceneIndex.h"
#include <pxr/usd/sdf/path.h>

PXR_NAMESPACE_OPEN_SCOPE
class HdSceneIndexDemo : public HdSceneIndexBase{
    public:

    HD_API
    HdSceneIndexDemo();

    HD_API
    ~HdSceneIndexDemo() override;

    HdSceneIndexPrim GetPrim(const SdfPath &primPath);
    SdfPathVector GetChildPrimPaths(const SdfPath &primPath);

};
PXR_NAMESPACE_CLOSE_SCOPE