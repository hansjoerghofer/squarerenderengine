#include "Scene/SceneNode.h"
#include "Scene/IGeometry.h"
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

unsigned int SceneNode::count() const
{
    unsigned int count = 1;
    for (SceneNodeSPtr child : m_children)
    {
        count += child->count();
    }
    return count;
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
    // TODO cache result!
    if (!m_parent.expired())
    {
        return m_parent.lock()->worldTransform() * localTransform();
    }
    else
    {
        return localTransform();
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

void SceneNode::setGeometry(IGeometrySPtr geometry)
{
    m_geometry = geometry;
}

IGeometrySPtr SceneNode::geometry() const
{
    return m_geometry;
}

void SceneNode::preRender(MaterialSPtr material)
{
    glm::mat4 modelToWorld = worldTransform();

    material->setUniform("modelToWorld", modelToWorld);
    material->setUniform("normalToWorld", glm::transpose(glm::inverse(modelToWorld)));
}

void SceneNode::postRender()
{

}

void SceneNode::setBounds(const BoundingBox& bounds)
{
    m_bounds = bounds;
}

const BoundingBox& SceneNode::bounds() const
{
    return m_bounds;
}

BoundingBox SceneNode::hierarchicalBounds() const
{
    BoundingBox aabb = bounds();

    for (SceneNodeSPtr child : m_children)
    {
        aabb.merge(child->localTransform() * child->hierarchicalBounds());
    }

    return aabb;
}
