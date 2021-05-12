#pragma once

#include "Common/Macros.h"

struct GLFWwindow;

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

class InputHandler
{
public:
	InputHandler(GLFWwindow* handle);

	virtual ~InputHandler();

	bool keyDown(Key k) const;
	bool keyPressed(Key k) const;
	bool keyUp(Key k) const;

	void poll() const;

protected:
	
	GLFWwindow* m_handle;
};

