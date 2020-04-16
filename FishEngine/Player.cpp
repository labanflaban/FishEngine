#include "Player.h"

Player::Player()
{
	jumpSound.create("./Sounds/Ah.wav");
	jumpSound.setVolume(0.1);
	assert(jumpSound.isValid());
}
