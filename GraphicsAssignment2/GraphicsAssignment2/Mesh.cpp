#include "Mesh.h"


// str to int
int StrToInt(const string &str) {

	int i;
	if (sscanf_s(str.c_str(), "%i", &i) == 1) return i;

	else return 0;
}

// split string
vector<string> split_string(const string& str, const string& split_str) {

	vector<string> stlv_string;
	string part_string("");
	string::size_type i;

	i = 0;
	while (i < str.size()) {

		if (split_str.find(str[i]) != string::npos) {
			stlv_string.push_back(part_string);
			part_string = "";

			while (split_str.find(str[i]) != string::npos) {
				++i;
			}
		}
		else {
			part_string += str[i];
			++i;
		}
	}

	if (!part_string.empty()) {
		stlv_string.push_back(part_string);
	}

	return stlv_string;
}

Mesh* loadFile(const char* file) {
	Mesh *m = new Mesh;
	m->dot_vertex.clear();
	m->face_index_vertex.clear();
	ifstream infile(file);
	if (infile.fail()) {
		cout << "Error opening file " << file;
		return NULL;
	}
	char current_line[1024];
	while (!infile.eof()) {
		infile.getline(current_line, 1024);
		switch (current_line[0]) {
		case'v':
			float x, y, z;
			switch (current_line[1]) {
			case 'n':
				sscanf_s(current_line, "vn %f %f %f", &x, &y, &z);
				m->dot_normalPerFace.push_back(Vec3f(x, y, z));
				m->dot_normalPerVertex.push_back(Vec3f(x, y, z));
				break;
			case 't':
				sscanf_s(current_line, "vt %f %f", &x, &y);
				m->dot_texture.push_back(Vec2f(x, y));
				break;
			default:
				sscanf_s(current_line, "v %f %f %f", &x, &y, &z);
				m->dot_vertex.push_back(Vec3f(x, y, z));
				break;
			}
			break;
		case 'f': {
			vector<string> faces = split_string(current_line, " \t.\r\n");
			int vnt[3][3] = { { -1, -1, -1 },{ -1, -1, -1 },{ -1, -1, -1 } };
			string::size_type begin, end;
			for (int i = 0; i < 2; i++) {
				begin = 0;
				int j = 0;
				do {
					end = faces[i + 1].find_first_of("/", begin);
					if (begin < end) {
						vnt[i][j] = StrToInt(faces[i + 1].substr(begin, end - begin)) - 1;
					}
					else {
						vnt[i][j] = -1;
					}
					begin = end + 1;
					j++;
				} while (end != string::npos);
			}
			for (unsigned int i = 3; i < faces.size(); i++) {
				begin = 0;
				int j = 0;
				do {
					end = faces[i].find_first_of("/", begin);
					if (begin < end) {
						vnt[2][j] = StrToInt(faces[i].substr(begin, end - begin)) - 1;
					}
					else {
						vnt[2][j] = -1;
					}
					begin = end + 1;
					j++;
				} while (end != string::npos);
				for (int j = 0; j < 3; j++) {
					m->face_index_vertex.push_back(vnt[j][0]);
					if (vnt[j][1] != -1) m->face_index_texture.push_back(vnt[j][1]);
					if (vnt[j][2] != -1) m->face_index_normalPerFace.push_back(vnt[j][2]);
					if (vnt[j][2] != -1) m->face_index_normalPerVertex.push_back(vnt[j][2]);
				}
				memcpy(&vnt[1], &vnt[2], sizeof(int) * 3);
			}
		}
				  break;
		default:
			break;
		}
	}

	return m;
}

