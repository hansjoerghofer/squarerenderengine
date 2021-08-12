#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"
#include "GUI/IWidget.h"

#include <string>
#include <vector>

DECLARE_PTRS(MaterialLibrary);
DECLARE_PTRS(Material);
DECLARE_PTRS(ShaderProgram);

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

template<typename T>
struct UniformGroupCache
{
	std::string name;
	std::string programName;
	std::vector<UniformValueCache<float>> floatUniforms;
	std::vector<UniformValueCache<signed int>> intUniforms;
	std::vector<UniformValueCache<glm::vec4>> vec4Uniforms;

	std::shared_ptr<T> ptr;
};

typedef UniformGroupCache<Material> MaterialCache;
typedef UniformGroupCache<ShaderProgram> ProgramCache;

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

	bool m_showHiddenUniforms = false;

	std::vector<ProgramCache> m_programCache;
	std::vector<MaterialCache> m_materialCache;
};
