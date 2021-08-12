#include "Common/Math3D.h"
#include "GUI/MaterialLibraryWidget.h"
#include "GUI/imgui.h"
#include "Material/MaterialLibrary.h"
#include "Material/Material.h"
#include "Material/ShaderProgram.h"

#include <functional>

inline bool numericClose(float a, float b)
{
	return fabs(a - b) < 1e-6;
}

inline bool numericClose(const glm::vec4& a, const glm::vec4& b)
{
	return numericClose(a.x, b.x)
		&& numericClose(a.y, b.y)
		&& numericClose(a.z, b.z);
}

template<typename T>
UniformValueCache<T> createFromUniform(const UniformValue& value, const UniformMetaInfo& uniform)
{
	UniformValueCache<T> uniformCache;
	uniformCache.name = uniform.name;
	uniformCache.oldValue = value.as<T>();
	uniformCache.newValue = uniformCache.oldValue;
	uniformCache.min = uniform.minValue.as<T>();
	uniformCache.max = uniform.maxValue.as<T>();
	return uniformCache;
}

template<typename T>
void insertUniformValue(UniformGroupCache<T>& uniformCache, const UniformValue& value, const UniformMetaInfo& metaInfo)
{
	UniformValue uniformValue = value;

	if (uniformValue.type == UniformType::Invalid)
	{
		uniformValue = metaInfo.defaultValue;
	}
	else if (uniformValue.type != metaInfo.type)
	{
		// should not happen!
		return;
	}

	switch (metaInfo.type)
	{
	case UniformType::Float:
		uniformCache.floatUniforms.emplace_back(createFromUniform<float>(uniformValue, metaInfo));
		uniformCache.floatUniforms.back().useRange = metaInfo.minValue.as<float>() >= -1000.f
			&& metaInfo.maxValue.as<float>() <= 1000.f;
		break;
	case UniformType::Int:
		uniformCache.intUniforms.emplace_back(createFromUniform<signed int>(uniformValue, metaInfo));
		uniformCache.intUniforms.back().useRange = metaInfo.minValue.as<signed int>() >= -1000
			&& metaInfo.maxValue.as<signed int>() <= 1000;
		break;
	case UniformType::Vec4:
	{
		const glm::vec4 min = metaInfo.minValue.as<glm::vec4>();
		const glm::vec4 max = metaInfo.maxValue.as<glm::vec4>();

		uniformCache.vec4Uniforms.emplace_back(createFromUniform<glm::vec4>(uniformValue, metaInfo));
		uniformCache.vec4Uniforms.back().useRange = min.x >= -1000.f && min.y >= -1000.f && min.z >= -1000.f
			&& max.x <= 1000.f && max.y <= 1000.f && max.z <= 1000.f;
		break;
	}
	}
}

template<typename T>
void updatUniformValue(UniformGroupCache<T>& uniformCache, std::function<bool(const std::string&, const UniformValue&)> updateFunc)
{
	for (auto& uniform : uniformCache.floatUniforms)
	{
		if (!numericClose(uniform.newValue, uniform.oldValue) &&
			updateFunc(uniform.name, uniform.newValue))
		{
			uniform.oldValue = uniform.newValue;
		}
	}
	for (auto& uniform : uniformCache.intUniforms)
	{
		if (uniform.newValue != uniform.oldValue &&
			updateFunc(uniform.name, uniform.newValue))
		{
			uniform.oldValue = uniform.newValue;
		}
	}
	for (auto& uniform : uniformCache.vec4Uniforms)
	{
		if (!numericClose(uniform.newValue, uniform.oldValue) &&
			updateFunc(uniform.name, uniform.newValue))
		{
			uniform.oldValue = uniform.newValue;
		}
	}
}

template<typename T>
void drawUniformGUI(UniformGroupCache<T>& uniformCache, bool showHidden)
{
	for (auto& uniform : uniformCache.floatUniforms)
	{
		if (!showHidden && uniform.name[0] == '_') continue;

		if (uniform.useRange)
		{
			ImGui::SliderScalar(uniform.name.c_str(), ImGuiDataType_Float, &uniform.newValue, &uniform.min, &uniform.max, "%.3f");
		}
		else
		{
			ImGui::InputScalar(uniform.name.c_str(), ImGuiDataType_Float, &uniform.newValue);
		}
	}
	for (auto& uniform : uniformCache.intUniforms)
	{
		if (!showHidden && uniform.name[0] == '_') continue;

		if (uniform.useRange)
		{
			ImGui::SliderScalar(uniform.name.c_str(), ImGuiDataType_S32, &uniform.newValue, &uniform.min, &uniform.max, "%.3f");
		}
		else
		{
			ImGui::InputScalar(uniform.name.c_str(), ImGuiDataType_S32, &uniform.newValue);
		}
	}
	for (auto& uniform : uniformCache.vec4Uniforms)
	{
		if (!showHidden && uniform.name[0] == '_') continue;

		if (uniform.useRange)
		{
			ImGui::SliderScalarN(uniform.name.c_str(),
				ImGuiDataType_Float, glm::value_ptr(uniform.newValue), 4,
				glm::value_ptr(uniform.min), glm::value_ptr(uniform.max), "%.3f");
		}
		else
		{
			ImGui::InputScalarN(uniform.name.c_str(),
				ImGuiDataType_Float, glm::value_ptr(uniform.newValue), 4);
		}
	}
}

