
#ifndef __SIBR_GRAPHICS_INPUT_HPP__
# define __SIBR_GRAPHICS_INPUT_HPP__

# include <array>

//#define GLEW_STATIC
#include <GL/glew.h>
# define GLFW_INCLUDE_GLU
# include <GLFW/glfw3.h>

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Viewport.hpp"

namespace sibr
{
	namespace Key
	{
		enum Code
		{
			Unknown = 0 /*GLFW_KEY_UNKNOWN*/,   
			Space = GLFW_KEY_SPACE,   
			Apostrophe = GLFW_KEY_APOSTROPHE,   
			Comma = GLFW_KEY_COMMA,   
			Minus = GLFW_KEY_MINUS,   
			Period = GLFW_KEY_PERIOD,   
			Slash = GLFW_KEY_SLASH,   
			Num0 = GLFW_KEY_0,   
			Num1 = GLFW_KEY_1,   
			Num2 = GLFW_KEY_2,   
			Num3 = GLFW_KEY_3,   
			Num4 = GLFW_KEY_4,   
			Num5 = GLFW_KEY_5,   
			Num6 = GLFW_KEY_6,   
			Num7 = GLFW_KEY_7,   
			Num8 = GLFW_KEY_8,   
			Num9 = GLFW_KEY_9,   
			Semicolon = GLFW_KEY_SEMICOLON,   
			Equal = GLFW_KEY_EQUAL,   
			A = GLFW_KEY_A,   
			B = GLFW_KEY_B,   
			C = GLFW_KEY_C,   
			D = GLFW_KEY_D,   
			E = GLFW_KEY_E,   
			F = GLFW_KEY_F,   
			G = GLFW_KEY_G,   
			H = GLFW_KEY_H,   
			I = GLFW_KEY_I,   
			J = GLFW_KEY_J,   
			K = GLFW_KEY_K,   
			L = GLFW_KEY_L,   
			M = GLFW_KEY_M,   
			N = GLFW_KEY_N,   
			O = GLFW_KEY_O,   
			P = GLFW_KEY_P,   
			Q = GLFW_KEY_Q,   
			R = GLFW_KEY_R,   
			S = GLFW_KEY_S,   
			T = GLFW_KEY_T,   
			U = GLFW_KEY_U,   
			V = GLFW_KEY_V,   
			W = GLFW_KEY_W,   
			X = GLFW_KEY_X,   
			Y = GLFW_KEY_Y,   
			Z = GLFW_KEY_Z,   
			LeftBracket = GLFW_KEY_LEFT_BRACKET,   
			Backslash = GLFW_KEY_BACKSLASH,   
			RightBracket = GLFW_KEY_RIGHT_BRACKET,   
			GraveAccent = GLFW_KEY_GRAVE_ACCENT,   
			World1 = GLFW_KEY_WORLD_1,   
			World2 = GLFW_KEY_WORLD_2,   
			Escape = GLFW_KEY_ESCAPE,   
			Enter = GLFW_KEY_ENTER,   
			Tab = GLFW_KEY_TAB,   
			Backspace = GLFW_KEY_BACKSPACE,   
			Insert = GLFW_KEY_INSERT,   
			Delete = GLFW_KEY_DELETE,   
			Right = GLFW_KEY_RIGHT,   
			Left = GLFW_KEY_LEFT,   
			Down = GLFW_KEY_DOWN,   
			Up = GLFW_KEY_UP,   
			Page_up = GLFW_KEY_PAGE_UP,   
			Page_down = GLFW_KEY_PAGE_DOWN,   
			Home = GLFW_KEY_HOME,   
			End = GLFW_KEY_END,   
			CapsLock = GLFW_KEY_CAPS_LOCK,   
			ScrollLock = GLFW_KEY_SCROLL_LOCK,   
			NumLock = GLFW_KEY_NUM_LOCK,   
			PrintScreen = GLFW_KEY_PRINT_SCREEN,   
			Pause = GLFW_KEY_PAUSE,   
			F1 = GLFW_KEY_F1,   
			F2 = GLFW_KEY_F2,   
			F3 = GLFW_KEY_F3,   
			F4 = GLFW_KEY_F4,   
			F5 = GLFW_KEY_F5,   
			F6 = GLFW_KEY_F6,   
			F7 = GLFW_KEY_F7,   
			F8 = GLFW_KEY_F8,   
			F9 = GLFW_KEY_F9,   
			F10 = GLFW_KEY_F10,   
			F11 = GLFW_KEY_F11,   
			F12 = GLFW_KEY_F12,   
			F13 = GLFW_KEY_F13,   
			F14 = GLFW_KEY_F14,   
			F15 = GLFW_KEY_F15,   
			F16 = GLFW_KEY_F16,   
			F17 = GLFW_KEY_F17,   
			F18 = GLFW_KEY_F18,   
			F19 = GLFW_KEY_F19,   
			F20 = GLFW_KEY_F20,   
			F21 = GLFW_KEY_F21,   
			F22 = GLFW_KEY_F22,   
			F23 = GLFW_KEY_F23,   
			F24 = GLFW_KEY_F24,   
			F25 = GLFW_KEY_F25,   
			KPNum0 = GLFW_KEY_KP_0,   
			KPNum1 = GLFW_KEY_KP_1,   
			KPNum2 = GLFW_KEY_KP_2,   
			KPNum3 = GLFW_KEY_KP_3,   
			KPNum4 = GLFW_KEY_KP_4,   
			KPNum5 = GLFW_KEY_KP_5,   
			KPNum6 = GLFW_KEY_KP_6,   
			KPNum7 = GLFW_KEY_KP_7,   
			KPNum8 = GLFW_KEY_KP_8,   
			KPNum9 = GLFW_KEY_KP_9,   
			KPDecimal = GLFW_KEY_KP_DECIMAL,   
			KPDivide = GLFW_KEY_KP_DIVIDE,   
			KPMultiply = GLFW_KEY_KP_MULTIPLY,   
			KPSubtract = GLFW_KEY_KP_SUBTRACT,   
			KPAdd = GLFW_KEY_KP_ADD,   
			KPEnter = GLFW_KEY_KP_ENTER,   
			KPEqual = GLFW_KEY_KP_EQUAL,   
			LeftShift = GLFW_KEY_LEFT_SHIFT,   
			LeftControl = GLFW_KEY_LEFT_CONTROL,   
			LeftAlt = GLFW_KEY_LEFT_ALT,   
			LeftSuper = GLFW_KEY_LEFT_SUPER,   
			RightShift = GLFW_KEY_RIGHT_SHIFT,   
			RightControl = GLFW_KEY_RIGHT_CONTROL,   
			RightAlt = GLFW_KEY_RIGHT_ALT,   
			RightSuper = GLFW_KEY_RIGHT_SUPER,   
			Menu = GLFW_KEY_MENU,  

