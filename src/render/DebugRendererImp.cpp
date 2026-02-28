#include "DebugRendererImp.h"

#include "Font.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

DebugRendererImp::DebugRendererImp(Renderer *inRenderer)
    : mRenderer(inRenderer), mFont(inFont) {
  // Create input layout for lines
  const PipelineState::EInputDescription line_vertex_desc[] = {
      PipelineState::EInputDescription::Position,
      PipelineState::EInputDescription::Color};

  // Lines
  line_shader_ = Shader("");
  mLineState = mRenderer->CreatePipelineState(
      line_vertex_desc, std::size(line_vertex_desc),
      PipelineState::EDrawPass::Normal, PipelineState::EFillMode::Solid,
      PipelineState::ETopology::Line, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::Backface);

  // Create input layout for triangles
  const PipelineState::EInputDescription triangles_vertex_desc[] = {
      PipelineState::EInputDescription::Position,
      PipelineState::EInputDescription::Normal,
      PipelineState::EInputDescription::TexCoord,
      PipelineState::EInputDescription::Color,
      PipelineState::EInputDescription::InstanceTransform,
      PipelineState::EInputDescription::InstanceInvTransform,
      PipelineState::EInputDescription::InstanceColor};

  // Triangles
  triangle_shader_ = Shader("");
  mTriangleStateBF = mRenderer->CreatePipelineState(
      triangles_vertex_desc, std::size(triangles_vertex_desc),
      PipelineState::EDrawPass::Normal, PipelineState::EFillMode::Solid,
      PipelineState::ETopology::Triangle, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::Backface);
  mTriangleStateFF = mRenderer->CreatePipelineState(
      triangles_vertex_desc, std::size(triangles_vertex_desc),
      PipelineState::EDrawPass::Normal, PipelineState::EFillMode::Solid,
      PipelineState::ETopology::Triangle, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::FrontFace);
  mTriangleStateWire = mRenderer->CreatePipelineState(
      triangles_vertex_desc, std::size(triangles_vertex_desc),
      PipelineState::EDrawPass::Normal, PipelineState::EFillMode::Wireframe,
      PipelineState::ETopology::Triangle, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::Backface);

  // Shadow pass
  triangle_depth_vertex_shader_ = Shader("");
  mShadowStateBF = mRenderer->CreatePipelineState(
      triangles_vertex_desc, std::size(triangles_vertex_desc),
      PipelineState::EDrawPass::Shadow, PipelineState::EFillMode::Solid,
      PipelineState::ETopology::Triangle, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::Backface);
  mShadowStateFF = mRenderer->CreatePipelineState(
      triangles_vertex_desc, std::size(triangles_vertex_desc),
      PipelineState::EDrawPass::Shadow, PipelineState::EFillMode::Solid,
      PipelineState::ETopology::Triangle, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::FrontFace);
  mShadowStateWire = mRenderer->CreatePipelineState(
      triangles_vertex_desc, std::size(triangles_vertex_desc),
      PipelineState::EDrawPass::Shadow, PipelineState::EFillMode::Wireframe,
      PipelineState::ETopology::Triangle, PipelineState::EDepthTest::On,
      PipelineState::EBlendMode::AlphaBlend,
      PipelineState::ECullMode::Backface);

  // Create instances buffer
  for (std::uint32_t n = 0; n < Renderer::cFrameCount; ++n)
    mInstancesBuffer[n] = mRenderer->CreateRenderInstances();

  // Create empty batch
  Vertex empty_vertex{JPH::Float3(0, 0, 0), JPH::Float3(1, 0, 0),
                      JPH::Float2(0, 0), JPH::Color::sWhite};
  std::uint32_t empty_indices[] = {0, 0, 0};
  mEmptyBatch = CreateTriangleBatch(&empty_vertex, 1, empty_indices, 3);

  // Initialize base class
  JPH::DebugRenderer::Initialize();
}

void DebugRendererImp::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo,
                                JPH::ColorArg inColor) {
  JPH::RVec3 offset = mRenderer->GetBaseOffset();

  Line line;
  JPH::Vec3(inFrom - offset).StoreFloat3(&line.mFrom);
  line.mFromColor = inColor;
  JPH::Vec3(inTo - offset).StoreFloat3(&line.mTo);
  line.mToColor = inColor;

  std::lock_guard lock(mLinesLock);
  mLines.push_back(line);
}

