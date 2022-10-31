#pragma once

class aiNode;
class aiMesh;
class Texture;
class GameObject;

namespace ModelImporter
{
	void Import(const char* fullPath, char* buffer, int bSize, GameObject* objRoot);
	void NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& sceneMeshes, aiNode* node, GameObject* objParent, const char* holderName);
	void PopulateTransform(GameObject* child, aiNode* node);
}