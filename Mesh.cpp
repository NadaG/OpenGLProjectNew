#include "Mesh.h"

Mesh::Mesh()
{
	vertices = new Vertex[1000000];
	indices = new GLuint[1000000];
}

Mesh::Mesh(MeshType meshType) 
{
	vertices = new Vertex[1000000];
	indices = new GLuint[1000000];
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
		float boxVertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };

		for (int i = 0; i < 36; i++)
		{
			vertices[i].position.x = boxVertices[i * 8];
			vertices[i].position.y = boxVertices[i * 8 + 1];
			vertices[i].position.z = boxVertices[i * 8 + 2];
			vertices[i].normal.x = boxVertices[i * 8 + 3];
			vertices[i].normal.y = boxVertices[i * 8 + 4];
			vertices[i].normal.z = boxVertices[i * 8 + 5];
			vertices[i].uv.x = boxVertices[i * 8 + 6];
			vertices[i].uv.y = boxVertices[i * 8 + 7];
		}

		// 삼각형의 위치 정보와 uv 정보를 활용해서 edge에 평행하는 tangent vector를 뽑아낼 수 있다.
		// bitangent vector 는 normal과 tangent vector를 cross 연산을 함으로써 구하고
		// 구해내 TBN을 행렬로 만들어서 world space의 좌표를 tangent space의 좌표로 바꿀 수 있다.
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
	case EARTH:
		LoadMesh("Earth/Earth.obj");
		drawMode = GL_TRIANGLES;
		break;

	case TREX:
	{
		// aiprocess_triangulate는 triangle 형태가 아닌 model load 할 때 triangle로 불러들이는 것
		// flipuvs는 y값은 flip하는 것
		// scene의 mMeshes에는 모든 mesh들이 저장되어 있다
		// scene은 mRootNode를 가지고 있고 각 노드에는 mesh가 있다
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile("Trex.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "Error::assimp::" << importer.GetErrorString() << endl;
		}
		else
		{
			unsigned long long int offset = 0;
			for (int i = 0; i < scene->mNumMeshes; i++)
			{
				// vertices지 positions가 아니라는 것을 주의할 것
				for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					vertices[j + offset].position.x = scene->mMeshes[i]->mVertices[j].x;
					vertices[j + offset].position.y = scene->mMeshes[i]->mVertices[j].y;
					vertices[j + offset].position.z = scene->mMeshes[i]->mVertices[j].z;

					if (scene->mMeshes[i]->HasNormals())
					{
						vertices[j + offset].normal.x = scene->mMeshes[i]->mNormals[j].x;
						vertices[j + offset].normal.y = scene->mMeshes[i]->mNormals[j].y;
						vertices[j + offset].normal.z = scene->mMeshes[i]->mNormals[j].z;
					}

					if (scene->mMeshes[i]->HasTextureCoords(j)) 
					{
						vertices[j + offset].uv.x = scene->mMeshes[i]->mTextureCoords[0][j].x;
						vertices[j + offset].uv.y = scene->mMeshes[i]->mTextureCoords[0][j].y;
					}
				}
				offset += scene->mMeshes[i]->mNumVertices;
			}
			vertexNum = offset;

			int indexOffset = 0;
			offset = 0;
			for (int i = 0; i < scene->mNumMeshes; i++)
			{
				for (int j = 0, jj = 0; j < scene->mMeshes[i]->mNumFaces; j++, jj += 3)
				{
					indices[jj + offset] = indexOffset + scene->mMeshes[i]->mFaces[j].mIndices[0];
					indices[jj + offset + 1] = indexOffset + scene->mMeshes[i]->mFaces[j].mIndices[1];
					indices[jj + offset + 2] = indexOffset + scene->mMeshes[i]->mFaces[j].mIndices[2];
				}
				offset += scene->mMeshes[i]->mNumFaces * 3;
				indexOffset += scene->mMeshes[i]->mNumVertices;
			}
			indexNum = offset;
		}
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