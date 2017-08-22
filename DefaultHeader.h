#pragma once

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<limits>
#include<random>
#include<opencv2\core.hpp>
#include<opencv2\opencv.hpp>
#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ifstream;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "assimp.lib")