Mesh* createPlane(int arena_width, int arena_depth, int arena_cell, int noiseType) {

	Mesh *me = new Mesh;
	int n = arena_width / arena_cell;
	int m = arena_depth / arena_cell;
	Noise planeNoise;
	double noiseTotal = 0;

	// vertices
	for (int i = 0; i<n; i++) {
		for (int j = 0; j < m; j++) {
			if (noiseType == 1) {
				noiseTotal = planeNoise.perlinMultiscale(i*arena_cell, j*arena_cell);
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 25 * noiseTotal, j*arena_cell));
			}
			else if (noiseType == 2) {
				noiseTotal = planeNoise.perlinMarble(i*arena_cell, j*arena_cell);
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 25 * noiseTotal, j*arena_cell));
			}
			else {
				me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0.0, j*arena_cell));
			}
		}
	}
	//texture
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	me->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	// faces
	for (int i = 0; i<(n*m) - m; i++) {
		if ((i + 1) % n == 0) continue;
		me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
		me->face_index_vertex.push_back(i + n);
		me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
		me->face_index_vertex.push_back(i + n);
		// index for texture
		for (int t = 0; t<6; t++) {
			me->face_index_texture.push_back(3);//0
			me->face_index_texture.push_back(2);//2
			me->face_index_texture.push_back(1);//1
			me->face_index_texture.push_back(2);//0
			me->face_index_texture.push_back(0);//3
			me->face_index_texture.push_back(1);//2
		}
	}
	return me;

}

Mesh* createCylinder(int radius, int height) {
	Mesh *can = new Mesh;
	GLfloat twicePi = 2.0f * M_PI;

	//Vertices
		//Bottom circle
	for (int i = 0; i<32; i++) {
		can->dot_vertex.push_back(Vec3<GLfloat>(radius*sin(i *  twicePi / 32)+radius, 0.0, radius*cos(i *  twicePi / 32) + radius));
	}
		//Top circle
	for (int i = 0; i<32; i++) {
		can->dot_vertex.push_back(Vec3<GLfloat>(radius*sin(i *  twicePi / 32) + radius, height, radius*cos(i *  twicePi / 32) + radius));
	}
	can->dot_vertex.push_back(Vec3<GLfloat>(radius, 0.0, radius));
	can->dot_vertex.push_back(Vec3<GLfloat>(radius, height, radius));


	//Faces
	for (int i = 1; i<32; i++) {
		can->face_index_vertex.push_back(i-1);
		can->face_index_vertex.push_back(i);
		can->face_index_vertex.push_back(can->dot_vertex.size() - 2);
	}
	can->face_index_vertex.push_back(31);
	can->face_index_vertex.push_back(0);
	can->face_index_vertex.push_back(can->dot_vertex.size() - 2);

	for (int i = 33; i<64; i++) {
		can->face_index_vertex.push_back(i - 1);
		can->face_index_vertex.push_back(i);
		can->face_index_vertex.push_back(can->dot_vertex.size() - 1);
	}
	can->face_index_vertex.push_back(63);
	can->face_index_vertex.push_back(32);
	can->face_index_vertex.push_back(can->dot_vertex.size() - 1);

	for (int i = 1; i<32; i++) {
		can->face_index_vertex.push_back(i - 1);
		can->face_index_vertex.push_back(i);
		can->face_index_vertex.push_back(i + 32);
		can->face_index_vertex.push_back(i - 1);
		can->face_index_vertex.push_back(i + 32);
		can->face_index_vertex.push_back(i + 31);
	}

	can->face_index_vertex.push_back(31);
	can->face_index_vertex.push_back(0);
	can->face_index_vertex.push_back(32);
	can->face_index_vertex.push_back(31);
	can->face_index_vertex.push_back(32);
	can->face_index_vertex.push_back(63);

	can->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	can->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	can->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	can->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));

	for (int t = 0; t<64; t++) {

		can->face_index_texture.push_back(0);//0
		can->face_index_texture.push_back(1);//1
		can->face_index_texture.push_back(2);//2
		can->face_index_texture.push_back(0);//0
		can->face_index_texture.push_back(2);//2
		can->face_index_texture.push_back(3);//3

	}

	return can;
}
Mesh* createCube() {
	Mesh *mesh = new Mesh;

	// Vertexes
	int BOX_WIDTH = 50;
	int BOX_HEIGHT = 50;
	int BOX_DEPTH = 50;

	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, BOX_DEPTH));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, BOX_DEPTH));

	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, 0.0));
	mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, 0.0));

	// face 1
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(2);//1
	mesh->face_index_vertex.push_back(1);//2
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(3);//2

	mesh->face_index_vertex.push_back(2);//3

	 // face 2
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(5);//1
	mesh->face_index_vertex.push_back(4);//5
	mesh->face_index_vertex.push_back(0);//0
	mesh->face_index_vertex.push_back(1);//5
	mesh->face_index_vertex.push_back(5);//4

	// face 3
	mesh->face_index_vertex.push_back(1);//1
	mesh->face_index_vertex.push_back(6);//2
	mesh->face_index_vertex.push_back(5);//6
	mesh->face_index_vertex.push_back(1);//1
	mesh->face_index_vertex.push_back(2);//6
	mesh->face_index_vertex.push_back(6);//5

	mesh->face_index_vertex.push_back(2);//2
	mesh->face_index_vertex.push_back(7);//3
	mesh->face_index_vertex.push_back(6);//7
	mesh->face_index_vertex.push_back(2);//2
	mesh->face_index_vertex.push_back(3);//7
	mesh->face_index_vertex.push_back(7);//6

	mesh->face_index_vertex.push_back(4);//0
	mesh->face_index_vertex.push_back(3);//3
	mesh->face_index_vertex.push_back(0);//7
	mesh->face_index_vertex.push_back(4);//0
	mesh->face_index_vertex.push_back(7);//7
	mesh->face_index_vertex.push_back(3);//4

	mesh->face_index_vertex.push_back(5);//4 
	mesh->face_index_vertex.push_back(7);//5
	mesh->face_index_vertex.push_back(4);//6
	mesh->face_index_vertex.push_back(5);//4
	mesh->face_index_vertex.push_back(6);//6
	mesh->face_index_vertex.push_back(7);//7

	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));

	for (int t = 0; t<6; t++) {

		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(2);//1
		mesh->face_index_texture.push_back(1);//2
		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(3);//2
		mesh->face_index_texture.push_back(2);//3

	}

	return mesh;

}

