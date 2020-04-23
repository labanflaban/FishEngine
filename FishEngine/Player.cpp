#include "Player.h"
using namespace DirectX;

Player::Player()
{
	jumpSound.create("./Sounds/Ah.wav");
	jumpSound.setVolume(0.1);
	assert(jumpSound.isValid());
}
void Player::updatePlayer(Tool* tool)
{
	
	fishingRodPos = XMFLOAT3(model->getTranslation().x + 3, model->getTranslation().y+10, model->getTranslation().z);
	tool->model->setTranslation(fishingRodPos);
}