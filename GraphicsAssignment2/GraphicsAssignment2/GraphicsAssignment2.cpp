#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "glut.h"
#include "Mesh.h"
#include "Noise.h"

int normalMode = 0; //PerFace = 0, Pervertex = 1;

//-SHADING MODES-
const int S_DIFFUSE = 0;
const int S_SPECULAR = 1;
const int S_DESIGN = 2;

int shadingMode = S_DESIGN;
//---------------

GLuint textureArray[8];

float angle = 0.0;
float x = 0.0f, y = 1.75f, z = 5.0f;
float lx = 0.0f, ly = 0.0f, lz = -1.0f;
float ratio = 1.0;
int width, height;
char msg[100];

//Mesh ID Numbers
GLuint PLANE, CUBE, CYLINDER, CHARIZARD, PLAYER, DOME;

// font
void renderBitmapString
(float x, float y, float z, void *font, char *string) {
	char *c;
	glRasterPos3f(x, y, z);   // fonts position
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(font, *c);
}

bool fire = false;
bool bullet = false;
float bulletPosX;
float bulletPosZ;
float bulletAngle;

/* CAMERA - Start */
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 1.0;
float x_angle = 0.0;
float y_angle = 0.0;

bool topDownView = false;
bool lightNav = false; //If false control camera, if true control light
bool firstPerson = false;
bool smoothTextures = false;
float lightAngle = 0;

bool playerCollisions = true;
float mazePositionX = 75;
float mazePositionZ = 925;

void mouse(int button, int state, int x, int y) {
	mouse_x = x;
	mouse_y = y;
	mouse_button = button;
}

void motion(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		if (lightNav) {
			lightAngle += (float(x - mouse_x) / (width*50)) *360.0;
		}
		else {
			y_angle += (float(x - mouse_x) / width) *360.0;
			x_angle += (float(y - mouse_y) / height)*360.0;
		}

	}
	if (mouse_button == GLUT_MIDDLE_BUTTON) {
		scale += (y - mouse_y) / 100.0;
		if (scale < 0.1) scale = 0.1;
		if (scale > 7)	scale = 7;
	}
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

/* CAMERA - End */


/* KEYBOARD CONTOLS - Start */
float playerX = -425;
float playerZ = 425;
float playerSpeed = 1;
float playerAngle = M_PI;
float playerCamY = 25;

float getReferenceX(float angle) {
	float ref = 0;

	if ((int) sin(angle) == 0) {
		ref = playerX;
	}
	else {
		ref = 500 * (int) sin(angle);
	}

	return ref;
}

float getReferenceZ(float angle) {
	float ref = 0;

	if ((int) cos(angle) == 0) {
		ref = playerZ;
	}
	else {
		ref = 500 * (int) cos(angle);
	}

	return ref;
}

void specialKeys(int c, int x, int y) {

	switch (c) {
	case GLUT_KEY_UP:
		if (!firstPerson) {
			if (!checkCollision(mazePositionX, mazePositionZ - 10, playerCollisions)) {
				playerZ -= playerSpeed;
				mazePositionZ -= playerSpeed;
			}
		}
		else {
			if (!checkCollision(mazePositionX + (10*playerSpeed*sin(playerAngle)), mazePositionZ + (10*playerSpeed*cos(playerAngle)), playerCollisions)) {
				playerX += playerSpeed*sin(playerAngle);
				playerZ += playerSpeed*cos(playerAngle);
				mazePositionX += playerSpeed*sin(playerAngle);
				mazePositionZ += playerSpeed*cos(playerAngle);
			}
		}
		break;
	case GLUT_KEY_LEFT:
		if (!firstPerson) {
			if (!checkCollision(mazePositionX - 10, mazePositionZ, playerCollisions)) {
				playerX -= playerSpeed;
				mazePositionX -= playerSpeed;
			}
		}
		break;
	case GLUT_KEY_DOWN:
		if (!firstPerson) {
			if (!checkCollision(mazePositionX, mazePositionZ + 10, playerCollisions)) {
				playerZ += playerSpeed;
				mazePositionZ += playerSpeed;
			}
		}
		else {
			if (!checkCollision(mazePositionX + (-10*playerSpeed*sin(playerAngle)), mazePositionZ + (-10*playerSpeed*cos(playerAngle)), playerCollisions)) {
				playerX += -playerSpeed*sin(playerAngle);
				playerZ += -playerSpeed*cos(playerAngle);
				mazePositionX += -playerSpeed*sin(playerAngle);
				mazePositionZ += -playerSpeed*cos(playerAngle);
			}
		}
		
		break;
	case GLUT_KEY_RIGHT:
		if (!firstPerson) {
			if (!checkCollision(mazePositionX, mazePositionZ - 10, playerCollisions)) {
				playerX += playerSpeed;
				mazePositionX += playerSpeed;
			}
		}
		break;
	}
}