Mesh* createDiamond() {
	Mesh *mesh = new Mesh;
	// Vertexes
	mesh->dot_vertex.push_back(Vec3f(20, 0, 20));
	mesh->dot_vertex.push_back(Vec3f(-20, 0, 20));
	mesh->dot_vertex.push_back(Vec3f(-20, 0, -20));
	mesh->dot_vertex.push_back(Vec3f(20, 0, -20));
	mesh->dot_vertex.push_back(Vec3f(0, 20, 0));
	mesh->dot_vertex.push_back(Vec3f(0, -20, 0));
	// Indexes
	mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1);
	mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(2);
	mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(4);
	mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(4);
	mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(4);
	mesh->face_index_vertex.push_back(4); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1);
	mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(5);
	mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(3);
	mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(2);
	mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(1);
	return mesh;

	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));

	for (int t = 0; t<4; t++) {

		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(2);//1
		mesh->face_index_texture.push_back(1);//2
		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(3);//2
		mesh->face_index_texture.push_back(2);//3

	}
	
}

Mesh* createDomePiece32() {
	Mesh *mesh = new Mesh;

	float twoPi = 2 * M_PI;
	mesh->dot_vertex.push_back(Vec3f(0,800,0));

	float vary = 1.0;
	for (int i = 600; i >= 0; i -= 200) {
		float j = 1000 - (i*vary) ;
		mesh->dot_vertex.push_back(Vec3f(j * sin(twoPi), i, j * cos(twoPi)));
		mesh->dot_vertex.push_back(Vec3f(j * sin(twoPi / 32), i, j * cos(twoPi / 32)));
		vary -= 0.15;
	}

	mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(0);
	for (int i = 1; (i + 3) < mesh->dot_vertex.size(); i += 2) {
		mesh->face_index_vertex.push_back(i+3); mesh->face_index_vertex.push_back(i+2); mesh->face_index_vertex.push_back(i);
		mesh->face_index_vertex.push_back(i+1); mesh->face_index_vertex.push_back(i+3); mesh->face_index_vertex.push_back(i);
	}

	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
	mesh->dot_texture.push_back(Vec2<GLfloat>(0.5, 1.0));

	mesh->face_index_texture.push_back(3);
	mesh->face_index_texture.push_back(0);
	mesh->face_index_texture.push_back(4);
	for (int i = 0; i < mesh->face_index_vertex.size() / 6; i++) {
		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(2);//1
		mesh->face_index_texture.push_back(1);//2
		mesh->face_index_texture.push_back(0);//0
		mesh->face_index_texture.push_back(3);//2
		mesh->face_index_texture.push_back(2);//3
	}

	return mesh;
}

