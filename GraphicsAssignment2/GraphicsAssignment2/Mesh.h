#pragma once

//Header Guards to limit duplicate includes
#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <fstream>
#include <math.h>
#include "imathvec.h"
#include "glut.h"
#include "Textures.h"
#include "Noise.h"

using namespace Imath;
using namespace std;

typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

struct Mesh
{
	vector<Vec3f> dot_vertex;
	vector<Vec3f> dot_normalPerFace;
	vector<Vec3f> dot_normalPerVertex;
	vector<Vec2f> dot_texture;

	vector<int> face_index_vertex;
	vector<int> face_index_normalPerFace;
	vector<int> face_index_normalPerVertex;
	vector<int> face_index_texture;
};

int StrToInt(const string &str);
vector<string> split_string(const string& str, const string& split_str);
Mesh* loadFile(const char* file);

Mesh* createPlane(int arena_width, int arena_depth, int arena_cell, int noiseType);
Mesh* createCylinder(int radius, int height);
Mesh* createCube();
Mesh* createDiamond();
Mesh* createDomePiece32();

const int PERVERTEX = 0;
const int PERFACE = 1;
void calculateNormalPerVertex(Mesh* m);
void calculateNormalPerFace(Mesh* m);
void calcCreaseAngle(Mesh* m, float angle);

GLuint meshToDisplayList(Mesh* m, int id, int line, int normals, GLuint texture, bool smooth);

const int X = 1;
const int C = 2;
const int o = 0;

bool checkCollision(float x, float z, bool collision);

const int mazeArray[20][20] = {

	{ X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X },
	{ X, o, o, o, X, o, o, o, X, o, X, o, X, o, o, o, X, o, o, X },
	{ X, o, o, o, o, o, X, o, o, o, o, o, o, o, o, o, X, o, o, X },
	{ X, o, o, X, X, X, C, X, X, X, X, o, X, X, X, o, o, o, o, X },
	{ X, o, o, o, o, o, X, o, o, o, X, o, X, X, o, X, o, o, o, X },
	{ X, o, X, o, C, o, o, o, X, o, X, o, o, X, o, o, o, C, o, X },
	{ X, X, X, o, C, o, X, o, X, o, X, X, X, X, X, X, o, o, o, X },
	{ X, o, X, o, o, o, o, o, o, o, o, o, o, o, o, X, o, C, o, X },
	{ X, o, X, o, o, o, X, X, o, C, o, C, o, X, o, o, o, o, o, X },
	{ X, o, o, o, X, X, X, o, o, o, o, o, o, o, o, C, o, o, X, X },
	{ X, o, X, X, X, o, X, X, o, C, o, C, o, X, o, X, o, o, o, X },
	{ X, o, o, o, X, o, o, o, o, o, o, o, o, o, o, X, o, X, o, X },
	{ X, X, X, o, X, o, X, X, o, o, X, X, o, X, X, X, o, o, o, X },
	{ X, o, o, o, o, o, o, o, o, X, o, o, o, o, o, X, o, X, o, X },
	{ X, o, X, X, o, X, X, X, X, X, o, X, X, X, o, X, o, X, o, X },
	{ X, o, o, o, o, o, o, o, o, X, o, o, o, X, o, o, o, X, o, X },
	{ X, X, o, X, o, C, o, X, o, X, o, X, o, X, o, X, o, o, o, X },
	{ X, o, o, X, o, X, o, C, o, o, o, X, o, o, o, o, X, X, X, X },
	{ X, o, o, X, o, o, o, o, o, X, o, X, X, o, X, o, o, o, o, X },
	{ X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X }
};
#endif