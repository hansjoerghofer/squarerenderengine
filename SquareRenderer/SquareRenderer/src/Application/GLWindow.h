#pragma once

#include "Common/Macros.h"
#include "Common/Logger.h"

#include <string>

struct GLFWwindow;
class InputHandler;

DECLARE_PTRS(LogWidget);
DECLARE_PTRS(GLWindow);
DECLARE_PTRS(InputHandler);

class GLWindow
{
public:
	GLWindow(int width, int height, const std::string& title);
	virtual ~GLWindow();

	void close();
	bool isOpen() const;

	void renderGUI();
	void swapBuffers();

	int width() const;
	int height() const;
	float dpiScaling() const;

	bool enableGUI();

	const InputHandler& inputHandler() const;

protected:

	std::string m_title;

	int m_width;
	int m_height;
	float m_dpiScaling = 1.f;

	bool m_guiInitialized = false;
	bool m_showUiDemo = true;

	LogWidgetSPtr m_logUiWidget;
	LogCallbackSPtr m_logCallback;
	InputHandlerSPtr m_inputHandler;

	GLFWwindow* m_handle = nullptr;

	bool makeContextCurrent();

	virtual void onGUI();
	virtual void onResize(int width, int height);

private:
	void cleanupGUI();

	void handleResize(int width, int height);

	static GLWindow* getGLWindowFromHandle(GLFWwindow* handle);

	static void onResizeCallback(GLFWwindow* handle, int width, int height);
};

