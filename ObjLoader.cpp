#include "ObjLoader.h"

void ObjLoader::LoadObj(const string& fileName)
{
	inFile.open(fileName);
	bool haveToGetLine = true;

	string lineStr;
	while (!inFile.eof())
	{
		inFile.clear();
		if (haveToGetLine)
			getline(inFile, lineStr);
		else
			haveToGetLine = true;
		lineStr.push_back(' ');
		// v 불러오기
		if (lineStr[0] == 'v' && lineStr[1] == ' ')
		{
			int index = 0;
			while (true)
			{
				// v가 끝나거나 vt같은 것이 시작된다면
				if (lineStr.size() == 0)
					break;

				if (lineStr[0] == '#')
					break;

				if (lineStr[0] != 'v' || (lineStr[0] && lineStr[1] != ' '))
				{
					haveToGetLine = false;
					break;
				}

				ParsePositions(lineStr, index);

				// 다음 라인 얻기
				getline(inFile, lineStr);
				// 뒤에 공백 문자를 두어 구분하기
				lineStr.push_back(' ');
				index++;
			}
			// 얻은 라인의 개수 저장하기
			// 사실은 position 개수인데..
			vertexNum = index;
		}
		else if (lineStr[0] == 'v' && lineStr[1] == 'n')
		{
			int index = 0;
			while (true)
			{
				// v가 끝나거나 vt같은 것이 시작된다면
				if (lineStr.size() == 0)
					break;

				if (lineStr[0] == '#')
					break;

				// 시작이 vt가 아니라면
				if (lineStr[0] == 'v' && lineStr[1] == 't')
				{
					haveToGetLine = false;
					break;
				}

				// 시작이 vn이 아니라면
				if (!(lineStr[0] == 'v' && lineStr[1] == 'n'))
					break;

				ParseNormals(lineStr, index);

				// 다음 라인 얻기
				getline(inFile, lineStr);
				// 뒤에 공백 문자를 두어 구분하기
				lineStr.push_back(' ');
				index++;
			}
			// 얻은 라인의 개수 저장하기
			normalNum = index;
		}
		else if (lineStr[0] == 'v' && lineStr[1] == 't')
		{
			int index = 0;
			while (true)
			{
				// v가 끝나거나 vt같은 것이 시작된다면
				if (lineStr.size() == 0)
					break;

				if (lineStr[0] == '#')
					break;

				// 시작이 vt가 아니라면
				if (lineStr[0] == 'v' && lineStr[1] == 'n')
				{
					haveToGetLine = false;
					break;
				}

				if (!(lineStr[0] == 'v' && lineStr[1] == 't'))
					break;

				// parse uv....
				ParseUvs(lineStr, index);

				// 다음 라인 얻기
				getline(inFile, lineStr);
				// 뒤에 공백 문자를 두어 구분하기
				lineStr.push_back(' ');
				index++;
			}
			// 얻은 라인의 개수 저장하기
			uvNum = index;
		}
		// f 불러오기 시작
		else if (lineStr[0] == 'f' && lineStr[1] == ' ')
		{
			int index = 0;
			while (!inFile.eof())
			{
				if (lineStr.size() <= 2)
					break;

				if (lineStr[0] != 'f')
				{
					inFile.clear();
					getline(inFile, lineStr);
					continue;
				}

				if (ParseFace(lineStr, index))
					index++;
				else
					index += 2;

				inFile.clear();
				getline(inFile, lineStr);
				lineStr.push_back(' ');
			}

			// 마지막 라인일 경우
			if (inFile.eof())
			{
				if (ParseFace(lineStr, index))
					index++;
				else
					index += 2;

				inFile.clear();
				getline(inFile, lineStr);
				lineStr.push_back(' ');
				//index++;
			}

			faceNum = index;
		}
		else if (lineStr[0] == 'm')
		{
			if (lineStr.size() > 6)
			{
				if (lineStr.substr(0, 6) == "mtllib")
				{
					int from = lineStr.find(' ');
					lineStr = lineStr.substr(from);
					while (lineStr[0] == ' ')
					{
						from++;
						lineStr = lineStr.substr(1);
					}

					ParseMtl(lineStr);
				}
			}
		}
	}
}


void ObjLoader::ParsePositions(string line, int index)
{
	int now = 0;
	while (line.find(' ') != -1)
	{
		line = line.substr(line.find(' '));
		while (line[0] == ' ')
			line = line.substr(1);
		int to = line.find(' ');

		switch (now)
		{
		case 0:
			positions[index].x = stof(line.substr(0, to));
			break;
		case 1:
			positions[index].y = stof(line.substr(0, to));
			break;
		case 2:
			positions[index].z = stof(line.substr(0, to));
			break;
		default:
			break;
		}

		now++;
	}
}

void ObjLoader::ParseUvs(string line, int index)
{
	int now = 0;
	while (line.find(' ') != -1)
	{
		line = line.substr(line.find(' '));
		while (line[0] == ' ')
			line = line.substr(1);
		int to = line.find(' ');

		switch (now)
		{
		case 0:
			uvs[index].x = stof(line.substr(0, to));
			break;
		case 1:
			uvs[index].y = stof(line.substr(0, to));
			break;
		case 2:
			if (line.size() <= 1)
				break;

			uvs[index].z = stof(line.substr(0, to));
			break;
		default:
			break;
		}

		now++;
	}
}