DebugRendererImp::Batch DebugRendererImp::CreateTriangleBatch(
    const Triangle *inTriangles, int inTriangleCount) {
  if (inTriangles == nullptr || inTriangleCount == 0) return mEmptyBatch;

  RenderPrimitive *primitive =
      mRenderer->CreateRenderPrimitive(PipelineState::ETopology::Triangle);
  primitive->CreateVertexBuffer(3 * inTriangleCount, sizeof(Vertex),
                                inTriangles);

  return primitive;
}

DebugRendererImp::Batch DebugRendererImp::CreateTriangleBatch(
    const Vertex *inVertices, int inVertexCount, const std::uint32_t *inIndices,
    int inIndexCount) {
  if (inVertices == nullptr || inVertexCount == 0 || inIndices == nullptr ||
      inIndexCount == 0)
    return mEmptyBatch;

  RenderPrimitive *primitive =
      mRenderer->CreateRenderPrimitive(PipelineState::ETopology::Triangle);
  primitive->CreateVertexBuffer(inVertexCount, sizeof(Vertex), inVertices);
  primitive->CreateIndexBuffer(inIndexCount, inIndices);

  return primitive;
}

void DebugRendererImp::DrawGeometry(
    JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds,
    float inLODScaleSq, JPH::ColorArg inModelColor,
    const GeometryRef &inGeometry, ECullMode inCullMode,
    ECastShadow inCastShadow, EDrawMode inDrawMode) {
  std::lock_guard lock(mPrimitivesLock);

  JPH::RVec3 offset = mRenderer->GetBaseOffset();

  JPH::Mat44 model_matrix = inModelMatrix.PostTranslated(-offset).ToMat44();
  JPH::AABox world_space_bounds = inWorldSpaceBounds;
  world_space_bounds.Translate(JPH::Vec3(-offset));

  // Our pixel shader uses alpha only to turn on/off shadows
  JPH::Color color = inCastShadow == ECastShadow::On
                         ? JPH::Color(inModelColor, 255)
                         : JPH::Color(inModelColor, 0);

  if (inDrawMode == EDrawMode::Wireframe) {
    mWireframePrimitives[inGeometry].mInstances.push_back(
        {model_matrix, model_matrix.GetDirectionPreservingMatrix(), color,
         world_space_bounds, inLODScaleSq});
    ++mNumInstances;
  } else {
    if (inCullMode != ECullMode::CullFrontFace) {
      mPrimitives[inGeometry].mInstances.push_back(
          {model_matrix, model_matrix.GetDirectionPreservingMatrix(), color,
           world_space_bounds, inLODScaleSq});
      ++mNumInstances;
    }

    if (inCullMode != ECullMode::CullBackFace) {
      mPrimitivesBackFacing[inGeometry].mInstances.push_back(
          {model_matrix, model_matrix.GetDirectionPreservingMatrix(), color,
           world_space_bounds, inLODScaleSq});
      ++mNumInstances;
    }
  }
}

void DebugRendererImp::FinalizePrimitive() {
  if (mLockedPrimitive != nullptr) {
    // Unlock the primitive
    mLockedPrimitive->UnlockVertexBuffer();

    // Set number of indices to draw
    mLockedPrimitive->SetNumVtxToDraw(
        int(mLockedVertices - mLockedVerticesStart));

    // Add to draw list
    mTempPrimitives[new Geometry(mLockedPrimitive.GetPtr(),
                                 mLockedPrimitiveBounds)]
        .mInstances.push_back({JPH::Mat44::sIdentity(), JPH::Mat44::sIdentity(),
                               JPH::Color::sWhite, mLockedPrimitiveBounds,
                               1.0f});
    ++mNumInstances;

    // Clear pointers
    mLockedPrimitive = nullptr;
    mLockedVerticesStart = nullptr;
    mLockedVertices = nullptr;
    mLockedVerticesEnd = nullptr;
    mLockedPrimitiveBounds = JPH::AABox();
  }
}