			count // this one is a 'tricks' to automatically get the number
			// of elements in this enum (just type sibr::Key::count).
		};
	} // namespace Key

	namespace Mouse
	{
		enum Code
		{
			Button1 = GLFW_MOUSE_BUTTON_1, 
			Button2 = GLFW_MOUSE_BUTTON_2,
			Button3 = GLFW_MOUSE_BUTTON_3,
			Button4 = GLFW_MOUSE_BUTTON_4,
			Button5 = GLFW_MOUSE_BUTTON_5,
			Button6 = GLFW_MOUSE_BUTTON_6,
			Button7 = GLFW_MOUSE_BUTTON_7,
			Button8 = GLFW_MOUSE_BUTTON_8,
			Last = GLFW_MOUSE_BUTTON_LAST,

			Left = GLFW_MOUSE_BUTTON_LEFT,
			Middle = GLFW_MOUSE_BUTTON_MIDDLE,
			Right = GLFW_MOUSE_BUTTON_RIGHT,

			Unknown,
			count
		};
	} // namespace Mouse

	struct SIBR_GRAPHICS_EXPORT KeyCombination 
	{
		KeyCombination();
		KeyCombination(int n, bool b); 
		
		operator bool() const; 

		int numKeys;
		bool isTrue;
	};
	
	KeyCombination SIBR_GRAPHICS_EXPORT operator&&( const KeyCombination & combA, const KeyCombination & combB);

	template <int TNbState, typename TEnum>
	class InputState
	{
	public:
		bool	isActivated( TEnum code ) const {
			return _currentStates[(size_t)code];
		}
		bool	isReleased( TEnum code ) const {
			return _lastStates[(size_t)code] \
				&& !_currentStates[(size_t)code];
		}
		/** Return TRUE if has been pressed since this frame. */
		bool	isPressed( TEnum code ) const {
			return !_lastStates[(size_t)code] \
				&& _currentStates[(size_t)code];
		}
		
		KeyCombination isPressedOnly( TEnum code ) const {
			return KeyCombination(1,isPressed(code));
		}