void calculateNormalPerFace(Mesh* m) {
	Vec3<float> v1, v2, v3, v4, v5;
	for (int i = 0; i < m->face_index_vertex.size(); i += 3) {
		v1 = m->dot_vertex[m->face_index_vertex[i]];
		v2 = m->dot_vertex[m->face_index_vertex[i + 1]];
		v3 = m->dot_vertex[m->face_index_vertex[i + 2]];
		v4 = (v2 - v1);
		v5 = (v3 - v1);
		v4 = v4.cross(v5);
		v4.normalize();
		m->dot_normalPerFace.push_back(v4);
		int pos = m->dot_normalPerFace.size() - 1;
		// same normal for all vertex in this face
		m->face_index_normalPerFace.push_back(pos);
		m->face_index_normalPerFace.push_back(pos);
		m->face_index_normalPerFace.push_back(pos);
	}
}

// calculate normal per vertex
void calculateNormalPerVertex(Mesh* m) {
	m->dot_normalPerVertex.clear();
	m->face_index_normalPerVertex.clear();
	Vec3<float> suma; suma.x = 0; suma.y = 0; suma.z = 0;
	//initialize
	for (unsigned int val = 0; val < m->dot_vertex.size(); val++) {
		m->dot_normalPerVertex.push_back(suma);
	}
	// calculate sum for vertex
	for (long pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->dot_normalPerVertex[m->face_index_vertex[pos]] +=
			m->dot_normalPerFace[m->face_index_normalPerFace[pos]];
	}
	// normalize for vertex 
	for (unsigned int val = 0; val < m->dot_normalPerVertex.size(); val++) {
		m->dot_normalPerVertex[val] = m->dot_normalPerVertex[val].normalize();
	}
	//normalVertexIndex is the same that vertexIndex
	for (unsigned int pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->face_index_normalPerVertex.push_back(m->face_index_vertex[pos]);
	}
}

void calcCreaseAngle(Mesh* m, float angle) {
	m->dot_normalPerVertex.clear();
	m->face_index_normalPerVertex.clear();
	Vec3<float> suma; suma.x = 0; suma.y = 0; suma.z = 0;
	//initialize
	for (unsigned int val = 0; val < m->dot_vertex.size(); val++) {
		m->dot_normalPerVertex.push_back(suma);
	}
	// calculate sum for vertex
	for (long pos = 0; pos < m->face_index_vertex.size(); pos++) {
		for (long pos2 = 0; pos2 < m->face_index_vertex.size(); pos2++) {
			if ((pos != pos2) && (m->face_index_vertex[pos] == m->face_index_vertex[pos2])) {
				float dotProduct = m->dot_normalPerFace[pos].dot(m->dot_normalPerFace[pos2]);
				float faceAngle = acos(dotProduct)*180.0/M_PI;
				if (faceAngle < angle) {
					m->dot_normalPerVertex[m->face_index_vertex[pos]] +=
						m->dot_normalPerFace[m->face_index_normalPerFace[pos]];
				}
			}
		}
		/*
		m->dot_normalPerVertex[m->face_index_vertex[pos]] +=
			m->dot_normalPerFace[m->face_index_normalPerFace[pos]];*/
	}
	// normalize for vertex 
	for (unsigned int val = 0; val < m->dot_normalPerVertex.size(); val++) {
		m->dot_normalPerVertex[val] = m->dot_normalPerVertex[val].normalize();
	}
	//normalVertexIndex is the same that vertexIndex
	for (unsigned int pos = 0; pos < m->face_index_vertex.size(); pos++) {
		m->face_index_normalPerVertex.push_back(m->face_index_vertex[pos]);
	}
}

