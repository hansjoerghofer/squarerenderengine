#pragma once

#include <string>
#include <map>
#include <memory>

struct GLFWwindow;
class InputHandler;

class GLWindow
{
public:
	GLWindow(int width, int height, const std::string& title);
	virtual ~GLWindow();

	void close();

	void renderGUI();
	void swapBuffers();

	bool isOpen() const;

	int width() const;
	int height() const;

	bool enableGUI();

protected:
	friend InputHandler;

	int m_width;
	int m_height;

	std::string m_title;

	float m_dpiScaling;

	bool m_guiInitialized;

	bool m_showUiDemo;

	GLFWwindow* m_handle;

	bool makeContextCurrent();

	void onResize(int width, int height);

private:
	void cleanupGUI();

	static GLWindow* getWindowFromHandle(GLFWwindow* handle);

	static void onResizeCallback(GLFWwindow* handle, int width, int height);

	
};

