#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include "GUI/IWidget.h"

#include <string>
#include <vector>

DECLARE_PTRS(RenderEngine);
DECLARE_PTRS(GLWindow);

constexpr auto FRAMERATE_BUFFER_SIZE = 128;

class StatisticsWidget : public IWidget
{
public:

	StatisticsWidget(const std::string& title, GLWindowSPtr window, RenderEngineSPtr engine);

	void update(double deltaTime) override;

	void draw() override;

	const std::string& name() const override;

	bool visible() const override;

	void setVisible(bool flag) override;

private:

	GLWindowSPtr m_window;

	RenderEngineSPtr m_engine;

	double m_frameTimeAcc = 0.0;

	double m_displayTimeAcc = 0.0;
	double m_renderTimeAcc = 0.0;

	int m_frametimeIndex = 0;
	float m_frametimeBuffer[FRAMERATE_BUFFER_SIZE];

	std::string m_title;

	bool m_visible;
};

