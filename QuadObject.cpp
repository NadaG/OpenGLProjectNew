#include "QuadObject.h"

QuadObject::QuadObject(ShaderProgram shaderProgram)
{
	this->shaderProgram = shaderProgram;
}

void QuadObject::Awake()
{
	mesh.LoadMesh(QUAD);
}

void QuadObject::Start()
{
}

void QuadObject::Update()
{
}