void DebugRendererImp::EnsurePrimitiveSpace(int inVtxSize) {
  const int cVertexBufferSize = 10240;

  if (mLockedPrimitive == nullptr ||
      mLockedVerticesEnd - mLockedVertices < inVtxSize) {
    FinalizePrimitive();

    // Create new
    mLockedPrimitive =
        mRenderer->CreateRenderPrimitive(PipelineState::ETopology::Triangle);
    mLockedPrimitive->CreateVertexBuffer(cVertexBufferSize, sizeof(Vertex));

    // Lock buffers
    mLockedVerticesStart = mLockedVertices =
        (Vertex *)mLockedPrimitive->LockVertexBuffer();
    mLockedVerticesEnd = mLockedVertices + cVertexBufferSize;
  }
}

void DebugRendererImp::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2,
                                    JPH::RVec3Arg inV3, JPH::ColorArg inColor,
                                    ECastShadow inCastShadow) {
  JPH::RVec3 offset = mRenderer->GetBaseOffset();

  JPH::Vec3 v1(inV1 - offset);
  JPH::Vec3 v2(inV2 - offset);
  JPH::Vec3 v3(inV3 - offset);

  std::lock_guard lock(mPrimitivesLock);

  EnsurePrimitiveSpace(3);

  // Set alpha to zero if we don't want to cast shadows to notify the pixel
  // shader
  JPH::Color color(inColor, inCastShadow == ECastShadow::Off ? 0 : 0xff);

  // Construct triangle in separate buffer and then copy it to the target memory
  // block (may be uncached memory)
  Triangle triangle(v1, v2, v3, color);
  *(Triangle *)mLockedVertices = triangle;
  mLockedVertices += 3;

  // Update bounding box
  mLockedPrimitiveBounds.Encapsulate(v1);
  mLockedPrimitiveBounds.Encapsulate(v2);
  mLockedPrimitiveBounds.Encapsulate(v3);
}

void DebugRendererImp::DrawInstances(const Geometry *inGeometry,
                                     const std::vector<int> &inStartIdx) {
  RenderInstances *instances_buffer =
      mInstancesBuffer[mRenderer->GetCurrentFrameIndex()];

  if (!inStartIdx.empty()) {
    // Get LODs
    const std::vector<LOD> &geometry_lods = inGeometry->mLODs;

    // Write instances for all LODS
    int next_start_idx = inStartIdx.front();
    for (size_t lod = 0; lod < geometry_lods.size(); ++lod) {
      int start_idx = next_start_idx;
      next_start_idx = inStartIdx[lod + 1];
      int num_instances = next_start_idx - start_idx;
      instances_buffer->Draw(static_cast<RenderPrimitive *>(
                                 geometry_lods[lod].mTriangleBatch.GetPtr()),
                             start_idx, num_instances);
    }
  }
}

void DebugRendererImp::DrawText3D(JPH::RVec3Arg inPosition,
                                  const std::string_view &inString,
                                  JPH::ColorArg inColor, float inHeight) {
  JPH::RVec3 offset = mRenderer->GetBaseOffset();

  JPH::Vec3 pos(inPosition - offset);

  std::lock_guard lock(mTextsLock);
  mTexts.emplace_back(pos, inString, inColor, inHeight);
}

void DebugRendererImp::DrawLines() {
  std::lock_guard lock(mLinesLock);

  // Draw the lines
  if (!mLines.empty()) {
    Ref<RenderPrimitive> primitive =
        mRenderer->CreateRenderPrimitive(PipelineState::ETopology::Line);
    primitive->CreateVertexBuffer((int)mLines.size() * 2, sizeof(Line) / 2);
    void *data = primitive->LockVertexBuffer();
    memcpy(data, &mLines[0], mLines.size() * sizeof(Line));
    primitive->UnlockVertexBuffer();
    mLineState->Activate();
    primitive->Draw();
  }
}

