#pragma once
#include "yse.hpp"
#include "Player.h"
#include "Enemy.h"
#include "Mesh.h"
#include "Particle.h"
#include "Heartdrop.h"

class SceneManager
{
public:
	Mesh* masterHeart;

	std::vector<Mesh*> sceneMeshes;
	std::vector<AnimatedMesh*> animatedMeshes;
	std::vector<Light*> lights;
	std::vector<Mesh*> transparentSceneObjects;
	std::vector<Enemy*> enemies;
	std::vector<Particle*> particles;
	std::vector<Heartdrop*> hearts;

	void addMesh(Mesh* mesh);
	void removeMesh(Mesh* mesh);

	void addAnimatedMesh(AnimatedMesh* animMesh);
	void removeAnimatedMesh(AnimatedMesh* animMesh);

	void addLight(Light* light);
	void removeLight(Light* light);

	void addTransparentObject(Mesh* mesh);
	void removeTransparentObject(Mesh* mesh);

	void addEnemy(Enemy* enemy);
	void removeEnemy(Enemy* enemy);

	void addParticle(Particle* particle);
	void removeParticle(Particle* particle);

	void addHeart(Heartdrop* drop);
	void removeHeart(Heartdrop* drop);

	SceneManager();
	~SceneManager();

private:

};