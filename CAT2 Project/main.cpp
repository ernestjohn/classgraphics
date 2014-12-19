#include <iostream>
#include <stdlib.h>
//#include <GL/glut.h>
#include "imageloader.h"
#include "vec3f.h"
#include "md2model.h"
#include "demo.h"


using namespace std;

//Represents our terrain, by storing a set of heights and normals at 2D locations
class Terrain {
private:
	int w; //Sets Width
	int l; //Set Length
	float** hs; //Sets Heights
	Vec3f** normals;
	bool computedNormals; //Computes upto date normals for the terrains
public:
	Terrain(int w2, int l2) {
		w = w2;
		l = l2;

		hs = new float*[l];
		for (int i =0; i < l; i++) {
			hs[i] = new float[w];
		}

		normals = new Vec3f*[l];
		for (int i = 0; i < l; i++) {
			normals[i] = new Vec3f[w];
		}

		computedNormals = false;
	}

	~Terrain() {
		for (int i = 0; i < l; i++) {
			delete[] hs[i];
		}
		delete[] hs;

		for (int i = 0; i < l; i++) {
			delete[] normals[i];
		}
		delete[] normals;
	}

	int width() {
		return w;
	}

	int length() {
		return l;
	}

	//Sets the height at (x, z) to y
	void setHeight(int x, int z, float y) {
		hs[z][x] = y;
		computedNormals = false;
	}

	//Returns the height at (x, z)
	float getHeight(int x, int z) {
		return hs[z][x];
	}

	//Computes the normals, if they haven't been computed yet
	void computeNormals() {
		if (computedNormals) {
			return;
		}

		//Compute the rough version of the normals
		Vec3f** normals2 = new Vec3f*[l];
		for (int i = 0; i < l; i++) {
			normals2[i] = new Vec3f[w];
		}

		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				Vec3f sum(0.0f, 0.0f, 0.0f);

				Vec3f out;
				if (z > 0) {
					out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
				}
				Vec3f in;
				if (z < l - 1) {
					in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
				}
				Vec3f left;
				if (x > 0) {
					left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
				}
				Vec3f right;
				if (x < w - 1) {
					right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
				}

				if (x > 0 && z > 0) {
					sum += out.cross(left).normalize();
				}
				if (x > 0 && z < l - 1) {
					sum += left.cross(in).normalize();
				}
				if (x < w - 1 && z < l - 1) {
					sum += in.cross(right).normalize();
				}
				if (x < w - 1 && z > 0) {
					sum += right.cross(out).normalize();
				}

				normals2[z][x] = sum;
			}
		}

		//Smooth out the normals
		const float FALLOUT_RATIO = 0.5f;
		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				Vec3f sum = normals2[z][x];

				if (x > 0) {
					sum += normals2[z][x - 1] * FALLOUT_RATIO;
				}
				if (x < w - 1) {
					sum += normals2[z][x + 1] * FALLOUT_RATIO;
				}
				if (z > 0) {
					sum += normals2[z - 1][x] * FALLOUT_RATIO;
				}
				if (z < l - 1) {
					sum += normals2[z + 1][x] * FALLOUT_RATIO;
				}

				if (sum.magnitude() == 0) {
					sum = Vec3f(0.0f, 1.0f, 0.0f);
				}
				normals[z][x] = sum;
			}
		}

		for (int i = 0; i < l; i++) {
			delete[] normals2[i];
		}
		delete[] normals2;

		computedNormals = true;
	}

	//Returns the normal at (x, z)
	Vec3f getNormal(int x, int z) {
		if (!computedNormals) {
			computeNormals();
		}
		return normals[z][x];
	}
};

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 1.5f);
			t->setHeight(x, y, h);
		}
	}

	delete image;
	t->computeNormals();
	return t;
}

float _angle = -0.0f;
float light = -1.0f;
//float light2 = 1.0f;
Terrain* _terrain;
GLuint _textureId;

