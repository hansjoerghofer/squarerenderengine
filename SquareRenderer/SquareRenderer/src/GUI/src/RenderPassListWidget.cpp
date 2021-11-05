#include "GUI/RenderPassListWidget.h"
#include "Common/MathUtils.h"
#include "GUI/imgui.h"
#include "Renderer/BloomRenderPass.h"
#include "Renderer/DepthBuffer.h"
#include "Renderer/GeometryRenderPass.h"
#include "Renderer/RenderEngine.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/ShadowMappingRenderPass.h"
#include "Texture/Texture2D.h"

class DefaultPassWidget : public IRenderPassWidget
{
public:
	DefaultPassWidget(IRenderPassSPtr pass)
		: m_renderPass(pass)
		, m_enabled(pass->isEnabled())
	{
	}

	virtual ~DefaultPassWidget() {};

	virtual void update(double /*deltaTime*/) override
	{
		if (m_enabled != m_renderPass->isEnabled())
		{
			m_renderPass->setEnabled(m_enabled);
		}
	}

	virtual void draw() override
	{
		ImGui::Checkbox("Enable", &m_enabled);
	}

	virtual const std::string& name() const override
	{
		return m_renderPass->name();
	}

private:

	bool m_enabled;

	IRenderPassSPtr m_renderPass;
};

class BloomPassWidget : public DefaultPassWidget
{
public:
	BloomPassWidget(BloomRenderPassSPtr pass)
		: DefaultPassWidget(pass)
		, m_bloomPass(pass)
		, m_iterations(pass->iterations())
		, m_threshold(pass->threshold())
		, m_intensity(pass->intensity())
	{
	}

	virtual ~BloomPassWidget() {};

	virtual void update(double deltaTime) override
	{
		DefaultPassWidget::update(deltaTime);

		if (m_iterations != m_bloomPass->iterations())
		{
			m_bloomPass->setIterations(m_iterations);
		}
		if (MathUtils::numericClose(m_threshold, m_bloomPass->threshold()))
		{
			m_bloomPass->setThreshold(m_threshold);
		}
		if (MathUtils::numericClose(m_intensity, m_bloomPass->intensity()))
		{
			m_bloomPass->setIntensity(m_intensity);
		}
	}

	virtual void draw() override
	{
		DefaultPassWidget::draw();

		const int minIter = 1;
		const int maxIter = 10;
		const float minThre = .01f;
		const float maxThre = 10.f;
		const float minInt = .01f;
		const float maxInt = 1.f;

		ImGui::SliderScalar("Iterations", ImGuiDataType_S32, &m_iterations, &minIter, &maxIter);
		ImGui::SliderScalar("Threshold", ImGuiDataType_Float, &m_threshold, &minThre, &maxThre, "%.2f");
		ImGui::SliderScalar("Intensity", ImGuiDataType_Float, &m_intensity, &minInt, &maxInt, "%.2f");

		if (ImGui::TreeNode("Buffer"))
		{
			constexpr float scale = 1.f;
			ImGui::Image((void*)(intptr_t)m_bloomPass->blurBuffer()->handle(),
				{ m_bloomPass->blurBuffer()->width() * scale, m_bloomPass->blurBuffer()->height() * scale },
				{ 0,1 }, { 1,0 });

			ImGui::TreePop();
		}
	}

private:

	int m_iterations;

	float m_threshold;

	float m_intensity;

	BloomRenderPassSPtr m_bloomPass;
};

class ScenePassWidget : public DefaultPassWidget
{
public:
	ScenePassWidget(GeometryRenderPassSPtr pass)
		: DefaultPassWidget(pass)
		, m_scenePass(pass)
		, m_wireframeMode(pass->wireframeMode())
	{
	}

	virtual ~ScenePassWidget() {};

	virtual void update(double deltaTime) override
	{
		DefaultPassWidget::update(deltaTime);

		if (m_wireframeMode != m_scenePass->wireframeMode())
		{
			m_scenePass->setWireframeMode(m_wireframeMode);
		}
	}

	virtual void draw() override
	{
		DefaultPassWidget::draw();

		ImGui::Checkbox("Wireframe", &m_wireframeMode);
	}

private:

	bool m_wireframeMode;

	GeometryRenderPassSPtr m_scenePass;
};

class ShadowPassWidget : public DefaultPassWidget
{
public:
	ShadowPassWidget(ShadowMappingRenderPassSPtr pass)
		: DefaultPassWidget(pass)
		, m_shadowPass(pass)
		, m_depthOffsetFactor(pass->depthOffsetFactor())
		, m_depthOffsetUnit(pass->depthOffsetUnit())
	{
	}

