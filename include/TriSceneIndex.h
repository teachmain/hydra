#include "pxr/pxr.h"
#include <pxr/imaging/hd/sceneIndex.h>
#include <pxr/usd/sdf/path.h>
PXR_NAMESPACE_OPEN_SCOPE
TF_DECLARE_REF_PTRS(TriSceneIndex);
class TriSceneIndex : public HdSceneIndexBase{
    public:
    TriSceneIndex();
    void Populate(bool); 
    virtual HdSceneIndexPrim GetPrim(const SdfPath &primPath) const;
    virtual SdfPathVector GetChildPrimPaths(const SdfPath &primPath) const;
    private:
    bool _isPopulated;
};
PXR_NAMESPACE_CLOSE_SCOPE