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
		vertices[0].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

		vertices[1].position = glm::vec3(scale, scale, 0.0f);
		vertices[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[1].uv = glm::vec2(1.0f, 1.0f);
		vertices[1].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

		vertices[2].position = glm::vec3(-scale, -scale, 0.0f);
		vertices[2].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[2].uv = glm::vec2(0.0f, 0.0f);
		vertices[2].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

		vertices[3].position = glm::vec3(-scale, -scale, 0.0f);
		vertices[3].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[3].uv = glm::vec2(0.0f, 0.0f);
		vertices[3].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

		vertices[4].position = glm::vec3(scale, scale, 0.0f);
		vertices[4].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[4].uv = glm::vec2(1.0f, 1.0f);
		vertices[4].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

		vertices[5].position = glm::vec3(scale, -scale, 0.0f);
		vertices[5].normal = glm::vec3(0.0f, 0.0f, 1.0f);
		vertices[5].uv = glm::vec2(1.0f, 0.0f);
		vertices[5].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

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

		for (int i = 0; i < 36; i += 3)
		{
			vertices[i].normal = glm::cross(vertices[i + 2].position - vertices[i].position, 
				vertices[i + 1].position - vertices[i].position);
			vertices[i].normal = glm::normalize(vertices[i].normal);
			vertices[i + 1].normal = vertices[i].normal;
			vertices[i + 2].normal = vertices[i].normal;
		}

		for (int i = 0; i < 36; i += 6)
		{
			vertices[i].uv.x = 0.0;
			vertices[i].uv.y = 1.0;

			vertices[i + 1].uv.x = 0.0;
			vertices[i + 1].uv.y = 0.0;
			
			vertices[i + 2].uv.x = 1.0;
			vertices[i + 2].uv.y = 0.0;
			
			vertices[i + 3].uv.x = 1.0;
			vertices[i + 3].uv.y = 0.0;
			
			vertices[i + 4].uv.x = 1.0;
			vertices[i + 4].uv.y = 1.0;

			vertices[i + 5].uv.x = 0.0;
			vertices[i + 5].uv.y = 1.0;
		}

		for (int i = 0; i < 36; i += 6)
		{
			glm::vec3 tangent1, tangent2;

			glm::vec3 edge1 = vertices[i + 1].position - vertices[i].position;
			glm::vec3 edge2 = vertices[i + 2].position - vertices[i].position;
			glm::vec2 deltaUV1 = vertices[i + 1].uv - vertices[i].uv;
			glm::vec2 deltaUV2 = vertices[i + 2].uv - vertices[i].uv;

			GLfloat f = 1.0f / (deltaUV1.x *deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			edge1 = vertices[i + 2].position - vertices[i].position;
			edge2 = vertices[i + 4].position - vertices[i].position;
			deltaUV1 = vertices[i + 2].uv - vertices[i].uv;
			deltaUV2 = vertices[i + 4].uv - vertices[i].uv;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent2 = glm::normalize(tangent2);

			vertices[i].tangent = tangent1;
			vertices[i + 1].tangent = tangent1;
			vertices[i + 2].tangent = tangent1;

			vertices[i + 3].tangent = tangent2;
			vertices[i + 4].tangent = tangent2;
			vertices[i + 5].tangent = tangent2;
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