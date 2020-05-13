#include "InputHandler.h"
float2 InputHandler::lastMousePos;
LPDIRECTINPUT8 InputHandler::DirectInput;
DIMOUSESTATE InputHandler::lastMouseState;
IDirectInputDevice8* InputHandler::DIMouse = nullptr;
Camera* InputHandler::camera = nullptr;
using namespace DirectX;

std::unique_ptr<DirectX::Mouse> InputHandler::mouse = nullptr;
std::unique_ptr<DirectX::Keyboard> InputHandler::keyboard = nullptr;

InputHandler::InputHandler(HWND& primaryWindow, Camera* camera)
{

	keyboard = std::make_unique<DirectX::Keyboard>();
	mouse = std::make_unique<DirectX::Mouse>();

	assert(primaryWindow != nullptr);
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
}

InputHandler::InputHandler()
{
	
}

void InputHandler::handleInput()
{
}

float InputHandler::getMousePosX()
{
	if (GetCursorPos(&p))
	{
		float xPos = p.x;
		return xPos;
	}
}

float InputHandler::getMousePosY()
{
	if (GetCursorPos(&p))
	{	
		float yPos = p.y;
		return yPos;
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
		PostQuitMessage(0);
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN: //When a key is pushed down.

		if (wParam == 0x57) //W Button, virtual keycodes.
		{
			
		}
		if (wParam == 0x53) //S Button
		{
			
		}

		if (wParam == 0x41) //A Button
		{
			//camera->cameraPosition = camera->cameraPosition - DirectX::XMVectorSet(0.5, 0, 0, 0);
			//camera->cameraTarget = camera->cameraTarget - DirectX::XMVectorSet(0.5, 0, 0, 0);
			//std::cout << "MOVED LEFT" << std::endl;
		}

		if (wParam == 0x44) //D Button
		{
			//camera->cameraPosition = camera->cameraPosition + XMVectorSet(0.5, 0, 0, 0);
			//camera->cameraTarget = camera->cameraTarget + XMVectorSet(0.5, 0, 0, 0);
			//std::cout << "MOVED RIGHT" << std::endl;
		}

		if (wParam == VK_SPACE)
		{
			//Jump
			//playerModel->rigidBody->setPushVelocity(btVector3(0, 1, 0));
		}
			

		if (wParam == VK_ESCAPE)
			//exit(1);

		break;
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}