void ObjLoader::ParseNormals(string line, int index)
{
	int now = 0;
	while (line.find(' ') != -1)
	{
		line = line.substr(line.find(' '));
		while (line[0] == ' ')
			line = line.substr(1);
		int to = line.find(' ');

		switch (now)
		{
		case 0:
			normals[index].x = stof(line.substr(0, to));
			break;
		case 1:
			normals[index].y = stof(line.substr(0, to));
			break;
		case 2:
			normals[index].z = stof(line.substr(0, to));
			break;
		default:
			break;
		}

		now++;
	}
}

bool ObjLoader::ParseFace(string line, int index)
{
	string uvLine;
	string normalLine;
	bool is3Face = true;
	int now = 0;
	while (line.find(' ') != -1)
	{
		int from = line.find(' ') + 1;
		line = line.substr(from);

		int to = line.find('/');
		uvLine = line.substr(to + 1);
		int uvTo = uvLine.find('/');
		normalLine = uvLine.substr(uvTo + 1);
		int normalTo = normalLine.find(' ');

		// obj 파일 인덱스는 1부터 시작하더라고...
		switch (now)
		{
		case 0:
			faces[index].vertexIndex.x = stoi(line.substr(0, to)) - 1;
			faces[index].uvIndex.x = stoi(uvLine.substr(0, uvTo)) - 1;
			faces[index].normalIndex.x = stoi(normalLine.substr(0, normalTo)) - 1;
			break;
		case 1:
			faces[index].vertexIndex.y = stoi(line.substr(0, to)) - 1;
			faces[index].uvIndex.y = stoi(uvLine.substr(0, uvTo)) - 1;
			faces[index].normalIndex.y = stoi(normalLine.substr(0, normalTo)) - 1;
			break;
		case 2:
			faces[index].vertexIndex.z = stoi(line.substr(0, to)) - 1;
			faces[index].uvIndex.z = stoi(uvLine.substr(0, uvTo)) - 1;
			faces[index].normalIndex.z = stoi(normalLine.substr(0, normalTo)) - 1;
			break;
		case 3:
			if (line.size() <= 2 || uvLine.size() <= 1 || normalLine.size() <= 1)
				break;

			is3Face = false;
			faces[index + 1].vertexIndex.x = faces[index].vertexIndex.z;
			faces[index + 1].uvIndex.x = faces[index].uvIndex.z;
			faces[index + 1].normalIndex.x = faces[index].normalIndex.z;

			faces[index + 1].vertexIndex.y = faces[index].vertexIndex.x;
			faces[index + 1].uvIndex.y = faces[index].uvIndex.x;
			faces[index + 1].normalIndex.y = faces[index].normalIndex.x;

			faces[index + 1].vertexIndex.z = stoi(line.substr(0, to)) - 1;
			faces[index + 1].uvIndex.z = stoi(uvLine.substr(0, uvTo)) - 1;
			faces[index + 1].normalIndex.z = stoi(normalLine.substr(0, normalTo)) - 1;
			break;
		default:
			break;
		}

		now++;
	}

	return is3Face;
}

void ObjLoader::ParseMtl(string fileName)
{
	mtlInFile.open(fileName);
	string lineStr;

	while (!mtlInFile.eof())
	{
		mtlInFile.clear();
		getline(mtlInFile, lineStr);
		lineStr.push_back(' ');

		if (lineStr[0] == '#')
			continue;

		if (lineStr.length() <= 1)
			continue;

		while (lineStr[0] == ' ' || lineStr[0] == '\t')
			lineStr = lineStr.substr(1);

		if (lineStr[0] == 'K')
		{
			ParseLightColor(lineStr, lineStr[1]);
		}
	}
}

void ObjLoader::ParseLightColor(string line, const char& mode)
{
	float colors[3];
	int now = 0;
	while (line.size() > 0)
	{
		line = line.substr(line.find(' ') + 1);
		while (line[0] == ' ')
			line = line.substr(1);

		while (true)
		{
			if (line.find(' ') <= 1)
				break;

			if (now >= 3)
				break;

			colors[now] = stof(line.substr(0, line.find(' ')));
			now++;
			line = line.substr(line.find(' '));
		}
	}

	switch (mode)
	{
	case 'a':
		ambientColor.x = colors[0];
		ambientColor.y = colors[1];
		ambientColor.z = colors[2];
		break;
	case 'd':
		diffuseColor.x = colors[0];
		diffuseColor.y = colors[1];
		diffuseColor.z = colors[2];
		break;
	case 's':
		specularColor.x = colors[0];
		specularColor.y = colors[1];
		specularColor.z = colors[2];
		break;
	default:
		break;
	}
}

const int& ObjLoader::GenerateVertices(Vertex* vertices)
{
	vertexNum = 0;
	for (int i = 0; i < faceNum; i++)
	{
		Vertex vertex;
		vertex.position = positions[(int)faces[i].vertexIndex.x];
		vertex.uv = uvs[(int)faces[i].uvIndex.x];
		vertex.normal = normals[(int)faces[i].normalIndex.x];
		vertices[i * 3] = vertex;

		vertex.position = positions[(int)faces[i].vertexIndex.y];
		vertex.uv = uvs[(int)faces[i].uvIndex.y];
		vertex.normal = normals[(int)faces[i].normalIndex.y];
		vertices[i * 3 + 1] = vertex;

		vertex.position = positions[(int)faces[i].vertexIndex.z];
		vertex.uv = uvs[(int)faces[i].uvIndex.z];
		vertex.normal = normals[(int)faces[i].normalIndex.z];
		vertices[i * 3 + 2] = vertex;

		vertexNum += 3;
	}

	return vertexNum;
}