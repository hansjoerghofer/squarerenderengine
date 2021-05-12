#include "Scene/SceneElement.h"
#include "Scene/Geometry.h"
#include "Material/Material.h"

SceneElement::SceneElement(const std::string& name)
    : m_name(name)
    , m_transform(1.0f)
{
}

SceneElement::SceneElement(std::string&& name)
    : m_name(name)
    , m_transform(1.0f)
{
}

SceneElement::~SceneElement()
{
}

std::string SceneElement::name() const
{
    return m_name;
}

const glm::mat4x4& SceneElement::transform() const
{
    return m_transform;
}

GeometrySPtr SceneElement::geometry() const
{
    return m_geometry;
}

MaterialSPtr SceneElement::material() const
{
    return m_material;
}

void SceneElement::setTransform(const glm::mat4x4& transform)
{
    m_transform = transform;
}

void SceneElement::setGeometry(GeometrySPtr geometry)
{
    m_geometry = geometry;
}

void SceneElement::setMaterial(MaterialSPtr material)
{
    m_material = material;
}
