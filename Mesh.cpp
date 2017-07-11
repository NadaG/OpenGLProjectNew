#include "Mesh.h"

Mesh::Mesh()
{
	vertices = new Vertex[60000];
}

Mesh::Mesh(MeshType meshType) 
{
	vertices = new Vertex[60000];
	LoadMesh(meshType);
}

void Mesh::LoadMesh(const MeshType& meshType)
{
	switch (meshType)
	{
	case TORUS:
		LoadMesh("torus.obj");
		drawMode = GL_TRIANGLES;
		break;
	case SPHERE:
		LoadMesh("sphereObj.obj");
		drawMode = GL_TRIANGLES;
		break;

	case QUAD:
		vertices[0].position = glm::vec3(-5.0f, 5.0f, 0.0f);
		vertices[0].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		
		vertices[1].position = glm::vec3(5.0f, 5.0f, 0.0f);
		vertices[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		
		vertices[2].position = glm::vec3(-5.0f, -5.0f, 0.0f);
		vertices[2].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		
		vertices[3].position = glm::vec3(-5.0f, -5.0f, 0.0f);
		vertices[3].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		
		vertices[4].position = glm::vec3(5.0f, 5.0f, 0.0f);
		vertices[4].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		
		vertices[5].position = glm::vec3(5.0f, -5.0f, 0.0f);
		vertices[5].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		
		vertexNum = 6;

		drawMode = GL_TRIANGLES;
		break;
	default:
		break;
	}
}

void Mesh::LoadMesh(const string& fileName)
{
	ObjLoader objloader;
	objloader.LoadObj(fileName);
	vertexNum = objloader.GenerateVertices(vertices);
}
