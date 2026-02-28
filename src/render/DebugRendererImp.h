#ifndef WIREBOUNDWORLDCREATOR_RENDER_DEBUGRENDERIMPL_H
#define WIREBOUNDWORLDCREATOR_RENDER_DEBUGRENDERIMPL_H

#include <Jolt/Core/Mutex.h>
#include <Jolt/Core/UnorderedMap.h>
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include "Renderer.h"

class Renderer;
class Font;

/// Implementation of DebugRenderer
class DebugRendererImp final : public JPH::DebugRenderer {
 public:
  /// Constructor
  DebugRendererImp(Renderer *inRenderer);

  /// Implementation of DebugRenderer interface
  void DrawLine(JPH::RVec3 inFrom, JPH::RVec3 inTo,
                JPH::ColorArg inColor) override;
  void DrawTriangle(JPH::RVec3 inV1, JPH::RVec3 inV2, JPH::RVec3 inV3,
                    JPH::ColorArg inColor, ECastShadow inCastShadow) override;
  Batch CreateTriangleBatch(const Triangle *inTriangles,
                            int inTriangleCount) override;
  Batch CreateTriangleBatch(const Vertex *inVertices, int inVertexCount,
                            const std::uint32_t *inIndices,
                            int inIndexCount) override;
  void DrawGeometry(JPH::RMat44Arg inModelMatrix,
                    const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq,
                    JPH::ColorArg inModelColor, const GeometryRef &inGeometry,
                    ECullMode inCullMode, ECastShadow inCastShadow,
                    EDrawMode inDrawMode) override;
  void DrawText3D(JPH::RVec3 inPosition, const std::string_view &inString,
                  JPH::ColorArg inColor, float inHeight) override;

  /// Draw all primitives from the light source
  void DrawShadowPass();

  /// Draw all primitives that were added
  void Draw();

  /// Clear all primitives (to be called after drawing)
  void Clear();

 private:
  /// Helper functions to draw sub parts
  void DrawLines();
  void DrawTriangles();
  void DrawTexts();

  /// Helper functions to clear sub parts
  void ClearLines();
  void ClearTriangles();
  void ClearTexts();

  /// Finalize the current locked primitive and add it to the primitives to draw
  void FinalizePrimitive();

  /// Ensure that the current locked primitive has space for a primitive
  /// consisting inVtxSize vertices
  void EnsurePrimitiveSpace(int inVtxSize);

  Renderer *mRenderer;

  /// Shaders for triangles
  std::unique_ptr<PipelineState> mTriangleStateBF;
  std::unique_ptr<PipelineState> mTriangleStateFF;
  std::unique_ptr<PipelineState> mTriangleStateWire;

  /// Shaders for shadow pass for triangles
  std::unique_ptr<PipelineState> mShadowStateBF;
  std::unique_ptr<PipelineState> mShadowStateFF;
  std::unique_ptr<PipelineState> mShadowStateWire;

  /// Lock that protects the triangle batches from being accessed from multiple
  /// threads
  JPH::Mutex mPrimitivesLock;

  Batch mEmptyBatch;

  /// Properties for a single rendered instance
  struct Instance {
    /// Constructor
    Instance(JPH::Mat44Arg inModelMatrix, JPH::Mat44Arg inModelMatrixInvTrans,
             JPH::ColorArg inModelColor)
        : mModelMatrix(inModelMatrix),
          mModelMatrixInvTrans(inModelMatrixInvTrans),
          mModelColor(inModelColor) {}

    JPH::Mat44 mModelMatrix;
    JPH::Mat44 mModelMatrixInvTrans;
    JPH::Color mModelColor;
  };

  /// Rendered instance with added information for lodding
  struct InstanceWithLODInfo : public Instance {
    /// Constructor
    InstanceWithLODInfo(JPH::Mat44Arg inModelMatrix,
                        JPH::Mat44Arg inModelMatrixInvTrans,
                        JPH::ColorArg inModelColor,
                        const JPH::AABox &inWorldSpaceBounds,
                        float inLODScaleSq)
        : Instance(inModelMatrix, inModelMatrixInvTrans, inModelColor),
          mWorldSpaceBounds(inWorldSpaceBounds),
          mLODScaleSq(inLODScaleSq) {}

    /// Bounding box for culling
    JPH::AABox mWorldSpaceBounds;

    /// Square of scale factor for LODding (1 = original, > 1 = lod out further,
    /// < 1 = lod out earlier)
    float mLODScaleSq;
  };

  /// Properties for a batch of instances that have the same primitive
  struct Instances {
    std::vector<InstanceWithLODInfo> mInstances;

    /// Start index in mInstancesBuffer for each of the LOD in the geometry
    /// pass. Length is one longer than the number of LODs to indicate how many
    /// instances the last lod has.
    std::vector<int> mGeometryStartIdx;

    /// Start index in mInstancesBuffer for each of the LOD in the light pass.
    /// Length is one longer than the number of LODs to indicate how many
    /// instances the last lod has.
    std::vector<int> mLightStartIdx;
  };

  using InstanceMap = JPH::UnorderedMap<Geometry *, Instances>;

  /// Clear map of instances and make it ready for the next frame
  void ClearMap(InstanceMap &ioInstances);

  /// Helper function to draw instances
  void DrawInstances(const Geometry *inGeometry,
                     const std::vector<int> &inStartIdx);

  /// List of primitives that are finished and ready for drawing
  InstanceMap mWireframePrimitives;
  InstanceMap mPrimitives;
  InstanceMap mTempPrimitives;
  InstanceMap mPrimitivesBackFacing;
  int mNumInstances = 0;
  RenderInstances *mInstancesBuffer[Renderer::cFrameCount];

  /// Primitive that is being built + its properties
  RenderPrimitive *mLockedPrimitive;
  Vertex *mLockedVerticesStart = nullptr;
  Vertex *mLockedVertices = nullptr;
  Vertex *mLockedVerticesEnd = nullptr;
  JPH::AABox mLockedPrimitiveBounds;

  /// A single text string
  struct Text {
    Text(JPH::Vec3 inPosition, const std::string_view &inText,
         JPH::ColorArg inColor, float inHeight)
        : mPosition(inPosition),
          mText(inText),
          mColor(inColor),
          mHeight(inHeight) {}

    JPH::Vec3 mPosition;
    std::string mText;
    JPH::Color mColor;
    float mHeight;
  };

  /// All text strings that are to be drawn on screen
  std::vector<Text> mTexts;
  JPH::Mutex mTextsLock;

  /// Font with which to draw the texts
  const Font *mFont;

  /// A single line segment
  struct Line {
    JPH::Float3 mFrom;
    JPH::Color mFromColor;
    JPH::Float3 mTo;
    JPH::Color mToColor;
  };

  /// The list of line segments
  std::vector<Line> mLines;
  JPH::Mutex mLinesLock;

  /// The shaders for the line segments
  std::unique_ptr<PipelineState> mLineState;
};

#endif  // WIREBOUNDWORLDCREATOR_RENDER_DEBUGRENDERIMPL_H