//Lets instanciate new three models
MD2Model* _model;
MD2Model* _model2;
MD2Model* _model3;
MD2Model* _model4;

float _guyPos = 0.0f; //The forward position of the guy
float _guyPos2 = 200.0f;

void cleanup() {
	delete _terrain;
	delete _model;
	delete _model2;
	delete _model3;
	delete _model4;
}
static int f = 0, r = 0, l = 0;

void idleFuncNoRotate(void)
{
	_angle += 0.0f;
	g_angleCamera += 0.0;
	glutPostRedisplay();
}

void handleKeypress(unsigned char key, int x, int y) {
	//switch (key) {
	//case 27: //Escape key
	//	cleanup();
	//	exit(0);
	//case 's': //a key right go right
	//	_angle += -5.0f;
	//case 'd': //d key go left
	//	_angle += 5.0f;
	//
	//}

	if (key == 27)
	{
		cleanup();
		exit(0);
	}
	if (key == 'a')
	{
		_angle += -5.0f;
	}
	if (key == 'd')
	{
		_angle += 5.0f;
	}
	if (key == 'l')
	{
		if (l == 0)
		{
			GLfloat lightColor0[] = { 0.7f, 0.7f, 0.0f, 1.0f };
			GLfloat lightPos0[] = { light, 0.8f, 0.1f, 0.0f };
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
			l = 1;
		}
		else if (l == 1)
		{
			GLfloat lightColor0[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			GLfloat lightPos0[] = { light, 0.0f, 0.0f, 0.0f };
			glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
			l = 0;
		}
	}
	if (key == 'r')
	{
		if (r == 0)
		{
			glutIdleFunc(idleFunc);
			r = 1;
		}
		else if (r == 1)
		{
			glutIdleFunc(idleFuncNoRotate);
			r = 0;
		}
	}
	if (key == 'f')
	{
		if (f == 0)
		{
			GLfloat fogColor[] = { 0.5f, 0.5f, 0.5f, 0.5, 1.0f };
			glFogfv(GL_FOG_COLOR, fogColor);

			//CHANGE VARIOUS FOG PROPERTIES
			glFogi(GL_FOG_MODE, GL_LINEAR);
			glFogf(GL_FOG_START, 0.0f);
			glFogf(GL_FOG_END, 10.0f);
			glFogf(GL_FOG_DENSITY, 1.0f);
			f = 1;
		}
		else if (f == 1)
		{
			GLfloat fogColor[] = { 0.0f, 0.0f, 0.0f, 0.0, 0.0f };
			glFogfv(GL_FOG_COLOR, fogColor);

			//CHANGE VARIOUS FOG PROPERTIES
			glFogi(GL_FOG_MODE, GL_LINEAR);
			glFogf(GL_FOG_START, 0.0f);
			glFogf(GL_FOG_END, 10.0f);
			glFogf(GL_FOG_DENSITY, 10.0f);
			f = 0;
		}
	}
}



void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_FOG);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); //grey
	//glClearColor(0.0f, 0.5f, 0.8f, 1.0f); //blue

	//Lets Load our the models
	_model = MD2Model::load("tallguy.md2");
	if (_model != NULL) {
		_model->setAnimation("run");
	}

	_model2 = MD2Model::load("tallguy.md2");
	if (_model2 != NULL) {
		_model2->setAnimation("run");
	}

	_model3 = MD2Model::load("tallguy.md2");
	if (_model3 != NULL) {
		_model3->setAnimation("walk");
	}

	_model4 = MD2Model::load("tallguy.md2");
	if (_model4 != NULL) {
		_model4->setAnimation("run");
	}
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25.0, (double)w / (double)h, 6.0, 350.0);
}

void idleFunc(void)
{
	_angle += 0.2f;
	g_angleCamera += 0.3;

	glutPostRedisplay();
}



