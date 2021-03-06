#include "SceneManager.h"

void SceneManager::addMesh(Mesh* mesh)
{
	this->sceneMeshes.push_back(mesh);
	mesh->vectorIndex = this->sceneMeshes.size() - 1;
}

void SceneManager::removeMesh(Mesh* mesh)
{
	int index = mesh->vectorIndex;
	std::swap(sceneMeshes.at(sceneMeshes.size() - 1), sceneMeshes.at(mesh->vectorIndex));

	sceneMeshes.at(index)->vectorIndex = index; //Update the moved mesh's index in the array.

	delete sceneMeshes.at(sceneMeshes.size() - 1); //Delete the object
	this->sceneMeshes.pop_back(); //Clear the pointer from vector
}

void SceneManager::addAnimatedMesh(AnimatedMesh* animMesh)
{
	this->animatedMeshes.push_back(animMesh);
	animMesh->vectorIndex = this->animatedMeshes.size() - 1;
	//std::cout << "Index " << animMesh->vectorIndex << std::endl;
}

void SceneManager::removeAnimatedMesh(AnimatedMesh* animMesh)
{
	AnimatedMesh* tempPtr = nullptr;

	int index = animMesh->vectorIndex;
	tempPtr = animatedMeshes.at(animatedMeshes.size() - 1); //Save the back one
	animatedMeshes.at(animatedMeshes.size() - 1) = animatedMeshes.at(index); //Replace back with the one to be deleted.
	animatedMeshes.at(index) = tempPtr; //Replace index with what was at the back.
	animatedMeshes.at(index)->vectorIndex = index; //Update its index.

	//Swap completed.
	delete this->animatedMeshes.at(animatedMeshes.size() - 1); //Delete the object
	this->animatedMeshes.pop_back(); //Clear the pointer from vector
}

void SceneManager::addLight(Light* light)
{
	this->lights.push_back(light);
	light->vectorIndex = this->lights.size() - 1;
}

void SceneManager::removeLight(Light* light)
{
	//if (light != nullptr)
	//{
	int index = light->vectorIndex;

	if (lights.size() > 0)
		std::swap(lights.at(lights.size() - 1), lights.at(light->vectorIndex));

	lights.at(index)->vectorIndex = index; //Update the moved mesh's index in the array.

	//removeMesh(lights.at(lights.size() - 1)->lightVolume);

	delete lights.at(lights.size() - 1); //Delete the object
	this->lights.pop_back(); //Clear the pointer from vector
//}
}

void SceneManager::addTransparentObject(Mesh* mesh)
{
	this->transparentSceneObjects.push_back(mesh);
	mesh->vectorIndex = this->transparentSceneObjects.size() - 1;
}

void SceneManager::removeTransparentObject(Mesh* mesh)
{
	int index = mesh->vectorIndex;
	std::swap(transparentSceneObjects.at(transparentSceneObjects.size() - 1), transparentSceneObjects.at(mesh->vectorIndex));

	transparentSceneObjects.at(index)->vectorIndex = index; //Update the moved mesh's index in the array.

	delete transparentSceneObjects.at(transparentSceneObjects.size() - 1); //Delete the object
	this->transparentSceneObjects.pop_back(); //Clear the pointer from vector
}

void SceneManager::addEnemy(Enemy* enemy)
{
	this->enemies.push_back(enemy);
	enemy->vectorIndex = this->enemies.size() - 1;
}

void SceneManager::removeEnemy(Enemy* enemy)
{
	Enemy* tempPtr = nullptr;

	int index = enemy->vectorIndex;
	tempPtr = enemies.at(enemies.size() - 1); //Save the back one
	enemies.at(enemies.size() - 1) = enemies.at(index); //Replace back with the one to be deleted.
	enemies.at(index) = tempPtr; //Replace index with what was at the back.
	enemies.at(index)->vectorIndex = index; //Update its index.
	//Swap completed.


	removeAnimatedMesh(enemies.at(enemies.size() - 1)->model);
	removeLight(enemies.at(enemies.size() - 1)->light);

	delete enemies.at(enemies.size() - 1); //Delete the object
	this->enemies.pop_back(); //Clear the pointer from vector
}

void SceneManager::addParticle(Particle* particle)
{
	this->particles.push_back(particle);
	particle->vectorIndex = this->particles.size() - 1;
}

void SceneManager::removeParticle(Particle* particle)
{
	Particle* tempPtr = nullptr;

	int index = particle->vectorIndex;
	tempPtr = particles.at(particles.size() - 1); //Save the back one
	particles.at(particles.size() - 1) = particles.at(index); //Replace back with the one to be deleted.
	particles.at(index) = tempPtr; //Replace index with what was at the back.
	particles.at(index)->vectorIndex = index; //Update its index.

	//Swap completed.
	delete this->particles.at(particles.size() - 1); //Delete the object
	this->particles.pop_back(); //Clear the pointer from vector
}

