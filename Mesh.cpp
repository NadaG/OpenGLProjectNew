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
	float scale = 3.0f;
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
		vertices[0].position = glm::vec3(-scale, scale, 0.0f);
		vertices[0].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[0].uv = glm::vec2(0.0f, 1.0f);

		vertices[1].position = glm::vec3(scale, scale, 0.0f);
		vertices[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[1].uv = glm::vec2(1.0f, 1.0f);

		vertices[2].position = glm::vec3(-scale, -scale, 0.0f);
		vertices[2].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[2].uv = glm::vec2(0.0f, 0.0f);

		vertices[3].position = glm::vec3(-scale, -scale, 0.0f);
		vertices[3].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[3].uv = glm::vec2(0.0f, 0.0f);

		vertices[4].position = glm::vec3(scale, scale, 0.0f);
		vertices[4].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[4].uv = glm::vec2(1.0f, 1.0f);

		vertices[5].position = glm::vec3(scale, -scale, 0.0f);
		vertices[5].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[5].uv = glm::vec2(1.0f, 0.0f);

		vertexNum = 6;

		drawMode = GL_TRIANGLES;
		break;

	case BOX:
	{
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		for (int i = 0; i < 36; i++)
		{
			vertices[i].position.x = skyboxVertices[i * 3];
			vertices[i].position.y = skyboxVertices[i * 3 + 1];
			vertices[i].position.z = skyboxVertices[i * 3 + 2];
		}

		vertexNum = 36;
		drawMode = GL_TRIANGLES;
		break;
	}
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
