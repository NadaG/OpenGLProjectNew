#include "CubeObject.h"

void CubeObject::Awake()
{
	this->Translate(glm::vec3(2.0f, 0.0f, 2.0f));
	this->Scale(glm::vec3(3.0f, 3.0f, 3.0f));
	mesh.LoadMesh(BOX);
}

void CubeObject::Start()
{
}

void CubeObject::Update()
{
}