void DebugRendererImp::DrawShadowPass() {
  std::lock_guard lock(mPrimitivesLock);

  // Finish the last primitive
  FinalizePrimitive();

  // Get the camera and light frustum for culling
  JPH::Vec3 camera_pos(mRenderer->GetCameraState().mPos -
                       mRenderer->GetBaseOffset());
  const Frustum &camera_frustum = mRenderer->GetCameraFrustum();
  const Frustum &light_frustum = mRenderer->GetLightFrustum();

  // Resize instances buffer and copy all visible instance data into it
  if (mNumInstances > 0) {
    // Create instances buffer
    RenderInstances *instances_buffer =
        mInstancesBuffer[mRenderer->GetCurrentFrameIndex()];
    instances_buffer->CreateBuffer(2 * mNumInstances, sizeof(Instance));
    Instance *dst_instance =
        reinterpret_cast<Instance *>(instances_buffer->Lock());

    // Next write index
    int dst_index = 0;

    // This keeps track of which instances use which lod, first array: 0 = light
    // pass, 1 = geometry pass
    std::vector<std::vector<int>> lod_indices[2];

    for (InstanceMap *primitive_map :
         {&mPrimitives, &mTempPrimitives, &mPrimitivesBackFacing,
          &mWireframePrimitives})
      for (InstanceMap::value_type &v : *primitive_map) {
        // Get LODs
        const std::vector<LOD> &geometry_lods = v.first->mLODs;
        size_t num_lods = geometry_lods.size();
        JPH_ASSERT(num_lods > 0);

        // Ensure that our lod index array is big enough (to avoid reallocating
        // memory too often)
        if (lod_indices[0].size() < num_lods) lod_indices[0].resize(num_lods);
        if (lod_indices[1].size() < num_lods) lod_indices[1].resize(num_lods);

        // Iterate over all instances
        const std::vector<InstanceWithLODInfo> &instances = v.second.mInstances;
        for (size_t i = 0; i < instances.size(); ++i) {
          const InstanceWithLODInfo &src_instance = instances[i];

          // Check if it overlaps with the light or camera frustum
          bool light_overlaps =
              light_frustum.Overlaps(src_instance.mWorldSpaceBounds);
          bool camera_overlaps =
              camera_frustum.Overlaps(src_instance.mWorldSpaceBounds);
          if (light_overlaps || camera_overlaps) {
            // Figure out which LOD to use
            const LOD &lod =
                v.first->GetLOD(camera_pos, src_instance.mWorldSpaceBounds,
                                src_instance.mLODScaleSq);
            size_t lod_index = &lod - geometry_lods.data();

            // Store which index goes in which LOD
            if (light_overlaps) lod_indices[0][lod_index].push_back((int)i);
            if (camera_overlaps) lod_indices[1][lod_index].push_back((int)i);
          }
        }

        // Loop over both passes: 0 = light, 1 = geometry
        std::vector<int> *start_idx[] = {&v.second.mLightStartIdx,
                                         &v.second.mGeometryStartIdx};
        for (int type = 0; type < 2; ++type) {
          // Reserve space for instance indices
          std::vector<int> &type_start_idx = *start_idx[type];
          type_start_idx.resize(num_lods + 1);

          // Write out geometry pass instances
          for (size_t lod = 0; lod < num_lods; ++lod) {
            // Write start index for this LOD
            type_start_idx[lod] = dst_index;

            // Copy instances
            std::vector<int> &this_lod_indices = lod_indices[type][lod];
            for (int i : this_lod_indices) {
              const Instance &src_instance = instances[i];
              dst_instance[dst_index++] = src_instance;
            }

            // Prepare for next iteration (will preserve memory)
            this_lod_indices.clear();
          }

          // Write out end of last LOD
          type_start_idx.back() = dst_index;
        }
      }

    instances_buffer->Unlock();
  }

  if (!mPrimitives.empty() || !mTempPrimitives.empty()) {
    // Front face culling, we want to render the back side of the geometry for
    // casting shadows
    mShadowStateFF->Activate();

    // Draw all primitives as seen from the light
    if (mNumInstances > 0)
      for (InstanceMap::value_type &v : mPrimitives)
        DrawInstances(v.first, v.second.mLightStartIdx);
    for (InstanceMap::value_type &v : mTempPrimitives)
      DrawInstances(v.first, v.second.mLightStartIdx);
  }

  if (!mPrimitivesBackFacing.empty()) {
    // Back face culling, we want to render the front side of back facing
    // geometry
    mShadowStateBF->Activate();

    // Draw all primitives as seen from the light
    for (InstanceMap::value_type &v : mPrimitivesBackFacing)
      DrawInstances(v.first, v.second.mLightStartIdx);
  }

  if (!mWireframePrimitives.empty()) {
    // Switch to wireframe mode
    mShadowStateWire->Activate();

    // Draw all wireframe primitives as seen from the light
    for (InstanceMap::value_type &v : mWireframePrimitives)
      DrawInstances(v.first, v.second.mLightStartIdx);
  }
}

