#include "InputHandler.h"
float2 InputHandler::lastMousePos;
LPDIRECTINPUT8 InputHandler::DirectInput;
DIMOUSESTATE InputHandler::lastMouseState;
IDirectInputDevice8* InputHandler::DIMouse = nullptr;
Camera* InputHandler::camera = nullptr;
bool InputHandler::windowClosed = false;
using namespace DirectX;

std::unique_ptr<DirectX::Mouse> InputHandler::mouse = nullptr;
std::unique_ptr<DirectX::Keyboard> InputHandler::keyboard = nullptr;

InputHandler::InputHandler(HWND& primaryWindow, Camera* camera)
{

	keyboard = std::make_unique<DirectX::Keyboard>();
	mouse = std::make_unique<DirectX::Mouse>();

	assert(keyboard != nullptr);
	mouse->SetWindow(primaryWindow);

	HRESULT createInputSucc  = DirectInput8Create(DxHandler::hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&DirectInput,
        NULL); 
	assert(SUCCEEDED(createInputSucc));

	HRESULT CreateDeviceSucc  = DirectInput->CreateDevice(GUID_SysMouse,
    &DIMouse,
    NULL);
	assert(SUCCEEDED(CreateDeviceSucc));

	HRESULT dataFormatMouseSucc = DIMouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(dataFormatMouseSucc));
	HRESULT setCoopSucc = DIMouse->SetCooperativeLevel(primaryWindow, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	assert(SUCCEEDED(setCoopSucc));

	this->camera = camera;

	this->primaryWindow = &primaryWindow;
	assert(primaryWindow != nullptr);
}

InputHandler::InputHandler()
{
	
}

void InputHandler::handleInput()
{
}

float InputHandler::getMousePosX()
{
	POINT point;
	if (GetCursorPos(&point))
	{
		RECT rect;
		//GetClientRect(*primaryWindow, &rect);

		ScreenToClient(*(this->primaryWindow), &point);

		return point.x;
	}
}

float InputHandler::getMousePosY()
{
	POINT point;
	if (GetCursorPos(&point))
	{	
		RECT rect;
		//GetClientRect(*primaryWindow, &rect);

		ScreenToClient(*(this->primaryWindow), &point);

		return point.y;
	}
}

bool InputHandler::isKeyDown(int keyCode)
{

	if(GetAsyncKeyState(keyCode) == 0) //Least bit is set
	{
		return false;
	}
	else
	{
		return true;
	}
}

LRESULT InputHandler::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_CREATE:

		break;
	case WM_DESTROY: //Destroy window
		//PostQuitMessage(0);
		break;
	case WM_CLOSE:
		windowClosed = true;
		break;
	case VK_ESCAPE:
		//PostQuitMessage(0);
		break;
	case WM_KEYDOWN: //When a key is pushed down.
		break;
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}