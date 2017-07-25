#pragma once
#include <vector>
#include "imathvec.h"
#include "glut.h"
#include<iostream>
#include<fstream>
#include<string>
class Noise
{
public:
	Noise();
	int p[512];
	//int permutation[];
	void printShit();
	double perlinMarble(float x, float y);
	double perlinMultiscale(float x, float y);
	double noise(double x, double y, double z);
	static double fade(double t);
	static double lerp(double t, double a, double b);
	static double grad(int hash, double x, double y, double z);
};

