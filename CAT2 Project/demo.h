//-----------------------------------------------------------------------------
// "Always share your knowledge"
//
// File: demo.h
//
// Author : JeGX - jegx2002@yahoo.fr - blox.e9h@caramail.com
//
// Date : 13.03.2002
//
// PURPOSE : Win32/Linux OpenGL test. Benchmark on SGI Fuel Platform.
// 
// Copyright (c) 2002 JeGX - BloX.E9h - http://jegx.free.fr
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//
//-----------------------------------------------------------------------------

#ifndef _DEMO_H
#define _DEMO_H


//#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <string.h>


#pragma comment( lib, "glut32.lib" )


// Intel's processors /////////////////////
#define _HOST_LITTLE_EDIAN  

// Motorola's processors //////////////////
//#define _HOST_BIG_EDIAN  


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Basic types START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

typedef unsigned long Dword_;	// 32 bits //
typedef unsigned short Word_;   // 16 bits //
typedef unsigned char Byte_;	// 8 bits //

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Basic types END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


Word_ getWord(Word_ w);
Dword_ getDword(Dword_ dw);


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RGB Pixel class START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

class cRGB_Byte_Pixel
{
public:

	cRGB_Byte_Pixel(Byte_ r_ = 0, Byte_ g_ = 0, Byte_ b_ = 0);
	void set(Byte_ r, Byte_ g, Byte_ b);
	void set(cRGB_Byte_Pixel *pixel);

	union
	{
		struct
		{
			Byte_ r;
			Byte_ g;
			Byte_ b;
		};

		Byte_ rgb[3];
	};
};

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< RGB Pixel class END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>





//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< BMP Structures START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct sBMP_header
{
	Word_ codmag;
	Dword_ taille;
	Word_ reserv1;
	Word_ reserv2;
	Dword_ offsetImage;
};


struct sBMP_info
{
	Dword_ TailleZoneInfo;
	Dword_ LargeurImage;
	Dword_ HauteurImage;
	Word_ Nb_plans;								// toujours 1
	Word_ BitParPixel;							// 1/4/8/24
	Dword_ FormatCompressImage;
	Dword_ TailleOctets;
	Dword_ ResolutionHorizon;					// pixel par mètres
	Dword_ ResolutionVertical;					// pixel par mètres
	Dword_ NbCouleurUtil;						// 0 = maximum  si 24 bits alors ce champ spécifie taille palette
	Dword_ NbIndexCouleur;						// 0 = toutes les couleur importantes						
};


struct sImageBMP
{
	sBMP_header header;
	sBMP_info info;
	cRGB_Byte_Pixel *data;
};

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< BMP Structures END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

float v0[] = { -10.0, 0.0, -10.0, 1.0 }; //p.set( -half_x_size, 0.0f, -half_z_size );
float v1[] = { 10.0, 0.0, -10.0, 1.0 }; //	v1.p.set( half_x_size, 0.0f, -half_z_size );
float v2[] = { -10.0, 0.0, 10.0, 1.0 }; //	v2.p.set( -half_x_size, 0.0f, half_z_size );
float v3[] = { 10.0, 0.0, 10.0, 1.0 }; //	v3.p.set( half_x_size, 0.0f, half_z_size );


float g_lightPosition[] = { 0.0, 20.0, 30.0, 0.9 };
float g_lightAmbient[] = { 0.9f, 0.9f, 0.9f, 0.9 };
float g_lightDiffuse[] = { 0.5f, 0.5f, 0.5f, 0.9 };

float g_materialAmbient[] = { 0.10, 0.3, 0.10, 0.5 };
float g_materialDiffuse[] = { 0.5, 0.5, 0.5, 0.5 };
float g_materialSpecular[] = { 0.1, 0.1, 0.9, 0.5 };
float g_shininess = 50.0;


float g_angle = 0.0;
float g_angleCamera = 0.0;


cRGB_Byte_Pixel *g_texture_pixmap = NULL;
int g_width;
int g_height;
unsigned int g_texID;
unsigned int g_tex3ID;

int g_texture_width;
int	g_texture_height;

void drawString(char *text, float x, float y);
cRGB_Byte_Pixel *BMP_Loader(char *filename);
void reshape(int width, int height);
void idleFunc(void);

void startupDemo();
void shutdownDemo();

void draw();


float time;
float timebase;
int frame;
float frameRate;
char fps[256];
char fps_glut[256];
float fpssss;


Dword_ list_terrain;
float g_threshold = 1.0;
float terrainMap[256 * 256];
float elevation(int i, int j, float nbSubDiv, float echelleVerticale);
void createTerrain(int nbSubDiv, float echelleVerticale);
void convertToGrayScale(float *terrain, cRGB_Byte_Pixel *pixmap);
int terrain_subdivision = 32;
float g_verticalScale = 2.0;



unsigned int fontBaseIndex;
unsigned int fontTextureID;
void buildFont();
void killFont();
//void glPrint(GLint x, GLint y, char *string, int set);
//void glPrint(GLint x, GLint y, char *string, int set, float r, float g, float b, float a);



class cVector
{

public:

	cVector(float dx = 0.0f, float dy = 0.0f, float dz = 0.0f, float dw = 1.0f);

	////////////////////////////////////////////////
	// Vector homogeneous coordinates
	//
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		float v[4];
	};

	///////////////////////////////////////////////
	// Vector setting
	//
	void set(float dx, float dy, float dz, float dw = 1.0f);
	void set(cVector &v);

	////////////////////////////////////////////////
	// Dot product
	//
	float dot(cVector &v);

	////////////////////////////////////////////////
	// Cross product
	//
	//void cross_v1( O3_Vector &in, O3_Vector &out ); 
	cVector cross_v2(cVector &v);

	//void scalarMult( Float_ scalar );

	//void add( O3_Vector &v );
	//void sub( O3_Vector &v );

	////////////////////////////////////////////////
	// Vector nomalization
	// Normalize the vector, length set to 1
	void normalize();

	////////////////////////////////////////////////
	// Vector magnitude
	//
	float mag();
};


struct sVertex
{
	float x;
	float y;
	float z;

	cVector n;

	float u;
	float v;
};

struct sFace
{
	Dword_ a;
	Dword_ b;
	Dword_ c;

	cVector n;
};


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Mesh class START >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

/*class cMesh
{

public:

	cMesh();
	virtual ~cMesh();

	void clear();

	////////////////////////////////////////////////////////
	// Mesh structure
	//
	sVertex *m_verticesList;
	Dword_ m_numVertices;
	sFace *m_facesList;
	Dword_ m_numFaces;
	float materialAmbient[4];
	float materialDiffuse[4];
	float materialSpecular[4];
	float shininess;
	float env_color[4];
	float materialColor[4];

	cVector m_scale;

	cVector position;
	float angle;


	///////////////////////////////////////////////////////
	// Mesh id used with OpenGL display List
	//
	int meshId;
	Dword_ textureId;

	Dword_ vertex;

	int polygonMode;

	int load(char *filename);
	void readLine(Byte_ *line);
	Byte_ *linePointer;
	void preProcess();

	////////////////////////////////////////////////////////
	// 
	//
	virtual void update();
	virtual void draw();
};

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< Mesh class END >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Polygon mode ///////
#define MODE_POINT 1
#define MODE_LINE  2
#define MODE_GOURAUD_FILL  3
#define MODE_TEXTURE_FILL  4


cMesh mesh;


float fogColor[4];
int fogActive = 0;*/

// Mouse ...
int old_x = 0, old_y = 0;
float spin_x = 0.0;
float spin_y = 0.0;
float z_value = 15.0;
float y_value = 10.0;


#endif