void keyRelease(int c, int x, int y) {
	switch (c) {
	case GLUT_KEY_LEFT:
		if (firstPerson)
			playerAngle += M_PI/2;
		break;
	case GLUT_KEY_RIGHT:
		if (firstPerson)
			playerAngle -= M_PI / 2;
		break;
	}
	cout << (int) sin(playerAngle) << "\n";
}

void regularKeys(unsigned char c, int x, int y) {
	
	switch (c) {
	case 'a':
		if (firstPerson)
			playerCamY--;
		else if (!lightNav) {
			scale -= 0.1;
			if (scale < 0.1) scale = 0.1;
		}
		break;
	case 's':
		if (firstPerson)
			playerCamY++;

		else if (!lightNav) {
			scale += 0.1;
			if (scale > 7.0) scale = 7.0;
		}

		break;
	}
}

void regularKeysUp(unsigned char c, int x, int y) {
	
	switch (c) {
	
	case 32:
		if (firstPerson && !fire) {
			fire = true;
		}
		break;
	}

}
/* KEYBOARD CONTROLS - End*/

void myReshape(int w, int h) {
	width = w;
	height = h;
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
}

// init

Mesh* cube = createCube();
Mesh* can = createCylinder(25, 50);
Mesh* plane = createPlane(1050, 1050, 40, 0);
Mesh* planeA = createPlane(1050, 1050, 40, 1); //Multiscale
Mesh* planeB = createPlane(1050, 1050, 40, 2); //Marble
Mesh* charizard = loadFile("Charizard.obj");
Mesh* diamond = createDiamond();
Mesh* domePiece = createDomePiece32();

void myInit() {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	
	calculateNormalPerFace(cube);
	calculateNormalPerFace(can);
	calculateNormalPerFace(plane);
	calculateNormalPerFace(planeA);
	calculateNormalPerFace(planeB);
	calculateNormalPerFace(charizard);
	calculateNormalPerFace(diamond);
	calculateNormalPerFace(domePiece);

	calculateNormalPerVertex(cube);
	calculateNormalPerVertex(can);
	calculateNormalPerVertex(plane);
	calculateNormalPerVertex(planeA);
	calculateNormalPerVertex(planeB);
	calculateNormalPerVertex(charizard);
	calculateNormalPerVertex(diamond);
	calculateNormalPerVertex(domePiece);

	bmpTexture(textureArray, "stones.bmp", 0);
	bmpTexture(textureArray, "red.bmp", 1);
	bmpTexture(textureArray, "metal_texture.bmp", 2);
	bmpTexture(textureArray, "blue.bmp", 3);
	bmpTexture(textureArray, "sky.bmp", 4);
	codedTexture(textureArray, 5);

	CUBE = meshToDisplayList(cube, 1, -1, normalMode, 3, smoothTextures);
	CYLINDER = meshToDisplayList(can, 2, -1, normalMode, 4, smoothTextures);
	PLANE = meshToDisplayList(plane, 3, -1, normalMode, 2, smoothTextures);
	CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, 2, smoothTextures);
	PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
	DOME = meshToDisplayList(domePiece, 6, -1, normalMode, 5, smoothTextures);

	ratio = (double)width / (double)height;

	// enable
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}


float ang_cube1 = 0;
float ang_cube2 = 0;

