#include "GUI/StatisticsWidget.h"
#include "GUI/imgui.h"

#include "Application/GLWindow.h"
#include "Renderer/RenderEngine.h"

#include <sstream>
#include <string>

StatisticsWidget::StatisticsWidget(const std::string& title, 
	GLWindowSPtr window,
	RenderEngineSPtr engine)
	: m_title(title)
	, m_window(window)
	, m_engine(engine)
	, m_visible(true)
{
}

double computeEWMA(double accumulator, double sample)
{
	constexpr double alpha = 0.01;
	constexpr double iAlpha = 1.0 - alpha;

	return (alpha * sample) + iAlpha * accumulator;
}

void StatisticsWidget::update(double deltaTime)
{
	m_frameTimeAcc = computeEWMA(m_frameTimeAcc, deltaTime);
}

void StatisticsWidget::draw()
{
	if (!ImGui::Begin(m_title.c_str(), &m_visible))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Resolution: %dx%d", m_engine->renderTarget()->width(), m_engine->renderTarget()->height());

	ImGui::Text("Frametime: %.3fs", m_frameTimeAcc);

	ImGui::Text("FPS: %d", static_cast<int>(1.0 / m_frameTimeAcc));

	ImGui::End();
}
