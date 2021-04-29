#pragma once

#include <memory>

class GLWindow;

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
	InputHandler(std::shared_ptr<GLWindow> window);

	virtual ~InputHandler();

	bool keyDown(Key k);
	bool keyPressed(Key k);
	bool keyUp(Key k);

	void poll();

protected:
	
	std::shared_ptr<GLWindow> m_window;
};

