#include "Textures.h"


// Load a DIB/BMP file from disk.
GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info) {
	FILE *fp;      // open file pointer
	GLubyte * bits; // bitmap pixel bits
	int bitsize;   // Size of bitmap
	int infosize;  // Size of header information
	BITMAPFILEHEADER header; // File header
							 // try opening the file; use "rb" mode to read this *binary* file.
	if ((fp = fopen(filename, "rb")) == NULL)
		return (NULL);
	// read the file header and any following bitmap information.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
		// Couldn't read the file header - return NULL.
		fclose(fp);
		return (NULL);
	}
	// Check for BM reversed.
	if (header.bfType != 'MB') {
		// not a bitmap file - return NULL.
		fclose(fp);
		return (NULL);
	}
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
		// couldn't allocate memory for bitmap info - return NULL.
		fclose(fp);
		return (NULL);
	}
	if (fread(*info, 1, infosize, fp) < infosize) {
		// Couldn't read the bitmap header - return NULL.
		free(*info);
		fclose(fp);
		return (NULL);
	}
	// Now that we have all the header info read in, allocate memory for the bitmap and read *it* in.
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth*(*info)->bmiHeader.biBitCount + 7) / 8 * abs((*info)->bmiHeader.biHeight);
	if ((bits = (GLubyte *)malloc(bitsize)) == NULL) {
		// Couldn't allocate memory - return NULL!
		free(*info);
		fclose(fp);
		return (NULL);
	}
	if (fread(bits, 1, bitsize, fp) < bitsize) {
		// couldn't read bitmap - free memory and return NULL!
		free(*info);
		free(bits);
		fclose(fp);
		return (NULL);
	}
	// OK, everything went fine - return the allocated bitmap.
	fclose(fp);
	return (bits);
}

// Create from algorithm
void codedTexture(UINT textureArray[], int n) {
	const int TexHeight = 128;
	const int TexWidth = 128;
	// create texture in memory
	GLubyte textureImage[TexHeight][TexWidth][4];
	for (int i = 0; i < TexHeight; i++)
		for (int j = 0; j < TexWidth; j++) {
			textureImage[i][j][0] = 127 + i; // red value from 0 to 255 
			textureImage[i][j][1] = 0; // green value from 0 to 255 
			textureImage[i][j][2] = 127 + j; // blue value from 0 to 255 
			textureImage[i][j][3] = 1; // alpha value from 0 to 255 
		}
	// setup texture
	glGenTextures(1, &textureArray[n]);
	glBindTexture(GL_TEXTURE_2D, textureArray[n]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
										   // glTexImage2D Whith size and minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TexWidth, TexHeight, GL_BGRA_EXT, GL_UNSIGNED_BYTE, textureImage); // BGRA to include alpha

}


// bmpTexture
void bmpTexture(UINT textureArray[], LPSTR file, int n) {
	BITMAPINFO *bitmapInfo; // Bitmap information
	GLubyte    *bitmapBits; // Bitmap data
	if (!file) {
		cout << "texture file not found!" << endl;
		return;
	}
	bitmapBits = LoadDIBitmap(file, &bitmapInfo);
	glGenTextures(1, &textureArray[n]);
	glBindTexture(GL_TEXTURE_2D, textureArray[n]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
										   // glTexImage2D Whith size and minification
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bitmapInfo->bmiHeader.biWidth, bitmapInfo->bmiHeader.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmapBits);
}

void addMaterial(int material) {

	if (material == M_CHROME) {
		GLfloat mat_emission[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
		GLfloat mat_ambient[] = { 0.25, 0.25, 0.25, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		GLfloat mat_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		GLfloat mat_specular[] = { 0.774597, 0.774597, 0.774597, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialf(GL_FRONT, GL_SHININESS, 76.8);
	}

	else if (material == M_PEARL) {
		GLfloat mat_emission4[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission4);
		GLfloat mat_ambient4[] = { 0.25, 0.20725, 0.20725, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient4);
		GLfloat mat_diffuse4[] = { 1.0, 0.829, 0.829, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse4);
		GLfloat mat_specular4[] = { 0.296648, 0.296648, 0.296648, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular4);
		glMaterialf(GL_FRONT, GL_SHININESS, 11.264);
	}

	else if (material == M_OBSIDIAN) {
		GLfloat mat_emission3[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission3);
		GLfloat mat_ambient3[] = { 0.05375, 0.05, 0.06625, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
		GLfloat mat_diffuse3[] = { 0.18275, 0.17, 0.22525, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);
		GLfloat mat_specular3[] = { 0.332471, 0.328634, 0.346435, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular3);
		glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
	}

	else if (material == M_JADE) {
		GLfloat mat_emission2[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);
		GLfloat mat_ambient2[] = { 0.135, 0.2225, 0.1575, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		GLfloat mat_diffuse2[] = { 0.54, 0.89, 0.63, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
		GLfloat mat_specular2[] = { 0.316228, 0.316228, 0.316228, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialf(GL_FRONT, GL_SHININESS, 12.8);
	}

	else if (material == M_RED_PLASTIC) {
		GLfloat mat_emission2[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);
		GLfloat mat_ambient2[] = { 0, 0, 0, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		GLfloat mat_diffuse2[] = { 0.5, 0.4, 0.4, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
		GLfloat mat_specular2[] = { 0.7, 0.04, 0.04, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialf(GL_FRONT, GL_SHININESS, 10);
	}

	else if (material == M_BLUE_PLASTIC) {
		GLfloat mat_emission2[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);
		GLfloat mat_ambient2[] = { 0, 0.5, 0.5, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		GLfloat mat_diffuse2[] = { 0.4, 0.5, 0.5, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
		GLfloat mat_specular2[] = { 0.05, 0.7, 0.7, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialf(GL_FRONT, GL_SHININESS, 10);
	}

	else if (material == M_DIFFUSE) {
		GLfloat mat_emission2[] = { 0.25, 0.25, 0.25, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);
		GLfloat mat_ambient2[] = { 0.1, 0.1, 0.1, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		GLfloat mat_diffuse2[] = {5.0, 5.0, 5.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
		GLfloat mat_specular2[] = { 0.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialf(GL_FRONT, GL_SHININESS, 85.0);
	}

	else if (material == M_SPECULAR) {
		GLfloat mat_emission2[] = { 0.25, 0.25, 0.25, 1.0 };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);
		GLfloat mat_ambient2[] = { 0.1, 0.1, 0.1, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		GLfloat mat_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
		GLfloat mat_specular2[] = { 3.0, 3.0, 3.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialf(GL_FRONT, GL_SHININESS, 85.0);
	}
}

void addShading(int material, int mode) {
	if (mode == 0) {
		addMaterial(M_DIFFUSE);
	}
	else if (mode == 1) {
		addMaterial(M_SPECULAR);
	}
	else {
		addMaterial(material);
	}
}