MaterialLibraryWidget::MaterialLibraryWidget(const std::string& title, MaterialLibrarySPtr matLib)
	: m_title(title)
	, m_matLib(matLib)
	, m_visible(true)
{
}

void MaterialLibraryWidget::update(double /*deltaTime*/)
{
	if (m_matLib->programs().size() != m_programCache.size())
	{
		m_programCache.clear();

		for (auto& [programName, program] : m_matLib->programs())
		{
			ProgramCache progCache;
			progCache.ptr = program;

			for (auto& [uniformName, metaInfo] : program->uniformMetaInfo())
			{
				UniformValue uniformValue = program->uniformDefaultValue(uniformName);

				insertUniformValue(progCache, uniformValue, metaInfo);
			}
			m_programCache.emplace_back(std::move(progCache));
		}
	}
	else
	{
		for (auto& program : m_programCache)
		{
			updatUniformValue(program, 
				[&](const std::string& name, const UniformValue& value) -> bool {
					return program.ptr->setUniformDefault(name, value);
				}
			);
		}
	}

	if (m_matLib->materials().size() != m_materialCache.size())
	{
		m_materialCache.clear();

		for (auto& [materialName, material] : m_matLib->materials())
		{
			MaterialCache matCache;
			matCache.ptr = material;

			for (auto& [uniformName, metaInfo] : material->program()->uniformMetaInfo())
			{
				UniformValue uniformValue = material->uniformValue(uniformName);

				insertUniformValue(matCache, uniformValue, metaInfo);
			}
			m_materialCache.emplace_back(std::move(matCache));
		}
	}
	else
	{
		for (auto& material : m_materialCache)
		{
			updatUniformValue(material,
				[&](const std::string& name, const UniformValue& value) -> bool {
					return material.ptr->setUniform(name, value);
				}
			);
		}
	}
}

void MaterialLibraryWidget::draw()
{
	if (!ImGui::Begin(m_title.c_str(), &m_visible))
	{
		ImGui::End();
		return;
	}

	//ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::Checkbox("Show hidden unfiorms", &m_showHiddenUniforms);

	if (ImGui::TreeNode("Programs:"))
	{
		for (auto& program : m_programCache)
		{
			if (ImGui::TreeNode(program.ptr->name().c_str()))
			{
				drawUniformGUI(program, m_showHiddenUniforms);

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Instances:"))
	{
		for (auto& material : m_materialCache)
		{
			if (ImGui::TreeNode(material.ptr->name().c_str()))
			{
				ImGui::Text(material.ptr->program()->name().c_str());

				drawUniformGUI(material, m_showHiddenUniforms);

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	//for (auto& [materialName, material] : m_matLib->materials())
	//{
	//	if (ImGui::TreeNode(materialName.c_str()))
	//	{
	//		ImGui::Text(material->program()->name().c_str());

	//		for (auto& [uniformName, uniformType] : material->program()->getUniformTypes())
	//		{
	//			UniformValue uniformValue = material->getUniformValue(uniformName);

	//			if (uniformValue.type != uniformType)
	//			{
	//				// should not happen!
	//				continue;
	//			}

	//			switch (uniformType)
	//			{
	//			case UniformType::Int:
	//				ImGui::SliderScalar(uniformName.c_str(),
	//					ImGuiDataType_S32, &scalarI, &minI, &maxI);
	//				break;
	//			case UniformType::Float:
	//			{
	//				float floatValue = std::any_cast<float>(uniformValue.value);
	//				float floatValueTmp = floatValue;

	//				ImGui::SliderScalar(uniformName.c_str(),
	//					ImGuiDataType_Float, &floatValueTmp, &minF, &maxF);

	//				// this does not work
	//				// try to copy the values on update to a separate struct (which is rendered here)
	//				// and then on update set the values back to the program if the value changed!
	//				if (floatValue != floatValueTmp)
	//				{
	//					//DO something
	//				}
	//				break;
	//			}
	//			case UniformType::Vec4:
	//				ImGui::SliderScalarN(uniformName.c_str(),
	//					ImGuiDataType_Float, &vector, 4, &vMin, &vMax);
	//				break;
	//			}
	//		}

	//		//ImGui::SameLine();
	//		if (ImGui::SmallButton("Reload")) 
	//		{
	//		}

	//		ImGui::TreePop();
	//	}
	//}

	//ImGui::EndChild();
	ImGui::End();
}