// display
void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, width, height / 10);
	glOrtho(-10, 10, -10, 10, -10, 10);
	// view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	float timeE = glutGet(GLUT_ELAPSED_TIME)/1000;
	sprintf(msg, "Time Ellapsed: %f", timeE);
	renderBitmapString(-10.0, 0.0, 0.0, GLUT_BITMAP_8_BY_13, msg);
	glEnable(GL_LIGHTING);
	// projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, height / 10, width, height);
	gluPerspective(45, ratio, 1, 1000);
	// view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//Camera
	if (firstPerson) {
		gluLookAt(playerX, playerCamY, playerZ,
			getReferenceX(playerAngle), playerCamY, getReferenceZ(playerAngle),
			0.0f, 1.0f, 0.0f);
	}
	else {
		if (topDownView) {
			gluLookAt(0.0f, 750.0f, 0.0,
				0.0f, 1.0f, -1.0f,
				0.0f, 0.0f, -1.0f);
		}
		else {
			gluLookAt(0.0f, 500.0f, 500.0,
				0.0f, 1.0f, -1.0f,
				0.0f, 1.0f, 0.0f);
		}
		glScalef(scale*0.5, scale*0.5, scale*0.5);
		//glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
		glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, 0.0f);
	}
	
	//Draw Plane
	addShading(M_RED_PLASTIC, shadingMode);
	glPushMatrix();
		glTranslatef(-500, 0, -500);
		glCallList( PLANE );
	glPopMatrix();

	//Draw Dome 
	addShading(M_BLUE_PLASTIC, shadingMode);
	for (int i = 0; i < 32; i++) {
		glPushMatrix();
		glRotatef((i * 11.25), 0.0f, 1.0f, 0.0f);
		glCallList(DOME);
		glPopMatrix();
	}

	//Draw Maze
	glPushMatrix();
	glTranslatef(-500, 0, -500);
	for (int i = 0; i < 20; i++) {

		for (int j = 0; j < 20; j++) {
			if (mazeArray[j][i] > 0) {
				glPushMatrix();
					glTranslatef((i * 50), 0, (j * 50));
					//glCallList(CUBE);

					if (mazeArray[j][i] == 2 /*(i == 0 || i == 19) || (j == 0 || j == 19)*/ ) {
						addShading(M_BLUE_PLASTIC, shadingMode); 
						glCallList(CYLINDER);
					}
					else if (mazeArray[j][i] == 1) {
						addShading(M_CHROME, shadingMode);
						glCallList(CUBE);
					}

				glPopMatrix();
			}

		}

	}
	glPopMatrix();

	//Draw Dragon
	addShading(M_PEARL, shadingMode);
	glPushMatrix();
	glTranslatef(425, 25, -425);
	glCallList(CHARIZARD);
	glPopMatrix();

	//Draw Player
	if (!firstPerson) {
		addShading(M_JADE, shadingMode);
		glPushMatrix();
		glTranslatef(playerX, 5, playerZ);
		glCallList(PLAYER);
		glPopMatrix();
	}

	//Bullet

	addShading(M_PEARL, shadingMode);
	if (firstPerson) {
		if (fire && !bullet) {
			bullet = true; 
			bulletPosX = playerX;
			bulletPosZ = playerZ;
			bulletAngle = playerAngle;

			glPushMatrix();
			//glScalef(0.5, 0.5, 0.5);
			glTranslatef(bulletPosX, 15, bulletPosZ);
			glCallList(PLAYER);
			glPopMatrix();
		}
		else if (bullet) {
			bulletPosX += (3*sinf(bulletAngle));
			bulletPosZ += (3*cosf(bulletAngle));
			if (bulletPosX > 500 || bulletPosX < -500 || bulletPosZ > 500 || bulletPosZ < -500) {
				bullet = false;
			}
			glPushMatrix();
			//glScalef(0.5, 0.5, 0.5);
			glTranslatef(bulletPosX, 15, bulletPosZ);
			glCallList(PLAYER);
			glPopMatrix();
		}
		
		fire = false;
	}

	//Rotating stuff
	addShading(M_PEARL, shadingMode);
	glPushMatrix();
	glTranslatef(50, 50, 0);
		glCallList(CYLINDER);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(200 *sinf(ang_cube1) + 50, 50, 200 * cosf(ang_cube1));
	glCallList(CUBE);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef((75 * sinf(ang_cube2)) + (200 * sinf(ang_cube1)) + 50, 50, (75 * cosf(ang_cube2)) + (200 * cosf(ang_cube1)));
	glRotatef(ang_cube2 * 180 / M_PI, 0, 1, 0);
	glCallList(CUBE);
	glPopMatrix();
	
	// light
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 2.0 };
	GLfloat light_diffuse[] = { 2.0, 2.0, 2.0, 2.0 };
	GLfloat light_specular[] = { 2.0, 2.0, 2.0, 2.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glPushMatrix();
	GLfloat light_position[] = { 500*sin(lightAngle), 125.0, 500.0*cos(lightAngle), 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glPopMatrix();

	ang_cube1 += M_PI/400; 
	ang_cube2 -= M_PI /300;

	glutSwapBuffers();
}

/* MENUS - Start */
void menuListener(int option) {
	switch (option) {
		//FLAT
	case 1:
		glShadeModel(GL_FLAT);
		break;
		//SMOOTH
	case 2:
		glShadeModel(GL_SMOOTH);
		break;

		//Top Down View
	case 3:
		firstPerson = false;
		mouse_x = 0;
		mouse_y = 0;
		scale = 1.0;
		x_angle = 0.0;
		y_angle = 0.0;
		topDownView = true;
		break;
		//45 degree view
	case 4:
		firstPerson = false;
		mouse_x = 0;
		mouse_y = 0;
		scale = 1.0;
		x_angle = 0.0;
		y_angle = 0.0;
		topDownView = false;
		break;
	case 21:
		firstPerson = true;
		playerCamY = 25;
		break;

		//Slow
	case 5:
		playerSpeed = 2.5;
		break;
		//Medium
	case 6:
		playerSpeed = 5;
		break;
		//Fast
	case 7:
		playerSpeed = 10;
		break;

		//Light On
	case 8:
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		break;
		//Light Off
	case 9:
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		break;

		//PerFace
	case 10:
		normalMode = 0;
		CUBE = meshToDisplayList(cube, 1, 1, normalMode, -1, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, 1, normalMode, -1, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, 1, normalMode, -1, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, 1, normalMode, -1, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, 1, normalMode, -1, smoothTextures);
		DOME = meshToDisplayList(domePiece, 6, 1, normalMode, -1, smoothTextures);
		break;
		//PerFace Visualization Off
	case 11:
		normalMode = 0;
		CUBE = meshToDisplayList(cube, 1, -1, normalMode, -1, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, -1, normalMode, -1, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, -1, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, -1, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
		DOME = meshToDisplayList(domePiece, 6, -1, normalMode, -1, smoothTextures);
		break;

		//Per Vertex
	case 12:
		normalMode = 1;
		CUBE = meshToDisplayList(cube, 1, 2, normalMode, 3, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, 2, normalMode, 4, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, 2, normalMode, 2, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, 2, normalMode, 2, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, 2, normalMode, -1, smoothTextures);
		DOME = meshToDisplayList(domePiece, 6, 2, normalMode, 5, smoothTextures);
		break;
	case 13:
		normalMode = 1;
		CUBE = meshToDisplayList(cube, 1, -1, normalMode, -1, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, -1, normalMode, -1, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, -1, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, -1, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
		DOME = meshToDisplayList(domePiece, 6, -1, normalMode, -1, smoothTextures);
		break;

	//Textures On/Off
	case 14:
		normalMode = 1;
		CUBE = meshToDisplayList(cube, 1, -1, normalMode, 3, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, -1, normalMode, 4, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, 2, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, 2, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
		DOME = meshToDisplayList(domePiece, 6, -1, normalMode, 5, smoothTextures);
		break;
	case 15:
		normalMode = 1;
		CUBE = meshToDisplayList(cube, 1, -1, normalMode, -1, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, -1, normalMode, -1, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, -1, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, -1, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
		DOME = meshToDisplayList(domePiece, 6, -1, normalMode, -1, smoothTextures);
		break;

	case 16:
		shadingMode = S_DIFFUSE;
		break;
	case 17:
		shadingMode = S_SPECULAR;
		break;
	case 18:
		shadingMode = S_DESIGN;
		break;
	
		//Navigation
	case 19:
		lightNav = true;
		break;
	case 20:
		lightNav = false;
		break;


	case 22: 
		smoothTextures = true;
		CUBE = meshToDisplayList(cube, 1, -1, normalMode, 3, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, -1, normalMode, 4, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, 2, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, 2, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
		break;
	case 23:
		smoothTextures = false;
		CUBE = meshToDisplayList(cube, 1, -1, normalMode, 3, smoothTextures);
		CYLINDER = meshToDisplayList(can, 2, -1, normalMode, 4, smoothTextures);
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, 2, smoothTextures);
		CHARIZARD = meshToDisplayList(charizard, 4, -1, normalMode, 2, smoothTextures);
		PLAYER = meshToDisplayList(diamond, 5, -1, normalMode, -1, smoothTextures);
		break;

	//Noise
	case 24: 
		PLANE = meshToDisplayList(plane, 3, -1, normalMode, 2, smoothTextures);
		break;
	case 25:
		PLANE = meshToDisplayList(planeA, 3, -1, normalMode, 2, smoothTextures);
		break;
	case 26:
		PLANE = meshToDisplayList(planeB, 3, -1, normalMode, 2, smoothTextures);
		break;

		//Collisions
	case 27:
		playerCollisions = true;
		break;
	case 28:
		playerCollisions = false;
		break;

	//Crease Angles
	case 29:
		break;
	case 30:
		break;
	case 31:
		break;
	case 32:
		break;
	}
}

void createMenus() {

	//Object Rendering
	int menuA = glutCreateMenu(menuListener);
	glutAddMenuEntry("Flat", 1);
	glutAddMenuEntry("Smooth", 2);

	//Camera View
	int menuB = glutCreateMenu(menuListener);
	glutAddMenuEntry("Top-Down", 3);
	glutAddMenuEntry("45 Degree", 4);
	glutAddMenuEntry("First Person", 21);

	//Player Speed
	int menuC = glutCreateMenu(menuListener);
	glutAddMenuEntry("Slow", 5);
	glutAddMenuEntry("Medium", 6);
	glutAddMenuEntry("Fast", 7);

	//Light
	int menuD = glutCreateMenu(menuListener);
	glutAddMenuEntry("On", 8);
	glutAddMenuEntry("Off", 9);

	//Normal Visualization
	int MenuEsub = glutCreateMenu(menuListener);
	glutAddMenuEntry("Visualization On", 12);
	glutAddMenuEntry("Visualization Off", 13);

	//Normals
	int menuE = glutCreateMenu(menuListener);
	glutAddMenuEntry("Visualization On", 10);
	glutAddMenuEntry("Visualization Off", 11);
	
	int menuFsub = glutCreateMenu(menuListener);
	glutAddMenuEntry("Blend", 22);
	glutAddMenuEntry("Replace", 23);
	//Textures
	int MenuF = glutCreateMenu(menuListener);
	glutAddMenuEntry("On", 14);
	glutAddMenuEntry("Off", 15);
	glutAddSubMenu("Blend/Replace", menuFsub);

	//Materials
	int menuG = glutCreateMenu(menuListener);
	glutAddMenuEntry("Materials Diffuse", 16);
	glutAddMenuEntry("Materials Specular", 17);
	glutAddMenuEntry("Materials Design", 18);

	//Navigation 
	int menuH = glutCreateMenu(menuListener);
	glutAddMenuEntry("Light Source Navigation", 19);
	glutAddMenuEntry("Camera Navigation", 20);

	//Noise
	int menuI = glutCreateMenu(menuListener);
	glutAddMenuEntry("Plane - Flat", 24);
	glutAddMenuEntry("Plane - Multiscale", 25);
	glutAddMenuEntry("Plane - Marble", 26);

	//Player Collisions
	int menuJ = glutCreateMenu(menuListener);
	glutAddMenuEntry("Player Collisions - On", 27);
	glutAddMenuEntry("Player Collisions - Off", 28);

	int menuK = glutCreateMenu(menuListener);
	glutAddMenuEntry("Crease Angles Off", 29);
	glutAddMenuEntry("Crease Angles - 15", 30);
	glutAddMenuEntry("Crease Angles - 25", 31);
	glutAddMenuEntry("Crease Angles - 35", 32);

	// create main menu
	int menu = glutCreateMenu(menuListener);
	glutAddSubMenu("Object Rendering (Shading)", menuA);
	glutAddSubMenu("Camera View", menuB);
	glutAddSubMenu("Player Speed", menuC);
	glutAddSubMenu("Light", menuD);
	glutAddSubMenu("Normals Per Face", menuE);
	glutAddSubMenu("Normals Per Vertex", MenuEsub);
	glutAddSubMenu("Textures", MenuF);
	glutAddSubMenu("Shading", menuG);
	glutAddSubMenu("Navigation", menuH);
	glutAddSubMenu("Noise (Plane)", menuI);
	glutAddSubMenu("Player Collisions", menuJ);
	glutAddSubMenu("Crease Angles", menuK);
	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
/* MENUS - End */

//Called when time reaches 0
void quit(int i) {
	exit(0);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("3D");
	createMenus();
	myInit();
	glutSpecialFunc(specialKeys);
	glutSpecialUpFunc(keyRelease);
	glutKeyboardFunc(regularKeys);
	glutKeyboardUpFunc(regularKeysUp);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myDisplay);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//glutTimerFunc(180000, quit, 180000); //Game quits after 3 minutes
	glutMainLoop();
	return(0);
}