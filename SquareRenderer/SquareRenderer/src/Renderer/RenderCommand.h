#pragma once

#include "Common/Macros.h"
#include "Renderer/RendererState.h"

#include <string>
#include <vector>

DECLARE_PTRS(Material);
DECLARE_PTRS(IDrawable);
DECLARE_PTRS(IRenderTarget);

struct RenderCommand
{
    bool enabled = true;

    std::string name;

	RendererState state;

    std::vector<IDrawableSPtr> drawables;

    IRenderTargetSPtr target = nullptr;

    MaterialSPtr overrideMaterial = nullptr;
};