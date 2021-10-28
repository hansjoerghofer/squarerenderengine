#pragma once

#include "Common/Macros.h"
#include "Renderer/RendererState.h"
#include "Texture/TextureDefines.h"
#include "Scene/IGeometryVisitor.h"

#include <vector>

DECLARE_PTRS(Camera);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(Material);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(Renderer);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(IRenderTarget);
DECLARE_PTRS(RenderVisitor);

class RenderVisitor : public IGeometryVisitor
{
public:
    virtual ~RenderVisitor() {}

	void prepare(const Material& mat);

	virtual void visit(Mesh& mesh) override;

	virtual void visit(PrimitiveSet& primitiveSet) override;

private:

    bool m_tesselate = false;
};

class Renderer
{
public:

	Renderer();

	void render(IGeometrySPtr geo, MaterialSPtr mat);

	void blit(IRenderTargetSPtr source, IRenderTargetSPtr target, 
		TextureFilter filter = TextureFilter::Linear,
		bool color = true, 
		bool depth = false, 
		bool stencil = false);

	void setTarget(IRenderTargetSPtr target);

	void applyState(const RendererState& state, bool force = false);

	void regenerateMipmaps(ITextureSPtr tex);

private:

	void bindTextures(MaterialSPtr mat);

	void unbindTextures();

	RenderVisitorUPtr m_geometryPainter;

	IRenderTargetSPtr m_currentRenderTarget;

	std::vector<ITextureSPtr> m_boundTextures;

	RendererState m_currentState;
};

