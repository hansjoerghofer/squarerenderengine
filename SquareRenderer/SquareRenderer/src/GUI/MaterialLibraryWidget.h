#pragma once

#include "Common/Macros.h"
#include "GUI/IWidget.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

DECLARE_PTRS(MaterialLibrary)
DECLARE_PTRS(Material)

template<typename T>
struct UniformValueCache
{
	std::string name;
	T oldValue = T(0);
	T newValue = T(0);
	T min = T(0);
	T max = T(0);

	bool useRange = true;
};

struct MaterialCache
{
	std::string name;
	std::string programName;
	std::vector<UniformValueCache<float>> floatUniforms;
	std::vector<UniformValueCache<signed int>> intUniforms;
	std::vector<UniformValueCache<glm::vec4>> vec4Uniforms;

	MaterialSPtr ptr;
};

class MaterialLibraryWidget : public IWidget
{
public:

	MaterialLibraryWidget(const std::string& title, MaterialLibrarySPtr matLib);

	void update(double deltaTime) override;

	void draw() override;

private:

	MaterialLibrarySPtr m_matLib;

	std::string m_title;

	bool m_visible;

	std::vector<MaterialCache> m_materialCache;
};
