#pragma once

#include "Common/Macros.h"
#include "Renderer/RendererState.h"
#include "Renderer/IDrawable.h"
#include "Renderer/IRenderTarget.h"

#include <string>
#include <vector>

DECLARE_PTRS(Camera);
DECLARE_PTRS(Material);
DECLARE_PTRS(Geometry);
DECLARE_PTRS(RenderTarget);

struct RenderCommand
{
    bool enabled = true;

    std::string name;

	RendererState state;

    std::vector<IDrawableSPtr> drawables;

    IRenderTargetSPtr target = nullptr;

    MaterialSPtr overrideMaterial = nullptr;
};