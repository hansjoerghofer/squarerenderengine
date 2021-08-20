#pragma once

#include "Common/Macros.h"
#include "Common/Logger.h"
#include "Application/GL.h"
#include "Renderer/RendererState.h"
#include "Scene/IGeometryVisitor.h"

#include <vector>

DECLARE_PTRS(Camera);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(Material);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(Renderer);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(IRenderTarget);

class Renderer
{
public:

	Renderer();

	void render(IGeometrySPtr geo, MaterialSPtr mat);

	void setTarget(IRenderTargetSPtr target);

	void applyState(const RendererState& state, bool force = false);

	void regenerateMipmaps(ITextureSPtr tex);

private:

	void bindTextures(MaterialSPtr mat);

	void unbindTextures();

	IGeometryVisitorUPtr m_geometryPainter;

	IRenderTargetSPtr m_currentRenderTarget;

	std::vector<ITextureSPtr> m_boundTextures;

	RendererState m_currentState;
};

