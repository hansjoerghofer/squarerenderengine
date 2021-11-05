#pragma once
#include "GUI/IWidget.h"
#include "Common/Math3D.h"

#include <unordered_map>

DECLARE_PTRS(Scene);
DECLARE_PTRS(ILightsource);

class LightingWidget : public IWidget
{
public:

	LightingWidget(const std::string& title, SceneSPtr scene);

	virtual ~LightingWidget();

	virtual const std::string& name() const override;

	virtual bool visible() const override;

	virtual void setVisible(bool flag) override;

	virtual void update(double deltaTime) override;

	virtual void draw() override;

private:

	std::string m_title;

	bool m_visible = false;

	SceneSPtr m_scene;

	struct Values
	{
		bool dirty = false;
		glm::vec3 vec;
		glm::vec3 col;
	};

	std::unordered_map<ILightsourceSPtr, Values> m_values;
};

