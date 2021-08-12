#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include "GUI/IWidget.h"

#include <string>
#include <vector>

DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(GLWindow);

class StatisticsWidget : public IWidget
{
public:

	StatisticsWidget(const std::string& title, GLWindowSPtr window, RenderEngineSPtr engine);

	void update(double deltaTime) override;

	void draw() override;

private:

	GLWindowSPtr m_window;

	RenderEngineSPtr m_engine;

	double m_frameTimeAcc = 0.0;

	double m_displayTimeAcc = 0.0;
	double m_renderTimeAcc = 0.0;

	std::string m_title;

	bool m_visible;
};

