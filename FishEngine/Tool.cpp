#include "Tool.h"

Tool::Tool()
{
	slapSound.create("./Sounds/slap.wav");
	slapSound.setVolume(0.1);
	assert(slapSound.isValid());
}