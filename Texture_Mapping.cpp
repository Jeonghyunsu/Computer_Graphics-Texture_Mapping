
#include "stdafx.h"
#include "Texture_Mapping.h"


GLfloat position[4];
AUX_RGBImageRec *texRec;
GLuint texID;
GLuint texIDs[6];
char* fileName[6] = { 
	"Material/wood.raw",
	"Material/check.raw",
	"Material/donut.raw",
	"Material/marble.raw",
	"Material/logo2.raw",
	"Material/grayscale_ornament.raw" };

Vector3x1 pt[36][18];
GLfloat gpt[36][18][3];
Vector3x1* nom[36][18];
GLfloat gnom[36][18][3];
GLfloat gvnom[36][18][3];
Vector3x1 seeYa(1, 1, 1);


GLuint LoadTexture(int num)
{
	GLuint texture;
	unsigned char* image;
	if (num >= 0 && num <= 3) image = new unsigned char[WIDTH_HEIGHT * WIDTH_HEIGHT * 3];
	else if (num >= 4 && num <= 5)image = new unsigned char[WIDTH_HEIGHT * WIDTH_HEIGHT * 4];

	FILE* file;
	file=fopen(fileName[num],"rb");

	if (num >= 0 && num <= 3) {
		fread(image, WIDTH_HEIGHT * WIDTH_HEIGHT * 3, 1, file);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH_HEIGHT, WIDTH_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (num >= 4 && num <= 5) {
	
		unsigned char* temp = new unsigned char[WIDTH_HEIGHT * WIDTH_HEIGHT];
		fread(temp, WIDTH_HEIGHT * WIDTH_HEIGHT, 1, file);

		for (int i = 0; i < WIDTH_HEIGHT * WIDTH_HEIGHT; i++)
		{
			image[ 4 * i ] = temp[i];
			image[ 4 * i + 1] = temp[i];
			image[ 4 * i + 2] = temp[i];
			if(image[4 * i] == 0)image[4 * i + 3] = 0;
			else image[4 * i + 3] = temp[i];
		}

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH_HEIGHT, WIDTH_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	fclose(file);
	free(image);

	return texture;
}
void ptTogpt()
{
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{

			for (int k = 0; k < 3; k++)
			{
				gpt[i][j][k] = pt[i][j].xyzArr[k];
			}


			if (i == 35)
			{
				if (j == 17)
				{
					Vector3x1* v1 = Vector3x1::VectorSubtraction(&pt[i][0], &pt[i][j]);
					Vector3x1* v2 = Vector3x1::VectorSubtraction(&pt[0][j], &pt[i][j]);

					nom[i][j] = Vector3x1::CrossProduct(v1, v2);
				}
				else
				{
					Vector3x1* v1 = Vector3x1::VectorSubtraction(&pt[i][j + 1], &pt[i][j]);
					Vector3x1* v2 = Vector3x1::VectorSubtraction(&pt[0][j], &pt[i][j]);

					nom[i][j] = Vector3x1::CrossProduct(v1, v2);
				}
			}
			else
			{

				if (j == 17)
				{
					Vector3x1* v1 = Vector3x1::VectorSubtraction(&pt[i][0], &pt[i][j]);
					Vector3x1* v2 = Vector3x1::VectorSubtraction(&pt[i + 1][j], &pt[i][j]);

					nom[i][j] = Vector3x1::CrossProduct(v1, v2);
				}
				else
				{
					Vector3x1* v1 = Vector3x1::VectorSubtraction(&pt[i][j + 1], &pt[i][j]);
					Vector3x1* v2 = Vector3x1::VectorSubtraction(&pt[i + 1][j], &pt[i][j]);

					nom[i][j] = Vector3x1::CrossProduct(v1, v2);
				}

			}
			for (int k = 0; k < 3; k++)
			{
				gnom[i][j][k] = -nom[i][j]->xyzArr[k];
			}
		}
	}
}
void SetTorus()
{
	double theta = 20 * PI / 180.0;
	double theta2 = 10 * PI / 180.0;

	double cx = 0.5, cy = 0.5, cz = 0;
	double sx = 0.7, sy = 0.5, sz = 0;

	Vector3x1 centerP(cx, cy, cz);
	Vector3x1 startP(sx, sy, sz);

	for (int i = 0; i < 18; i++)
	{
		Vector3x1* newV = Vector3x1::VectorSubtraction(&startP, &centerP);
		Matrix4x4 rotationMat = Matrix4x4(ZAXIS, theta);
		Vector3x1* rotationV = Matrix4x4::Matrix_VectorMultiplication(&rotationMat, newV);
		Vector3x1* resultV = Vector3x1::VectorAddition(rotationV, &centerP);

		pt[0][i] = *resultV;
		theta += 20 * PI / 180.0;
	}

	for (int j = 1; j < 36; j++)
	{
		for (int i = 0; i < 18; i++)
		{
			Matrix4x4 rotationMat2 = Matrix4x4(YAXIS, theta2);
			Vector3x1 newV = Vector3x1(pt[0][i].xyzArr[0], pt[0][i].xyzArr[1], pt[0][i].xyzArr[2]);
			Vector3x1* rotationV = Matrix4x4::Matrix_VectorMultiplication(&rotationMat2, &newV);

			pt[j][i] = *rotationV;
		}
		theta2 += 10 * PI / 180.0;
	}

	

}
void Normalize(GLfloat p[3])
{
	GLfloat d = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
	if (d > 0)
	{
		GLfloat len = (GLfloat)(1.0 / sqrt(d));
		p[0] *= len;
		p[1] *= len;
		p[2] *= len;
	}
}
void keyboard(unsigned char key, int x, int y);
void RenderScene();
void ChangeSize(int w, int h);
void SetupRC();
void init();
void setNom()
{
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			Vector3x1* ori = Vector3x1::ScalarMultiplication(Vector3x1::VectorAddition(&pt[i%36][j], &pt[(i + 1) % 36][(j + 1)%18]), 0.5);
			GLfloat gori[3];

			Vector3x1* nom1 = Vector3x1::ScalarMultiplication(nom[i%36][j%18], -20);
			Vector3x1* result = Vector3x1::VectorAddition(ori, nom1);

			for (int k = 0; k < 3; k++)
			{
				gnom[i%36][j%18][k] = result->xyzArr[k];
				gori[k] = ori->xyzArr[k];
			}
		}
	}
}
void DrawVertexNormalVectors()
{
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			

			for (int k = 0; k < 3; k++)
			{
				gvnom[i][j][k]= 
				   (gnom[i%36][j%18][k] + 
					gnom[i % 36][(j + 1) % 18][k] + 
					gnom[(i +1) % 36][j % 18][k] + 
					gnom[(i + 1) % 36][(j + 1) % 18][k])/4;

					Normalize(gvnom[i][j]);
				
			}
		
		}
	}

}
void DrawTorus()
{
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			

			glBegin(GL_QUAD_STRIP);
			glNormal3fv(gvnom[i % 36][j % 18]);
			glTexCoord2d(i / 36.0, j/ 18.0);
			glVertex3fv(gpt[i % 36][j % 18]);

			glNormal3fv(gvnom[(i + 1) % 36][j % 18]);
			glTexCoord2d( (i+1)/ 36.0, j/ 18.0);
			glVertex3fv(gpt[(i + 1) % 36][j % 18]);

			glNormal3fv(gvnom[(i % 36)][(j + 1) % 18]);
			glTexCoord2d( i/ 36.0,(j+1)/ 18.0);
			glVertex3fv(gpt[(i % 36)][(j+1)%18]);

			glNormal3fv(gvnom[(i + 1) % 36][(j + 1) % 18]);
			glTexCoord2d((i+1)/ 36.0, (j + 1) / 18.0);
			glVertex3fv(gpt[(i + 1) % 36][(j+1)%18]);
			glEnd();
		}

	}
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("");

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	glutKeyboardFunc(keyboard);

	init();
	SetupRC();

	glutMainLoop();

	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Q':
	case 'q':
		exit(1);
		break;

	case 's':
	case 'S':
		texID = texIDs[0];
		break;

	case 't':
	case 'T':
		texID = texIDs[1];
		break;

	case 'u':
	case 'U':
		texID = texIDs[2];
		break;

	case 'v':
	case 'V':
		texID = texIDs[3];
		break;

	case 'w':
	case 'W':
		texID = texIDs[4];
		break;

	case 'x':
	case 'X':
		texID = texIDs[5];
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLineWidth(1);
	gluLookAt(1.0f, 1.0f, 1.0f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	{
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(0, 0.0, 0.0);
		glVertex3f(200, 0.0, 0.0);

		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.0, 0, 0.0);
		glVertex3f(0.0, 200, 0.0);

		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.0, 0.0, 0);
		glVertex3f(0.0, 0.0, 100);
	}
	glEnd();
	glColor3d(1,1,1);
	

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);
	DrawTorus();

	glutSwapBuffers();
}
void SetupRC()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}
void ChangeSize(int w, int h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
	{
		glOrtho(-1.5, 1.5, -1.5*(float)h / (float)w, 1.5*(float)h / (float)w, -10.0, 10.0);
	}
	else
	{
		glOrtho(-1.5, 1.5, -1.5*(float)w / (float)h, 1.5*(float)w / (float)h, -10.0, 10.0);
	}
}
void init()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	for (int i = 0; i < 6; i++)texIDs[i] = LoadTexture(i);

	position[0] = 1; position[1] = 1; position[2] = 1; position[3] = 1;

	
	for (int i = 0; i < 15; i++)
	{
		SetTorus();
		ptTogpt();
		DrawVertexNormalVectors();
	}
}