void SceneManager::addHeart(Heartdrop* drop)
{
	this->hearts.push_back(drop);
	drop->vectorIndex = this->hearts.size() - 1;
}

void SceneManager::removeHeart(Heartdrop* drop)
{
	Heartdrop* tempPtr = nullptr;

	int index = drop->vectorIndex;
	tempPtr = hearts.at(hearts.size() - 1); //Save the back one
	hearts.at(hearts.size() - 1) = hearts.at(index); //Replace back with the one to be deleted.
	hearts.at(index) = tempPtr; //Replace index with what was at the back.
	hearts.at(index)->vectorIndex = index; //Update its index.
	drop->model->vertexBuffer = nullptr;

	removeTransparentObject(drop->model);

	//Swap completed.
	delete this->hearts.at(hearts.size() - 1); //Delete the object
	this->hearts.pop_back(); //Clear the pointer from vector
}

void SceneManager::addPoint(Pointdrop* drop)
{
	this->points.push_back(drop);
	drop->vectorIndex = this->points.size() - 1;
}

void SceneManager::removePoint(Pointdrop* drop)
{
	Pointdrop* tempPtr = nullptr;

	int index = drop->vectorIndex;
	tempPtr = points.at(points.size() - 1); //Save the back one
	points.at(points.size() - 1) = points.at(index); //Replace back with the one to be deleted.
	points.at(index) = tempPtr; //Replace index with what was at the back.
	points.at(index)->vectorIndex = index; //Update its index.
	drop->model->vertexBuffer = nullptr;

	removeTransparentObject(drop->model);

	//Swap completed.
	delete this->points.at(points.size() - 1); //Delete the object
	this->points.pop_back(); //Clear the pointer from vector
}

void SceneManager::addPlatform(MovingPlatform* platform)
{
	this->movingPlatforms.push_back(platform);
	platform->vectorIndex = this->sceneMeshes.size() - 1;
}

void SceneManager::removePlatform(MovingPlatform* platform)
{
	MovingPlatform* tempPtr = nullptr;

	int index = platform->vectorIndex;
	tempPtr = movingPlatforms.at(movingPlatforms.size() - 1); //Save the back one
	movingPlatforms.at(movingPlatforms.size() - 1) = movingPlatforms.at(index); //Replace back with the one to be deleted.
	movingPlatforms.at(index) = tempPtr; //Replace index with what was at the back.
	movingPlatforms.at(index)->vectorIndex = index; //Update its index.
	//Swap completed.
	removeMesh(movingPlatforms.at(movingPlatforms.size() - 1)->platform);

	delete movingPlatforms.at(movingPlatforms.size() - 1); //Delete the object
	this->movingPlatforms.pop_back(); //Clear the pointer from vector
}

void SceneManager::addSpike(Spike* spike)
{
	this->spikes.push_back(spike);
	spike->vectorIndex = this->sceneMeshes.size() - 1;
}

void SceneManager::removeSpike(Spike* spike)
{
	Spike* tempPtr = nullptr;

	int index = spike->vectorIndex;
	tempPtr = spikes.at(spikes.size() - 1); //Save the back one
	spikes.at(spikes.size() - 1) = spikes.at(index); //Replace back with the one to be deleted.
	spikes.at(index) = tempPtr; //Replace index with what was at the back.
	spikes.at(index)->vectorIndex = index; //Update its index.
	//Swap completed.
	removeMesh(spikes.at(spikes.size() - 1)->model);

	delete spikes.at(spikes.size() - 1); //Delete the object
	this->spikes.pop_back(); //Clear the pointer from vector
}

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	for (int i = 0; i < sceneMeshes.size(); i++)
		delete sceneMeshes.at(i);
	sceneMeshes.clear();

	for (int i = 0; i < animatedMeshes.size(); i++)
		delete animatedMeshes.at(i);
	animatedMeshes.clear();

	for (int i = 0; i < lights.size(); i++)
		delete lights.at(i);
	lights.clear();

	for (int i = 0; i < transparentSceneObjects.size(); i++)
		delete transparentSceneObjects.at(i);
	transparentSceneObjects.clear();

	for (int i = 0; i < enemies.size(); i++)
		delete enemies.at(i);
	enemies.clear();

	for (int i = 0; i < particles.size(); i++)
		delete particles.at(i);
	particles.clear();

	for (int i = 0; i < hearts.size(); i++)
		delete hearts.at(i);
	hearts.clear();

	for (int i = 0; i < points.size(); i++)
		delete points.at(i);
	points.clear();
}

