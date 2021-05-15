#include "Scene/SceneNode.h"
#include "Scene/Geometry.h"
#include "Material/Material.h"

SceneNode::SceneNode(const std::string& name)
    : m_name(name)
    , m_transform(1.f)
{
}

const std::string& SceneNode::name() const
{
    return m_name;
}

void SceneNode::setParent(SceneNodeSPtr node)
{
    m_parent = node;
}

SceneNodeWPtr SceneNode::parent() const
{
    return m_parent;
}

void SceneNode::addChild(SceneNodeSPtr node)
{
    m_children.push_back(node);
}

const std::list<SceneNodeSPtr>& SceneNode::children() const
{
    return m_children;
}

void SceneNode::setLocalTransform(const glm::mat4& transform)
{
    m_transform = transform;
}

const glm::mat4& SceneNode::localTransform() const
{
    return m_transform;
}

glm::mat4 SceneNode::worldTransform() const
{
    if (!m_parent.expired())
    {
        return m_parent.lock()->worldTransform() * m_transform;
    }
    else
    {
        return m_transform;
    }
}

void SceneNode::setMaterial(MaterialSPtr material)
{
    m_material = material;
}

MaterialSPtr SceneNode::material() const
{
    return m_material;
}

void SceneNode::setGeometry(GeometrySPtr geometry)
{
    m_geometry = geometry;
}

GeometrySPtr SceneNode::geometry() const
{
    return m_geometry;
}

bool SceneNode::isDrawable() const
{
    return m_geometry && m_material;
}
