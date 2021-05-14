#pragma once

#include "Common/Macros.h"
#include "Renderer/RendererState.h"

#include <string>
#include <vector>

DECLARE_PTRS(Camera);
DECLARE_PTRS(Material);
DECLARE_PTRS(Geometry);
DECLARE_PTRS(SceneElement);

struct RenderElement
{
    GeometrySPtr geometry;
    MaterialSPtr material;

    RenderElement(GeometrySPtr geo, MaterialSPtr mat)
        : geometry(geo), material(mat) {}
};

struct RenderCommand
{
    bool enabled = true;

    std::string name;

	RendererState state;

    CameraSPtr camera = nullptr;

    std::vector<RenderElement> elements;

    MaterialSPtr overrideMaterial = nullptr;
};