#include "GUI/LightingWidget.h"
#include "Common/MathUtils.h"
#include "GUI/imgui.h"
#include "Scene/Scene.h"
#include "Scene/ILightsource.h"
#include "Scene/DirectionalLight.h"

constexpr glm::vec2 yawPitch = glm::vec2(360, 180);
constexpr glm::vec2 invYawPitch = glm::vec2(1/ yawPitch.x, 1 / yawPitch.y);

LightingWidget::LightingWidget(const std::string& title, SceneSPtr scene)
	: m_title(title)
	, m_scene(scene)
{
}

LightingWidget::~LightingWidget()
{
}

const std::string& LightingWidget::name() const
{
	return m_title;
}

bool LightingWidget::visible() const
{
	return m_visible;
}

void LightingWidget::setVisible(bool flag)
{
	m_visible = flag;
}

void LightingWidget::update(double /*deltaTime*/)
{
	if (m_values.empty())
	{
		for (const ILightsourceSPtr light : m_scene->lights())
		{
			if (light->type() == LightsourceType::Directional)
			{
				DirectionalLightSPtr dirLight = std::static_pointer_cast<DirectionalLight>(light);

				const glm::vec2 uv = MathUtils::dir2LonLat(dirLight->direction());

				m_values[light] = { false, glm::vec3(uv * yawPitch, 0), dirLight->color() };
			}
		}
	}

	for (auto& [light, values] : m_values)
	{
		if (light->type() == LightsourceType::Directional)
		{
			DirectionalLightSPtr dirLight = std::static_pointer_cast<DirectionalLight>(light);

			if (values.dirty)
			{
				glm::vec2 pitchYaw = glm::vec2(values.vec) * invYawPitch;

				const glm::vec3 dir = MathUtils::lonLat2Dir(pitchYaw);

				dirLight->setDirection(dir);
				values.dirty = false;
			}
		}
	}
}

void LightingWidget::draw()
{
	if (!visible()) return;

	if (!ImGui::Begin(m_title.c_str(), &m_visible))
	{
		ImGui::End();
		return;
	}

	int numDirLights = 0;
	for (auto& [light, values] : m_values)
	{
		if (light->type() == LightsourceType::Directional)
		{
			++numDirLights;

			const std::string name = "Directional Light #" + std::to_string(numDirLights);
			if (ImGui::TreeNode(name.c_str()))
			{
				const float minYaw = 0;
				const float maxYaw = 360;

				const float minPitch = 0;
				const float maxPitch = 180;

				values.dirty |= ImGui::SliderScalar("Yaw", ImGuiDataType_Float, &values.vec[0], &minYaw, &maxYaw, "%.1f");
				values.dirty |= ImGui::SliderScalar("Pitch", ImGuiDataType_Float, &values.vec[1], &minPitch, &maxPitch, "%.1f");

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}