void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	GLfloat fogColor[] = { 0.5f, 0.5f, 0.5f, 0.5, 1.0f };

	//COMMENT TO REMOVE FOG
	//glFogfv(GL_FOG_COLOR, fogColor);

	//CHANGE VARIOUS FOG PROPERTIES
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 10.0f);
	glFogf(GL_FOG_DENSITY, 10.0f);

	glTranslatef(0.0f, 0.0f, -9.0f);
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-_angle, 0.0f, 7.0f, 0.0f); //terrain rotation

	//glRotatef(180.0, 0.0, 1.0, 0.0);

	GLfloat ambientColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	//CHANGE LIGHTING COLOUR
	/*GLfloat lightColor0[] = { 0.7f, 0.7f, 0.0f, 1.0f };
	GLfloat lightPos0[] = { light, 0.8f, 0.1f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);*/

	/*GLfloat lightColor1[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat lightPos1[] = { light2, 0.8f, 0.1f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);*/

	float scale = 5.0f / (_terrain->width() - 1, _terrain->length() - 1); //removed max
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(_terrain->width() - 1) / 2, 0.0f,
		-(float)(_terrain->length() - 1) / 2);

	//Change Terrain Colour
	glColor3f(1.5f, 1.0f, 0.2f); 
	
	
	for (int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}

	//Draw Our Humanoid Guys
	if (_model != NULL) {
		glPushMatrix();
		glTranslatef(100.0f, 10.0f, _guyPos);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		_model->draw();
		glPopMatrix();
	}
	
	if (_model2 != NULL) {
		glPushMatrix();
		glTranslatef(150.0f, 10.0f, _guyPos2);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		_model2->draw();
		glPopMatrix();
	}

	//Second Model Larger One
	if (_model3 != NULL) {
		glPushMatrix();
		glTranslatef(130.0f, 10.0f, 130.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		//glScalef(1.0f, 1.0f, 1.0f); //Line to change the size of the object
		_model3->draw();
		glPopMatrix();
	}

	if (_model4 != NULL) {
		glPushMatrix();
		glTranslatef(80.0f, 10.0f, _guyPos2);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(1.5f, 1.5f, 1.5f); //Line to change the size of the object
		_model4->draw();
		glPopMatrix();
	}

	glutSwapBuffers();
}

void update(int value) {
	//_angle -= 5.0f;
	/*if (_angle > 360) {
	_angle -= 360;
	}*/

	//update lights position
	light += 0.05f;
	//light2 -= 0.1f;
	if (light >= 2.0f) {
	light -= 4.0f;
	}
	/*if (light2 <= -1.0f) {
		light2 += 2.0f;
	}*/

	if (_model != NULL) {
		_model->advance(0.025f);
	}

	if (_model2 != NULL) {
		_model2->advance(0.025f);
	}

	//Update _guyPos & _guyPos2
	_guyPos += 0.5f;
	_guyPos2 -= 0.5f;
	if (_guyPos >= 200)
		_guyPos = 0.0f;
	if (_guyPos2 <= 0.0f)
	_guyPos2 = 200.0f;

	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

void menustate(int state)
{

}

void menu(int item)
{
	handleKeypress((unsigned char)item, 0, 0);
}



int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//Our window size
	glutInitWindowSize(800, 600); 

	glutCreateWindow("Final Semester Project");
	initRendering();

		//Load bitmap set height of bitmap
	//_terrain = loadTerrain("terrain.bmp", 40);
	_terrain = loadTerrain("terrain.bmp", 40);

	//Update resize
	glutDisplayFunc(drawScene);

	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	//glutIdleFunc(idleFunc);
	
	

	glutMenuStateFunc(menustate);
	glutCreateMenu(menu);
	glutAddMenuEntry("[f]   Fog on/off", 'f');
	glutAddMenuEntry("[i]   Lighting on/off", 'l');
	glutAddMenuEntry("[r]   Automatic Rotation on/off", 'r');
	glutAddMenuEntry("[a]   Rotate Left", 'a');
	glutAddMenuEntry("[d]   Rotate Right", 'd');
	glutAddMenuEntry("[Esc] Quit", 27);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}