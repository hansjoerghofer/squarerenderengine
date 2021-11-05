#pragma once

#include "Common/Macros.h"
#include "GUI/IWidget.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(IRenderPass);
DECLARE_PTRS(IRenderPassWidget);

class IRenderPassWidget
{
public:
	virtual ~IRenderPassWidget() {};

	virtual void update(double deltaTime) = 0;

	virtual void draw() = 0;

	virtual const std::string& name() const = 0;
};

class RenderPassListWidget : public IWidget
{
public:
	RenderPassListWidget(const std::string& title, RenderEngineSPtr renderEngine);

	void update(double deltaTime) override;

	void draw() override;

	const std::string& name() const override;

	bool visible() const override;

	void setVisible(bool flag) override;

private:

	bool m_visible = true;

	std::string m_title;

	RenderEngineSPtr m_renderEngine;

	std::unordered_map<std::string, std::function<IRenderPassWidgetUPtr(IRenderPassSPtr)>> m_widgetFactory;

	std::vector<IRenderPassWidgetSPtr> m_renderPassWidgets;
};

