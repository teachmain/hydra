#include "pxr/pxr.h"
#include "pxr/base/vt/array.h"
#include "pxr/imaging/hd/sceneIndex.h"
#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/imaging/hd/meshSchema.h"
#include "pxr/imaging/hd/meshTopologySchema.h"
#include "pxr/base/tf/token.h"
#include "pxr/base/vt/array.h"
#include "pxr/imaging/hd/dataSourceTypeDefs.h"
#include "pxr/imaging/hd/schemaTypeDefs.h"
#include "pxr/imaging/hd/tokens.h"
#include "pxr/imaging/hd/primvarsSchema.h"
#include "pxr/imaging/hd/purposeSchema.h"
#include "pxr/imaging/hd/visibilitySchema.h"
#include "pxr/imaging/hd/xformSchema.h"
#include "pxr/imaging/hd/extentSchema.h"
#include <pxr/base/vt/array.h>
#include <pxr/pxr.h>
#include "TriSceneIndex.h"

PXR_NAMESPACE_OPEN_SCOPE

TriSceneIndex::TriSceneIndex(){
}
void TriSceneIndex::Populate(bool populate) {
    if (populate && !_isPopulated) {
        _SendPrimsAdded({{SdfPath("/Quad"), HdPrimTypeTokens->mesh}});
    } else if (!populate && _isPopulated) {
        _SendPrimsRemoved({{SdfPath("/Quad")}});
    }
    _isPopulated = populate;
}

HdSceneIndexPrim TriSceneIndex::GetPrim(const SdfPath &primPath) const {
    static HdSceneIndexPrim prim = {
        HdPrimTypeTokens->mesh,
        HdRetainedContainerDataSource::New(
            HdMeshSchemaTokens->mesh,
                HdMeshSchema::BuildRetained(
                    /* topology = */ HdMeshTopologySchema::BuildRetained(
                        /* faceVertexCounts = */ HdRetainedTypedSampledDataSource<VtArray<int>>::New({4}),
                        /* faceVertexIndices = */ HdRetainedTypedSampledDataSource<VtArray<int>>::New({0,1,2,3}),
                        /* holeIndices = */ nullptr,
                        /* orientation = */
                            HdRetainedTypedSampledDataSource<TfToken>::New(HdTokens->rightHanded)),
                    /* subdivisionScheme = */ HdRetainedTypedSampledDataSource<TfToken>::New(HdPrimvarRoleTokens->none),
                    /* subdivisionTags = */ nullptr,
                    /* geomSubsets = */ nullptr,
                    /* doubleSided = */ HdRetainedTypedSampledDataSource<bool>::New(true)),
            HdPrimvarsSchemaTokens->primvars,
                HdRetainedContainerDataSource::New(
                    HdTokens->points,
                    HdPrimvarSchema::BuildRetained(
                        /* primvarValue = */ HdRetainedTypedSampledDataSource<VtArray<GfVec3f>>::New(
                            {{-1,-1,0},{1,-1,0},{1,1,0},{-1,1,0}}),
                        /* indexedPrimvarValue = */ nullptr,
                        /* indices = */ nullptr,
                        /* interpolation = */ HdRetainedTypedSampledDataSource<TfToken>::New(HdPrimvarSchemaTokens->vertex),
                        /* role = */
                            HdRetainedTypedSampledDataSource<TfToken>::New(HdPrimvarSchemaTokens->point))),
            HdPurposeSchemaTokens->purpose,
                HdPurposeSchema::BuildRetained(
                    /* purpose = */
                        HdRetainedTypedSampledDataSource<TfToken>::New(HdRenderTagTokens->geometry)),
            HdVisibilitySchemaTokens->visibility,
                HdVisibilitySchema::BuildRetained(
                    /* visibility = */ HdRetainedTypedSampledDataSource<bool>::New(true)),
            HdXformSchemaTokens->xform,
                HdXformSchema::BuildRetained(
                    /* xform = */ HdRetainedTypedSampledDataSource<GfMatrix4d>::New(GfMatrix4d(1)),
                    /* resetXformStack = */ HdRetainedTypedSampledDataSource<bool>::New(false)),
            HdExtentSchemaTokens->extent,
                HdExtentSchema::BuildRetained(
                    /* min = */ HdRetainedTypedSampledDataSource<GfVec3d>::New(GfVec3d(-1,-1,0)),
                    /* max = */ HdRetainedTypedSampledDataSource<GfVec3d>::New(GfVec3d(1,1,0)))
        )
    };
    if (primPath == SdfPath("/Quad")) {
        return prim;
    } else {
        return { TfToken(), nullptr };
    }
}

SdfPathVector TriSceneIndex::GetChildPrimPaths(const SdfPath &primPath) const {
    if (primPath == SdfPath::AbsoluteRootPath()) {
        return  {SdfPath("/Quad")} ;
    } else {
        return {};
    }
}

PXR_NAMESPACE_CLOSE_SCOPE