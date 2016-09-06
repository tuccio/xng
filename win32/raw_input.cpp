#include <xng/os/raw_input.hpp>

namespace xng
{
	namespace os
	{
		void translate_keyboard_key(const RAWKEYBOARD & rawKeyboard, xng_keyboard_key & xngKey, bool & keyUp)
		{
			UINT virtualKey = rawKeyboard.VKey;
			UINT scanCode   = rawKeyboard.MakeCode;
			UINT flags      = rawKeyboard.Flags;

			xngKey = XNG_KEYBOARD_UNKNOWN;
			keyUp  = ((flags & RI_KEY_BREAK) != 0);

			if (virtualKey >= 'A' && virtualKey <= 'Z')
			{
				xngKey = static_cast<xng_keyboard_key>(XNG_KEYBOARD_A + (virtualKey - 'A'));
			}
			else if (virtualKey >= '0' && virtualKey <= '9')
			{
				xngKey = static_cast<xng_keyboard_key>(XNG_KEYBOARD_0 + (virtualKey - '0'));
			}
			else if (virtualKey >= VK_F1 && virtualKey <= VK_F24)
			{
				xngKey = static_cast<xng_keyboard_key>(XNG_KEYBOARD_F1 + (virtualKey - VK_F1));
			}
			else
			{
				// Source: https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
				

				if (virtualKey == 255)
				{
					// discard "fake keys" which are part of an escaped sequence
					return;
				}
				else if (virtualKey == VK_SHIFT)
				{
					// correct left-hand / right-hand SHIFT
					virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
				}
				else if (virtualKey == VK_NUMLOCK)
				{
					// correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
					scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
				}

				// e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
				// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
				const bool isE0 = ((flags & RI_KEY_E0) != 0);
				const bool isE1 = ((flags & RI_KEY_E1) != 0);

				if (isE1)
				{
					// for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
					// however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
					if (virtualKey == VK_PAUSE)
						scanCode = 0x45;
					else
						scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
				}

				switch (virtualKey)
				{
				case VK_TAB:
					xngKey = XNG_KEYBOARD_TAB;
					break;

				case VK_LSHIFT:
					xngKey = XNG_KEYBOARD_LEFT_CTRL;
					break;

				case VK_RSHIFT:
					xngKey = XNG_KEYBOARD_RIGHT_CTRL;
					break;

					// right-hand CONTROL and ALT have their e0 bit set
				case VK_CONTROL:
					xngKey = isE0 ? XNG_KEYBOARD_RIGHT_CTRL : XNG_KEYBOARD_LEFT_CTRL;
					break;

				case VK_MENU:
					xngKey = isE0 ? XNG_KEYBOARD_RIGHT_ALT : XNG_KEYBOARD_LEFT_ALT;
					break;

					// NUMPAD ENTER has its e0 bit set
				case VK_RETURN:
					xngKey = isE0 ? XNG_KEYBOARD_NUMPAD_ENTER : XNG_KEYBOARD_ENTER;
					break;

					// the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys will always have their e0 bit set, but the
					// corresponding keys on the NUMPAD will not.
				case VK_INSERT:
					xngKey = isE0 ? XNG_KEYBOARD_INSERT : XNG_KEYBOARD_NUMPAD_0;
					break;

				case VK_DELETE:
					xngKey = isE0 ? XNG_KEYBOARD_DELETE : XNG_KEYBOARD_NUMPAD_DECIMAL;
					break;

				case VK_HOME:
					xngKey = isE0 ? XNG_KEYBOARD_HOME : XNG_KEYBOARD_NUMPAD_7;
					break;

				case VK_END:
					xngKey = isE0 ? XNG_KEYBOARD_END : XNG_KEYBOARD_NUMPAD_1;
					break;

				case VK_PRIOR:
					xngKey = isE0 ? XNG_KEYBOARD_PRIOR : XNG_KEYBOARD_NUMPAD_9;
					break;

				case VK_NEXT:
					xngKey = isE0 ? XNG_KEYBOARD_NEXT : XNG_KEYBOARD_NUMPAD_3;
					break;

					// the standard arrow keys will always have their e0 bit set, but the
					// corresponding keys on the NUMPAD will not.
				case VK_LEFT:
					xngKey = isE0 ? XNG_KEYBOARD_LEFT_ARROW : XNG_KEYBOARD_NUMPAD_4;
					break;

				case VK_RIGHT:
					xngKey = isE0 ? XNG_KEYBOARD_RIGHT_ARROW : XNG_KEYBOARD_NUMPAD_6;
					break;

				case VK_UP:
					xngKey = isE0 ? XNG_KEYBOARD_UP_ARROW : XNG_KEYBOARD_NUMPAD_8;
					break;

				case VK_DOWN:
					xngKey = isE0 ? XNG_KEYBOARD_DOWN_ARROW : XNG_KEYBOARD_NUMPAD_2;
					break;

					// NUMPAD 5 doesn't have its e0 bit set
				case VK_CLEAR:
					if (!isE0)
						xngKey = XNG_KEYBOARD_NUMPAD_5;
					break;
				}
			}
		}
	}
}