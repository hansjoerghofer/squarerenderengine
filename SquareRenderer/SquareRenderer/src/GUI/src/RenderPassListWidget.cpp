#include "GUI/RenderPassListWidget.h"
#include "GUI/imgui.h"

#include "Renderer/RenderEngine.h"
#include "Renderer/BloomRenderPass.h"
#include "Renderer/GeometryRenderPass.h"

#include "Texture/ITexture.h"

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
		if (m_threshold != m_bloomPass->threshold())
		{
			m_bloomPass->setThreshold(m_threshold);
		}
	}

	virtual void draw() override
	{
		DefaultPassWidget::draw();

		const int minIter = 1;
		const int maxIter = 10;
		const float minThre = .01f;
		const float maxThre = 10.f;

		ImGui::SliderScalar("Iterations", ImGuiDataType_S32, &m_iterations, &minIter, &maxIter);
		ImGui::SliderScalar("Threshold", ImGuiDataType_Float, &m_threshold, &minThre, &maxThre, "%.2f");

		constexpr float scale = 1.f;
		ImGui::Image((void*)(intptr_t)m_bloomPass->blurBuffer()->handle(),
			{ m_bloomPass->blurBuffer()->width() * scale, m_bloomPass->blurBuffer()->height() * scale },
			{ 0,1 }, { 1,0 });
	}

private:

	int m_iterations;

	float m_threshold;

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