Vec3f getCenter(Vec3f pointA, Vec3f pointB, Vec3f pointC) {
	Vec3f center;
	
	center[0] = (pointA[0] + pointB[0] + pointC[0]) / 3;
	center[1] = (pointA[1] + pointB[1] + pointC[1]) / 3;
	center[2] = (pointA[2] + pointB[2] + pointC[2]) / 3;

	return center;
}

GLuint meshToDisplayList(Mesh* m, int id, int lines, int normals, GLuint texture, bool smooth) {

	GLuint listID = glGenLists(id);
	glNewList(listID, GL_COMPILE);
	/*
	if (mat > -1)
		glCallList(mat);*/

	if (texture > 0) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		if (smooth == true) {
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		}
		else {
			//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	glBegin(GL_TRIANGLES);

	for (unsigned int i = 0; i < m->face_index_vertex.size(); i++) {
		if ((normals == 1) && (!m->dot_normalPerVertex.empty() && !m->face_index_normalPerVertex.empty())) {
			glNormal3fv(&m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x);
			//printf("PERVERTEX");
		}
		else if ((normals == 0) && (!m->dot_normalPerFace.empty() && !m->face_index_normalPerFace.empty())) {
			glNormal3fv(&m->dot_normalPerFace[m->face_index_normalPerFace[i]].x);
			//printf("PERFACE");
		}
		if (!m->dot_texture.empty() && !m->face_index_texture.empty()) {
			glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
		}
		// color
		Vec3f offset = (m->dot_vertex[m->face_index_vertex[i]]);
		//
		glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
		glVertex3fv(&m->dot_vertex[m->face_index_vertex[i]].x);
	}

	if (texture > 0) {
		glDisable(GL_TEXTURE_2D);
	}

	glEnd();

	if (lines == 1) {
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		for (int i = 0; i < m->face_index_vertex.size(); i += 3) {
			Vec3f center = getCenter(m->dot_vertex[m->face_index_vertex[i]], m->dot_vertex[m->face_index_vertex[i + 1]], m->dot_vertex[m->face_index_vertex[i + 2]]);
			glVertex3f(center[0], center[1], center[2]);
			glVertex3f(m->dot_normalPerFace[m->face_index_normalPerFace[i]].x * 10 + center[0], m->dot_normalPerFace[m->face_index_normalPerFace[i]].y * 10 + center[1], m->dot_normalPerFace[m->face_index_normalPerFace[i]].z * 10 + +center[2]);	
		}
		glEnd();
	}
	else if (lines == 2) {
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		for (int i = 0; i < m->face_index_vertex.size(); i ++) {
			Vec3f center = getCenter(m->dot_vertex[m->face_index_vertex[i]], m->dot_vertex[m->face_index_vertex[i]], m->dot_vertex[m->face_index_vertex[i]]);
			glVertex3f(center[0], center[1], center[2]);
			glVertex3f(m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x * 10 + center[0], m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].y * 10 + center[1], m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].z * 10 + +center[2]);
		}
		glEnd();
	}
	
	glEndList();
	return listID;

}

bool checkCollision(float x, float z, bool collision) {

	bool collide = false;

	if (mazeArray[(int) z / 50][(int) x / 50] > 0) {
		cout << "Collision\n";
		collide = true;
	}
	cout << x / 50 << " " << z / 50 << "\n";

	if (!collision) {
		collide = false;
	}

	return collide;
}