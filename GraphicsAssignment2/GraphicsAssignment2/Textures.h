#pragma once

//Header Guards to limit duplicate includes
#ifndef _TEXTURES_H_
#define _TEXTURES_H_

#include <stdio.h>
#include<iostream>
#include "glut.h"
#include <windows.h>
#include <wingdi.h>


using namespace std;

GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
void codedTexture(UINT textureArray[], int n);
void bmpTexture(UINT textureArray[], LPSTR file, int n);

const int M_CHROME    = 0;
const int M_PEARL     = 1;
const int M_JADE      = 2;
const int M_OBSIDIAN  = 3;

const int M_RED_PLASTIC = 4;
const int M_BLUE_PLASTIC = 5;

const int M_DIFFUSE = 100;
const int M_SPECULAR = 101;

void addMaterial(int material);
void addShading(int material, int mode);

#endif