void DebugRendererImp::DrawTriangles() {
  // Bind the shadow map texture
  mRenderer->GetShadowMap()->Bind();

  if (!mPrimitives.empty() || !mTempPrimitives.empty()) {
    // Bind the normal shader, back face culling
    mTriangleStateBF->Activate();

    // Draw all primitives
    if (mNumInstances > 0)
      for (InstanceMap::value_type &v : mPrimitives)
        DrawInstances(v.first, v.second.mGeometryStartIdx);
    for (InstanceMap::value_type &v : mTempPrimitives)
      DrawInstances(v.first, v.second.mGeometryStartIdx);
  }

  if (!mPrimitivesBackFacing.empty()) {
    // Front face culling, the next batch needs to render inside out
    mTriangleStateFF->Activate();

    // Draw all back primitives
    for (InstanceMap::value_type &v : mPrimitivesBackFacing)
      DrawInstances(v.first, v.second.mGeometryStartIdx);
  }

  if (!mWireframePrimitives.empty()) {
    // Wire frame mode
    mTriangleStateWire->Activate();

    // Draw all wireframe primitives
    for (InstanceMap::value_type &v : mWireframePrimitives)
      DrawInstances(v.first, v.second.mGeometryStartIdx);
  }
}

void DebugRendererImp::DrawTexts() {
  std::lock_guard lock(mTextsLock);
  const CameraState &camera_state = mRenderer->GetCameraState();

  for (const Text &t : mTexts) {
    JPH::Vec3 forward = camera_state.mForward;
    JPH::Vec3 right = forward.Cross(camera_state.mUp).Normalized();
    JPH::Vec3 up = right.Cross(forward).Normalized();
    JPH::Mat44 transform(JPH::Vec4(right, 0), JPH::Vec4(up, 0),
                         JPH::Vec4(forward, 0), JPH::Vec4(t.mPosition, 1));

    mFont->DrawText3D(transform * JPH::Mat44::sScale(t.mHeight), t.mText,
                      t.mColor);
  }
}

void DebugRendererImp::Draw() {
  DrawLines();
  DrawTriangles();
  DrawTexts();
}

void DebugRendererImp::ClearLines() {
  std::lock_guard lock(mLinesLock);
  mLines.clear();
}

void DebugRendererImp::ClearMap(InstanceMap &ioInstances) {
  std::vector<GeometryRef> to_delete;

  for (InstanceMap::value_type &kv : ioInstances) {
    if (kv.second.mInstances.empty())
      to_delete.push_back(kv.first);
    else
      kv.second.mInstances.clear();
  }

  for (GeometryRef &b : to_delete) ioInstances.erase(b);
}

void DebugRendererImp::ClearTriangles() {
  std::lock_guard lock(mPrimitivesLock);

  // Close any primitive that's being built
  FinalizePrimitive();

  // Move primitives to draw back to the free list
  ClearMap(mWireframePrimitives);
  ClearMap(mPrimitives);
  mTempPrimitives.clear();  // These are created by FinalizePrimitive() and need
                            // to be cleared every frame
  ClearMap(mPrimitivesBackFacing);
  mNumInstances = 0;
}

void DebugRendererImp::ClearTexts() {
  std::lock_guard lock(mTextsLock);
  mTexts.clear();
}

void DebugRendererImp::Clear() {
  ClearLines();
  ClearTriangles();
  ClearTexts();
  JPH::DebugRenderer::NextFrame();
}