	virtual ~ShadowPassWidget() {};

	virtual void update(double deltaTime) override
	{
		DefaultPassWidget::update(deltaTime);

		if (MathUtils::numericClose(m_depthOffsetFactor, m_shadowPass->depthOffsetFactor()))
		{
			m_shadowPass->setDepthOffsetFactor(m_depthOffsetFactor);
		}
		if (MathUtils::numericClose(m_depthOffsetUnit, m_shadowPass->depthOffsetUnit()))
		{
			m_shadowPass->setDepthOffsetUnit(m_depthOffsetUnit);
		}
	}

	virtual void draw() override
	{
		DefaultPassWidget::draw();

		const float min = .0f;
		const float max = 100.f;

		ImGui::SliderScalar("Bias Factor", ImGuiDataType_Float, &m_depthOffsetFactor, &min, &max, "%.2f");
		ImGui::SliderScalar("Bias Unit", ImGuiDataType_Float, &m_depthOffsetUnit, &min, &max, "%.2f");

		if (ImGui::TreeNode("Shadow Maps"))
		{
			for (const auto& [light, data] : m_shadowPass->shadowData())
			{
				Texture2DSPtr depthMap = data.target->depthBufferAs<DepthTextureWrapper>()->texture();

				constexpr float scale = 0.5f;
				ImGui::Image((void*)(intptr_t)depthMap->handle(),
					{ depthMap->width() * scale, depthMap->height() * scale },
					{ 0,1 }, { 1,0 });
			}
			ImGui::TreePop();
		}		
	}

private:

	float m_depthOffsetFactor = 0.f;
	float m_depthOffsetUnit = 0.f;

	ShadowMappingRenderPassSPtr m_shadowPass;
};

RenderPassListWidget::RenderPassListWidget(const std::string& title, RenderEngineSPtr renderEngine)
	: m_title(title)
	, m_renderEngine(renderEngine)
{
	// register factory methods
	m_widgetFactory["Bloom"] = [](IRenderPassSPtr pass) -> IRenderPassWidgetUPtr
	{
		return std::make_unique<BloomPassWidget>(std::static_pointer_cast<BloomRenderPass>(pass));
	};
	m_widgetFactory["Opaque Scene"] = [](IRenderPassSPtr pass) -> IRenderPassWidgetUPtr
	{
		return std::make_unique<ScenePassWidget>(std::static_pointer_cast<GeometryRenderPass>(pass));
	};
	m_widgetFactory["Transparent Scene"] = [](IRenderPassSPtr pass) -> IRenderPassWidgetUPtr
	{
		return std::make_unique<ScenePassWidget>(std::static_pointer_cast<GeometryRenderPass>(pass));
	};
	m_widgetFactory["Shadow Mapping"] = [](IRenderPassSPtr pass) -> IRenderPassWidgetUPtr
	{
		return std::make_unique<ShadowPassWidget>(std::static_pointer_cast<ShadowMappingRenderPass>(pass));
	};
}

void RenderPassListWidget::update(double deltaTime)
{
	if (!visible()) return;

	if (m_renderPassWidgets.empty())
	{
		for (IRenderPassSPtr pass : m_renderEngine->renderPasses())
		{
			auto found = m_widgetFactory.find(pass->name());
			if (found != m_widgetFactory.end())
			{
				m_renderPassWidgets.emplace_back(found->second(pass));
			}
			else
			{
				m_renderPassWidgets.emplace_back(std::make_unique<DefaultPassWidget>(pass));
			}
		}
	}

	for (IRenderPassWidgetSPtr passWidget : m_renderPassWidgets)
	{
		passWidget->update(deltaTime);
	}
}

void RenderPassListWidget::draw()
{
	if (!visible()) return;

	if (!ImGui::Begin(m_title.c_str(), &m_visible))
	{
		ImGui::End();
		return;
	}

	for (IRenderPassWidgetSPtr widget : m_renderPassWidgets)
	{
		//if (ImGui::CollapsingHeader(widget->name().c_str()))
		if (ImGui::TreeNode(widget->name().c_str()))
		{
			widget->draw();

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

const std::string& RenderPassListWidget::name() const
{
	return m_title;
}

bool RenderPassListWidget::visible() const
{
	return m_visible;
}

void RenderPassListWidget::setVisible(bool flag)
{
	m_visible = flag;
}