		KeyCombination isActivatedOnly( TEnum code ) const {
			return KeyCombination(1,isActivated(code));
		}

		bool	isActivated( TEnum code, const char* desc ) {
			size_t c = (size_t)code;
			if (_shortcuts[c] == nullptr)
				_shortcuts[c] = desc;
			else if (_shortcuts[c] != desc)
				SIBR_ERR << "shortcut already used (current: '"
				<< _shortcuts[c] << "', conflict with '" << desc << "')"
				<< std::endl;
			return _currentStates[(size_t)code];
		}
		bool	isReleased( TEnum code, const char* desc ) {
			size_t c = (size_t)code;
			if (_shortcuts[c] == nullptr)
				_shortcuts[c] = desc;
			else if (_shortcuts[c] != desc)
				SIBR_ERR << "shortcut already used (current: '"
				<< _shortcuts[c] << "', conflict with '" << desc << "')"
				<< std::endl;
			return _lastStates[(size_t)code] \
				&& !_currentStates[(size_t)code];
		}

		void	press( TEnum code ) {
			_currentStates[(size_t)code] = true;
		}
		void	release( TEnum code ) {
			_currentStates[(size_t)code] = false;
			_lastStates[(size_t)code] = true;
		}
		void	silent( TEnum code ) {
			_currentStates[(size_t)code] = \
				_lastStates[(size_t)code] = false;
		}

		void	clearStates( void ) {
			std::fill(_currentStates.begin(), _currentStates.end(), false);
			std::fill(_lastStates.begin(), _lastStates.end(), false);
		}
		void	swapStates( void ) {
			_lastStates = _currentStates;
			//std::swap(_lastStates, _currentStates);
			//std::fill(_currentStates.begin(), _currentStates.end(), false);
		}

		int getNumActivated( void ) const {
			int n=0;
			for(int i=0; i<TNbState; ++i){
				n += (int)_currentStates[i];
			}
			return n;
		}

	private:
		std::array<bool, TNbState>			_currentStates;
		std::array<bool, TNbState>			_lastStates;
		std::array<const char*, TNbState>	_shortcuts; // test tmp
	};

	/**
	\ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Input
	{
	public:
		typedef InputState<Key::count, Key::Code>		Keyboard;
		typedef InputState<Mouse::count, Mouse::Code>	MouseButton;

	public:
		// Not a single (you can make copies/edits them)
		static Input&	global( void );

		static Input subInput(const sibr::Input & global, const sibr::Viewport & viewport, const bool mouseOutsideDisablesKeyboard = true);

		bool isInsideViewport(const sibr::Viewport & viewport) const;

		static void		poll( void );

		const Keyboard&	key( void ) const {
			return _keyboard;
		}
		Keyboard&	key( void ) {
			return _keyboard;
		}

		const MouseButton&	mouseButton( void ) const {
			return _mouseButton;
		}
		MouseButton&	mouseButton( void ) {
			return _mouseButton;
		}

		const Vector2i&	mousePosition( void ) const {
			return _mousePos;
		}
		void mousePosition( Vector2i mousePos ) {
			_mousePos = mousePos;
		}
		Vector2i mouseDeltaPosition( void ) const {
			return _mousePrevPos-_mousePos;
		}
		
		// return 0, 1, 2, ... or 9 if corresponding key pressed (if several, the tinest), -1 otherwise
		int pressedNumber() const {
			static const std::vector<sibr::Key::Code> keys = {
				Key::Num0, Key::Num1, Key::Num2, Key::Num3, Key::Num4,
				Key::Num5, Key::Num6, Key::Num7, Key::Num8, Key::Num9
			};
		
			for (int i = 0; i < 10; ++i){
				if (_keyboard.isPressed(keys[i]) ){
					return i;
				}
			}
			return -1;
		}

		void swapStates( void ) {
			key().swapStates();
			mouseButton().swapStates();
			_mousePrevPos = _mousePos;
			_mouseScroll = 0.0;
		}

		double			mouseScroll( void ) const {
			return _mouseScroll;
		}
		void			mouseScroll(double v) {
			_mouseScroll = v;
		}

		bool empty() const {
			return _empty;
		}

	private:

		Keyboard			_keyboard;
		MouseButton			_mouseButton;

		Vector2i			_mousePos = {0, 0};
		Vector2i			_mousePrevPos = { 0, 0 };
		double				_mouseScroll = 0.0;
		bool				_empty = true;

	};

	///// DEFINITIONS /////


} // namespace sibr

#endif // __SIBR_GRAPHICS_INPUT_HPP__
