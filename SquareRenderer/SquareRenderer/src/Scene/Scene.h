#pragma once

#include "Common/Macros.h"

#include <vector>
#include <stack>

DECLARE_PTRS(SceneNode);
DECLARE_PTRS(Scene);
DECLARE_PTRS(ILightsource);
class BoundingBox;

class Scene
{
public:
    class Traverser;

	Scene(SceneNodeSPtr root);

	SceneNodeSPtr root() const;

    BoundingBox sceneBounds() const;

    const std::vector<ILightsourceSPtr>& lights() const;

    void addLight(ILightsourceSPtr light);

    unsigned int nodeNum() const;

    Traverser traverser() const;

    class Traverser
    {
    public:
        Traverser(SceneNodeSPtr node);

        bool hasNext() const;

        SceneNodeSPtr next();

    private:
        SceneNodeSPtr m_node;

        std::stack<SceneNodeSPtr> m_stack;
    };

protected:

	SceneNodeSPtr m_root;

    std::vector<ILightsourceSPtr> m_lights;
};

