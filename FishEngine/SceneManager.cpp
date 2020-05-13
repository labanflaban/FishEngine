#include "SceneManager.h"

void SceneManager::addMesh(Mesh* mesh)
{
	this->sceneMeshes.push_back(mesh);
	mesh->vectorIndex = this->sceneMeshes.size()-1;
}

void SceneManager::removeMesh(Mesh* mesh)
{
	int index = mesh->vectorIndex;
	std::swap(sceneMeshes.at(sceneMeshes.size()-1), sceneMeshes.at(mesh->vectorIndex));

	sceneMeshes.at(index)->vectorIndex = index; //Update the moved mesh's index in the array.

	delete sceneMeshes.at(sceneMeshes.size()-1); //Delete the object
	this->sceneMeshes.pop_back(); //Clear the pointer from vector
}

void SceneManager::addAnimatedMesh(AnimatedMesh* animMesh)
{
	this->animatedMeshes.push_back(animMesh);
	animMesh->vectorIndex = this->sceneMeshes.size();
}

void SceneManager::removeAnimatedMesh(AnimatedMesh* animMesh)
{
	int index = animMesh->vectorIndex;
	std::swap(animatedMeshes.at(animatedMeshes.size() - 1), animatedMeshes.at(animMesh->vectorIndex));

	animatedMeshes.at(index)->vectorIndex = index; //Update the moved mesh's index in the array.

	delete animatedMeshes.at(animatedMeshes.size() - 1); //Delete the object
	this->animatedMeshes.pop_back(); //Clear the pointer from vector
}

void SceneManager::addLight(Light* light)
{
	this->lights.push_back(light);
	light->vectorIndex = this->lights.size()-1;
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
}

void SceneManager::removeTransparentObject(Mesh* mesh)
{
}

void SceneManager::addEnemy(Enemy* enemy)
{
	this->enemies.push_back(enemy);
	enemy->vectorIndex = this->enemies.size()-1;
}

void SceneManager::removeEnemy(Enemy* enemy)
{
	Enemy* tempPtr = nullptr;

	int index = enemy->vectorIndex;
	std::swap(enemies.at(enemies.size()-1), enemies.at(index));

	enemies.at(index)->vectorIndex = index; //Update the moved mesh's index in the array.

	removeMesh(enemies.at(enemies.size()-1)->model);
	removeLight(enemies.at(enemies.size()-1)->light);
	
	delete enemies.at(enemies.size()-1); //Delete the object
	this->enemies.pop_back(); //Clear the pointer from vector
}

void SceneManager::addParticle(Particle* particle)
{
}

void SceneManager::removeParticle(Particle* particle)
{
}

