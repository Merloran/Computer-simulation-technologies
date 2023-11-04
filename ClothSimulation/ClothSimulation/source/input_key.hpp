#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

enum class InputKey {
	UNKNOWN,

	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	SPACE,
	ENTER,
	ESCAPE,
	BACKSPACE,
	TAB,
	LEFT_CONTROL,
	RIGHT_CONTROL,
	LEFT_SHIFT,
	RIGHT_SHIFT,
	LEFT_ALT,
	RIGHT_ALT,
	UP,
	DOWN,
	LEFT,
	RIGHT,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
};

static InputKey glfw_mouse_button_to_input_key(int button) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		return InputKey::MOUSE_LEFT;
	case GLFW_MOUSE_BUTTON_RIGHT:
		return InputKey::MOUSE_RIGHT;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return InputKey::MOUSE_MIDDLE;
	default:
		return InputKey::UNKNOWN;
	}
}

static InputKey glfw_key_to_input_key(int key) {
	switch (key) {
	case GLFW_KEY_A:
		return InputKey::A;
	case GLFW_KEY_B:
		return InputKey::B;
	case GLFW_KEY_C:
		return InputKey::C;
	case GLFW_KEY_D:
		return InputKey::D;
	case GLFW_KEY_E:
		return InputKey::E;
	case GLFW_KEY_F:
		return InputKey::F;
	case GLFW_KEY_G:
		return InputKey::G;
	case GLFW_KEY_H:
		return InputKey::H;
	case GLFW_KEY_I:
		return InputKey::I;
	case GLFW_KEY_J:
		return InputKey::J;
	case GLFW_KEY_K:
		return InputKey::K;
	case GLFW_KEY_L:
		return InputKey::L;
	case GLFW_KEY_M:
		return InputKey::M;
	case GLFW_KEY_N:
		return InputKey::N;
	case GLFW_KEY_O:
		return InputKey::O;
	case GLFW_KEY_P:
		return InputKey::P;
	case GLFW_KEY_Q:
		return InputKey::Q;
	case GLFW_KEY_R:
		return InputKey::R;
	case GLFW_KEY_S:
		return InputKey::S;
	case GLFW_KEY_T:
		return InputKey::T;
	case GLFW_KEY_U:
		return InputKey::U;
	case GLFW_KEY_V:
		return InputKey::V;
	case GLFW_KEY_W:
		return InputKey::W;
	case GLFW_KEY_X:
		return InputKey::X;
	case GLFW_KEY_Y:
		return InputKey::Y;
	case GLFW_KEY_Z:
		return InputKey::Z;
	case GLFW_KEY_SPACE:
		return InputKey::SPACE;
	case GLFW_KEY_ENTER:
		return InputKey::ENTER;
	case GLFW_KEY_ESCAPE:
		return InputKey::ESCAPE;
	case GLFW_KEY_BACKSPACE:
		return InputKey::BACKSPACE;
	case GLFW_KEY_TAB:
		return InputKey::TAB;
	case GLFW_KEY_LEFT_CONTROL:
		return InputKey::LEFT_CONTROL;
	case GLFW_KEY_RIGHT_CONTROL:
		return InputKey::RIGHT_CONTROL;
	case GLFW_KEY_LEFT_SHIFT:
		return InputKey::LEFT_SHIFT;
	case GLFW_KEY_RIGHT_SHIFT:
		return InputKey::RIGHT_SHIFT;
	case GLFW_KEY_LEFT_ALT:
		return InputKey::LEFT_ALT;
	case GLFW_KEY_RIGHT_ALT:
		return InputKey::RIGHT_ALT;
	case GLFW_KEY_UP:
		return InputKey::UP;
	case GLFW_KEY_DOWN:
		return InputKey::DOWN;
	case GLFW_KEY_LEFT:
		return InputKey::LEFT;
	case GLFW_KEY_RIGHT:
		return InputKey::RIGHT;
	case GLFW_KEY_F1:
		return InputKey::F1;
	case GLFW_KEY_F2:
		return InputKey::F2;
	case GLFW_KEY_F3:
		return InputKey::F3;
	case GLFW_KEY_F4:
		return InputKey::F4;
	case GLFW_KEY_F5:
		return InputKey::F5;
	case GLFW_KEY_F6:
		return InputKey::F6;
	case GLFW_KEY_F7:
		return InputKey::F7;
	case GLFW_KEY_F8:
		return InputKey::F8;
	case GLFW_KEY_F9:
		return InputKey::F9;
	case GLFW_KEY_F10:
		return InputKey::F10;
	case GLFW_KEY_F11:
		return InputKey::F11;
	case GLFW_KEY_F12:
		return InputKey::F12;
	default:
		return InputKey::UNKNOWN;
	}
}

