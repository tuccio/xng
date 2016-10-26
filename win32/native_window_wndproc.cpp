#include <xng/os/native_window.hpp>
#include <xng/os/raw_input.hpp>

#include <windowsx.h>

using namespace xng;
using namespace xng::os;
using namespace xng::math;

LRESULT CALLBACK native_window::wndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_DESTROY:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_destroy, wnd);
		break;
	}

	case WM_SIZE:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_resize, wnd, wnd->get_window_size(), wnd->get_client_size());
		return 0;
	}

	case WM_SETFOCUS:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_focus, wnd);
		break;
	}

	case WM_KILLFOCUS:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_unfocus, wnd);
		break;
	}

	case WM_MOUSEMOVE:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_move, wnd, uint2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;
	}

	case WM_INPUT:
	{
		RAWINPUT rawInput;
		UINT size = sizeof(RAWINPUT);
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &rawInput, &size, sizeof(RAWINPUTHEADER));

		switch (rawInput.header.dwType)
		{

		case RIM_TYPEKEYBOARD:
		{
			xng_keyboard_key key;
			bool keyUp;
			translate_keyboard_key(rawInput.data.keyboard, key, keyUp);

			if (key != XNG_KEYBOARD_UNKNOWN)
			{
				native_window * wnd = get_window_object(hWnd);

				if (keyUp)
				{
					wnd->notify(&os::native_window_observer::on_keyboard_key_up, wnd, key);
				}
				else
				{
					wnd->notify(&os::native_window_observer::on_keyboard_key_down, wnd, key);
				}
			}
			break;
		}
		
		// TODO: Handle raw input movement
		case RIM_TYPEMOUSE:
		{
			native_window * wnd = get_window_object(hWnd);

			/*static POINT lastPosition;
			
			static struct __initLastPos {
				__initLastPos(void) {
					GetCursorPos(&lastPosition);
				}
			} initLastPos;
			
			POINT newPosition;
			POINT screenPosition;
			bool  mouseMoved = false;
			BOOL  mouseOnScreen;


			if (rawInput.data.mouse.usFlags == MOUSE_MOVE_RELATIVE &&
				(rawInput.data.mouse.lLastX != 0 ||
				rawInput.data.mouse.lLastY != 0))
			{
				screenPosition = newPosition = POINT{ lastPosition.x + rawInput.data.mouse.lLastX, lastPosition.y + rawInput.data.mouse.lLastY };
				mouseOnScreen  = ScreenToClient(hWnd, &screenPosition);
				mouseMoved     = true;
			}
			else if ((rawInput.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) != 0)
			{
				screenPosition = newPosition = POINT{ rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY };
				mouseOnScreen  = ScreenToClient(hWnd, &screenPosition);
				mouseMoved     = true;
			}*/

			// Mouse keys

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_1);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_1);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_2);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_2);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_3);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_3);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_4);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_4);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_5);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			{
				wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_5);
			}

			if (rawInput.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				wnd->notify(&os::native_window_observer::on_mouse_wheel, wnd, ((SHORT) rawInput.data.mouse.usButtonData) / WHEEL_DELTA);
			}

			// Notify movement

			/*if (mouseMoved)
			{
				lastPosition = newPosition;

				if (mouseOnScreen)
				{
					wnd->notify(&os::native_window_observer::on_mouse_move, wnd, reinterpret_cast<const uint2&>(screenPosition));
				}
			}*/
			break;
		}
		}

		break;
	}

	case WM_LBUTTONDOWN:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_1);
		break;
	}

	case WM_LBUTTONUP:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_1);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_2);
		break;
	}

	case WM_RBUTTONUP:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_2);
		break;
	}

	case WM_MBUTTONDOWN:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_key_down, wnd, XNG_MOUSE_BUTTON_3);
		break;
	}

	case WM_MBUTTONUP:
	{
		native_window * wnd = get_window_object(hWnd);
		wnd->notify(&os::native_window_observer::on_mouse_key_up, wnd, XNG_MOUSE_BUTTON_3);
		break;
	}

	case WM_MOUSEWHEEL:
	{
		native_window * wnd = get_window_object(hWnd);
		int32_t wheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		wnd->notify(&os::native_window_observer::on_mouse_wheel, wnd, wheel);
		break;
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}