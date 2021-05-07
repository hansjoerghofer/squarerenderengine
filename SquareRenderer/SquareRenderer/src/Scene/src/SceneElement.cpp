#include "Scene/SceneElement.h"
#include "Scene/Geometry.h"
#include "Material/Material.h"

SceneElement::SceneElement(const std::string& name)
    : m_name(name)
{
}

SceneElement::SceneElement(std::string&& name)
    : m_name(name)
{
}

SceneElement::~SceneElement()
{
}

std::string SceneElement::name() const
{
    return m_name;
}

GeometrySPtr SceneElement::geometry() const
{
    return m_geometry;
}

MaterialSPtr SceneElement::material() const
{
    return m_material;
}

void SceneElement::setGeometry(GeometrySPtr geometry)
{
    m_geometry = geometry;
}

void SceneElement::setMaterial(MaterialSPtr material)
{
    m_material = material;
}
