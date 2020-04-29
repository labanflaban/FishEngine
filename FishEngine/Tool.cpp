#include "Tool.h"

Tool::Tool()
{
	slapSound.create("./Sounds/slap.wav");
	slapSound.setVolume(0.1);
	assert(slapSound.isValid());
}

void Tool::throwHook(Tool* hook)
{
	std::cout << inputhandler.getMousePosX() << std::endl;
}
