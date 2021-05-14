#pragma once

#include "Common/Macros.h"
#include "Common/Logger.h"
#include "Application/GL.h"
#include "Renderer/RendererState.h"

DECLARE_PTRS(Camera);
DECLARE_PTRS(Geometry);
DECLARE_PTRS(Material);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(Renderer);

class Renderer
{
public:

	Renderer();

	void render(Geometry& geo, Material& mat);

	void setupView(const Camera& cam);

	void applyState(const RendererState& state, bool force = false);

private:

	RendererState m_currentState;
};

