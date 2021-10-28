#pragma once

#include "Common/Macros.h"
#include "Common/Logger.h"
#include "Renderer/IRenderTarget.h"
#include "API/SharedResource.h"

#include <string>

struct GLFWwindow;
class InputHandler;

DECLARE_PTRS(LogWidget);
DECLARE_PTRS(GLWindow);
DECLARE_PTRS(InputHandler);
DECLARE_PTRS(IWidget);

class GLWindow : public IRenderTarget
{
public:
	GLWindow(int width, int height, const std::string& title);
	virtual ~GLWindow();

	void close();
	bool isOpen() const;

	void update(double deltaTime);

	void render();
	void swapBuffers();

	virtual int width() const override;
	virtual int height() const override;
	virtual glm::vec4 dimensions() const override;
	virtual SharedResource::Handle handle() const override;

	float dpiScaling() const;

	bool enableGUI();
	void addWidget(IWidgetSPtr widget);

	InputHandlerSPtr inputHandler() const;

protected:

	std::string m_title;

	int m_width;
	int m_height;
	float m_dpiScaling = 1.f;

	bool m_guiInitialized = false;
	bool m_showUiDemo = false;

	std::list<IWidgetSPtr> m_widgets;
	LogCallbackSPtr m_logCallback;

	InputHandlerSPtr m_inputHandler;

	GLFWwindow* m_handle = nullptr;

	bool makeContextCurrent();

	virtual void onGUI();
	virtual void onResize(int width, int height);

private:
	friend InputHandler;

	void renderGUI();
	
	void cleanupGUI();

	void handleResize(int width, int height);

	static GLWindow* getGLWindowFromHandle(GLFWwindow* handle);

	static void onResizeCallback(GLFWwindow* handle, int width, int height);
};

