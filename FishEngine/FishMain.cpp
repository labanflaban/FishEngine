#include "Engine.h"
#include "DXHandler.h"

/*
Authors: Christian Leo, David Olsson, Filip Michael, Ghazi Hicheri, Isak Zimmerman & Pontuz Aronsson
Start date: 7/04/2020
Produced for an assignment in the course DV 1572/UD 1446 at Blekinge Institute of Technology.

*/

int WinMain(HINSTANCE startInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DxHandler::hInstance = startInstance;

	Engine engine;
	engine.initialSetup(); //Will set up all the DXStuff, and quite literally everything else
	engine.engineLoop(); //Starts the rendering loop

	return 0;
};