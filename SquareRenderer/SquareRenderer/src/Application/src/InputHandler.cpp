#include "Application/InputHandler.h"
#include "Common/Logger.h"

#include <GLFW/glfw3.h>

int ConvertKey(Key k)
{
	switch (k)
	{
	case Key::Escape:	return GLFW_KEY_ESCAPE;
	case Key::Space:	return GLFW_KEY_SPACE;
	// TODO all other cases!
	default:
		Logger::Warning("Unhandled Key, missing implementation!");
		return GLFW_KEY_UNKNOWN;
	}
}

InputHandler::InputHandler(GLFWwindow* handle)
	: m_handle(handle)
{
	//glfwSetCursorPosCallback(window->m_handle, mouse_callback);
}

InputHandler::~InputHandler()
{
}

bool InputHandler::keyDown(Key k) const
{
	return glfwGetKey(m_handle, ConvertKey(k)) == GLFW_PRESS;
}

bool InputHandler::keyPressed(Key k) const
{
	return glfwGetKey(m_handle, ConvertKey(k)) == GLFW_REPEAT;
}

bool InputHandler::keyUp(Key k) const
{
	return glfwGetKey(m_handle, ConvertKey(k)) == GLFW_RELEASE;
}

void InputHandler::poll() const
{
	glfwPollEvents();
}
