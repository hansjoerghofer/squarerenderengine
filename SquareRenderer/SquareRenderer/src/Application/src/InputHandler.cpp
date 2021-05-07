#include "Application/InputHandler.h"
#include "Application/GLWindow.h"

#include <GLFW/glfw3.h>

int ConvertKey(Key k)
{
	switch (k)
	{
	case Key::Escape:	return GLFW_KEY_ESCAPE;
	case Key::Space:	return GLFW_KEY_SPACE;
	// TODO all other cases!
	default:			return GLFW_KEY_UNKNOWN;
	}
}

InputHandler::InputHandler(GLWindowSPtr window)
	: m_window(window)
{
}

InputHandler::~InputHandler()
{
}

bool InputHandler::keyDown(Key k)
{
	return glfwGetKey(m_window->m_handle, ConvertKey(k)) == GLFW_PRESS;
}

bool InputHandler::keyPressed(Key k)
{
	return glfwGetKey(m_window->m_handle, ConvertKey(k)) == GLFW_REPEAT;
}

bool InputHandler::keyUp(Key k)
{
	return glfwGetKey(m_window->m_handle, ConvertKey(k)) == GLFW_RELEASE;
}

void InputHandler::poll()
{
	glfwPollEvents();
}
