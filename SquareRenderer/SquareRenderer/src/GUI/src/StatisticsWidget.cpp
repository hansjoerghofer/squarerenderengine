#include "GUI/StatisticsWidget.h"
#include "Application/GLWindow.h"
#include "GUI/imgui.h"
#include "Renderer/IRenderPass.h"
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
	memset(m_frametimeBuffer, 0, FRAMERATE_BUFFER_SIZE);
}

double computeEWMA(double accumulator, double sample)
{
	constexpr double alpha = 0.05;
	constexpr double iAlpha = 1.0 - alpha;

	return (alpha * sample) + iAlpha * accumulator;
}

void StatisticsWidget::update(double deltaTime)
{
	m_frameTimeAcc = computeEWMA(m_frameTimeAcc, deltaTime);

	m_frametimeIndex = (m_frametimeIndex + 1) % FRAMERATE_BUFFER_SIZE;

	m_frametimeBuffer[m_frametimeIndex] = static_cast<float>(1.0f / deltaTime);
}

void StatisticsWidget::draw()
{
	if (!visible()) return;

	if (!ImGui::Begin(m_title.c_str(), &m_visible))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Resolution: %dx%d", m_engine->renderTarget()->width(), m_engine->renderTarget()->height());

	ImGui::Text("Frametime: %.2fms", m_frameTimeAcc * 1e3);

	ImGui::Text("FPS: %d", static_cast<int>(1.0 / m_frameTimeAcc));

	ImGui::PlotLines("", m_frametimeBuffer, IM_ARRAYSIZE(m_frametimeBuffer), m_frametimeIndex, 0, 0.0f, 90.0f, ImVec2(0, 90.0f));

	for (IRenderPassSPtr pass : m_engine->renderPasses())
	{
		ImGui::Separator();

		double gpuTime = 0;
		//double cpuTime = 0;
		//int primitiveCount = 0;

		if (pass->isEnabled())
		{
			RenderStatisticsData stats = pass->renderStatistics();
			gpuTime = stats.gpuTimeMs;
			//cpuTime = stats.cpuTimeMs;
			//primitiveCount = static_cast<int>(stats.rendererdPrimitives);
		}

		ImGui::Text("%s: %.2fms", pass->name().c_str(), gpuTime);

		//ImGui::Text(pass->name().c_str());
		//ImGui::Text("GPU time: %.2fms", gpuTime);
		//ImGui::Text("CPU time: %.2fms", cpuTime);
		//ImGui::Text("Primitives: %i", primitiveCount);
	}

	ImGui::End();
}

const std::string& StatisticsWidget::name() const
{
	return m_title;
}

bool StatisticsWidget::visible() const
{
	return m_visible;
}

void StatisticsWidget::setVisible(bool flag)
{
	m_visible = flag;
}
