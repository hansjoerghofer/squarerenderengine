#pragma once

#include "Common/Macros.h"

#include <glm/glm.hpp>

enum class Key
{
	Escape, Enter, Pause,
	Space, Tab, Backspace, Caps,
	Insert, Delete,
	Right, Left, Down, Up,
	Num0, Num1, Num2, Num3, Num4,
	Num5, Num6, Num7, Num8, Num9,
	A, B, C, D, E, F, G, H, I, J, K, L, M, 
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

struct GLFWwindow;
DECLARE_PTRS(Camera);

class InputHandler
{
public:
	InputHandler(GLFWwindow* handle);

	virtual ~InputHandler();

	bool keyDown(Key k) const;
	bool keyPressed(Key k) const;
	bool keyUp(Key k) const;

	void update(double deltaTime);

	void enableCameraNavigation(CameraSPtr camera, 
		float orbitSpeed = 1.f, 
		float zoomSpeed = 1.f);

	const glm::vec2& cursorPosition() const;

	void guiHasMouseFocus(bool flag);

protected:

	void handleCursorMove(double x, double y);
	void handleScroll(double x, double y);

	static void onCursorMove(GLFWwindow* handle, double x, double y);
	static void onScroll(GLFWwindow* handle, double x, double y);
	
	glm::vec2 m_currentCursorPosition = glm::vec2(0,0);
	glm::vec2 m_lastCursorPosition = glm::vec2(0, 0);
	glm::vec2 m_deltaCursorPosition = glm::vec2(0, 0);

	CameraSPtr m_camera;

	bool m_leftMouseButtonPressed = false;
	bool m_middleMouseButtonPressed = false;
	bool m_rightMouseButtonPressed = false;

	double m_deltaTime = 0.0;

	float m_orbitSpeed = 1.f;
	float m_zoomSpeed = 1.f;
	glm::vec3 m_orbitOrigin = glm::vec3(0);

	bool m_guiHasMouseFocus = false;

	GLFWwindow* m_handle;
};

