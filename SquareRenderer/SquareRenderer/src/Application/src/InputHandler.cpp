#include "Application/InputHandler.h"
#include "Application/GLWindow.h"
#include "Renderer/Camera.h"
#include "Common/Logger.h"

#include <glm/gtx/transform.hpp>
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
	glfwSetCursorPosCallback(m_handle, onCursorMove);
	glfwSetScrollCallback(m_handle, onScroll);
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

void InputHandler::update(double deltaTime)
{
	m_deltaTime = deltaTime;

	glfwPollEvents();

	// keyboard inputs
	if (keyDown(Key::Escape))
	{
		GLWindow* window = GLWindow::getGLWindowFromHandle(m_handle);
		if (window)
		{
			window->close();
		}
	}

	// mouse buttons
	if (!m_guiHasMouseFocus &&
		!m_leftMouseButtonPressed &&
		glfwGetMouseButton(m_handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		m_leftMouseButtonPressed = true;
		//Logger::Debug("Left mouse button pressed at (%f,%f)", 
		//	m_currentCursorPosition.x, m_currentCursorPosition.y);

		m_lastCursorPosition = m_currentCursorPosition;
		m_deltaCursorPosition = glm::vec2(0, 0);
	}
	if (m_leftMouseButtonPressed &&
		glfwGetMouseButton(m_handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		m_leftMouseButtonPressed = false;
		//Logger::Debug("Left mouse button released");
	}
}

void InputHandler::enableCameraNavigation(CameraSPtr camera, float orbitSpeed, float zoomSpeed)
{
	m_camera = camera;
	m_orbitSpeed = orbitSpeed;
	m_zoomSpeed = zoomSpeed;
}

const glm::vec2& InputHandler::cursorPosition() const
{
	return m_currentCursorPosition;
}

void InputHandler::guiHasMouseFocus(bool flag)
{
	m_guiHasMouseFocus = flag;
}

void InputHandler::handleCursorMove(double x, double y)
{
	glm::vec2 newCursorPosition(x, y);

	//Logger::Debug("Drag (%.3f, %.3f)", newCursorPosition.x, newCursorPosition.y);

	if (m_camera && (m_leftMouseButtonPressed || m_middleMouseButtonPressed))
	{
		m_deltaCursorPosition = newCursorPosition - m_lastCursorPosition;

		m_lastCursorPosition = newCursorPosition;

		const glm::vec2 delta(
			m_deltaCursorPosition.x / static_cast<float>(m_camera->width()),
			m_deltaCursorPosition.y / static_cast<float>(m_camera->height()));

		//Logger::Debug("Drag (%.3f, %.3f)", m_deltaCursorPosition.x, m_deltaCursorPosition.y);

		if (m_leftMouseButtonPressed)
		{
			// orbit
			const float rotationSpeed = m_orbitSpeed * static_cast<float>(m_deltaTime);
			glm::vec3 camPosition = m_camera->position();

			// get horizontal rotation (yaw)
			glm::mat3 horizontalRot = glm::rotate(rotationSpeed * delta.x , -glm::vec3(0, 1, 0));
			camPosition = horizontalRot * camPosition;

			// get vertical rotation (pitch)
			glm::vec3 camView = glm::normalize(camPosition - m_orbitOrigin);
			glm::vec3 camRight = glm::normalize(glm::cross(camView, glm::vec3(0, 1, 0)));
			glm::mat3 verticalRot = glm::rotate(rotationSpeed * delta.y, camRight);
			camPosition = verticalRot * camPosition;

			constexpr float maxYaw = 0.99f;
			const float angle = glm::dot(glm::normalize(camPosition), glm::vec3(0, 1, 0));
			if (std::abs(angle) < maxYaw)
			{
				m_camera->lookAt(camPosition, m_orbitOrigin);
			}
		}
		else
		{
			//onNavPan(delta);
		}
	}

	m_currentCursorPosition = newCursorPosition;
}

void InputHandler::handleScroll(double /*x*/, double y)
{
	if (m_camera && !m_guiHasMouseFocus)
	{
		constexpr float minDist = 0.2f;
		const float zoomSpeed = m_zoomSpeed * static_cast<float>(m_deltaTime * y);

		glm::vec3 position = m_camera->position();

		position += m_camera->viewDirection() * zoomSpeed;

		const float distance = glm::distance(position, m_orbitOrigin);
		if (distance > minDist)
		{
			m_camera->lookAt(position, m_orbitOrigin);
		}
	}
}

void InputHandler::onCursorMove(GLFWwindow* handle, double x, double y)
{
	GLWindow* window = GLWindow::getGLWindowFromHandle(handle);
	if (window)
	{
		window->inputHandler()->handleCursorMove(x, y);
	}
}

void InputHandler::onScroll(GLFWwindow* handle, double x, double y)
{
	GLWindow* window = GLWindow::getGLWindowFromHandle(handle);
	if (window)
	{
		window->inputHandler()->handleScroll(x, y);
	}
}
