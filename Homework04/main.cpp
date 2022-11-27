#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MySphere.h"
#include "Mesh.h"
#include "MeshRenderer.h"

#include <vec.h>
#include <mat.h>

MyCube cube;
MySphere sphere;

GLuint program;
GLuint prog_phong;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);

int winWidth = 500;
int winHeight = 500;

bool bPlay = true;

mat4 myLookAt(vec3 eye, vec3 at, vec3 up)
{
	// Implement your own look-at function
	mat4 V(1.0f);
	vec3 n = at-eye;
	n /= length(n);

	float a = dot(up, n);
	vec3 v = up - a*n;
	v /= length(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);

	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			Rc[i][j] = Rw[j][i];

	mat4 Tc = Translate(-eye.x, -eye.y, -eye.z);

	V = Rc*Tc;
		
	return V;
}

mat4 myOrtho(float l, float r, float b, float t, float zNear, float zFar)
{
	// Implement your own Ortho function
	mat4 V(1.0f);

	V[0][0] = 2/(r-l);
	V[1][1] = 2/(t-b);
	V[2][2] = 2/(zFar-zNear);
	V[0][3] = -(r+l)/(r-l);
	V[1][3] = -(t+b)/(t-b);
	V[2][3] = (zNear)/(zFar-zNear);

	return V;
}

mat4 myPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	mat4 P(1.0f);
	
	float rad = fovy * 3.141592 / 180.0;
	
	float sz = 1 / zFar;
	float h = zFar * tan(rad/2);
	
	float sy = 1/h;
	float w = h*aspectRatio;
	float sx = 1/w;

	mat4 S = Scale(sx, sy, sz);
	mat4 M(1.0f);

	float c = -zNear / zFar;
	M[2][2] = 1/(c+1);
	M[2][3] = -c/(c+1);
	M[3][2] = -1;
	M[3][3] = 0;

	P = M*S;

	return P;
}


void myInit()
{
	cube.Init();
	sphere.Init(40, 40);

	program = InitShader("vshader.glsl", "fshader.glsl");
	prog_phong = InitShader("vphong.glsl", "fphong.glsl");
}

void DrawAxis()
{
	glUseProgram(program);
	mat4 x_a= Translate(1.,0,0)*Scale(2,0.05,0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a= Translate(0,1.,0)*Scale(0.05,2,0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a= Translate(0,0,1.)*Scale(0.05,0.05,2);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

void DrawGrid()
{
	glUseProgram(program);
	float n = 40;
	float w = 10;
	float h = 10;

	for(int i=0; i<n; i++)
	{
		mat4 m = Translate(0,-0.5,-h+2*h/n*i)*Scale(w*2,0.02,0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for(int i=0; i<n; i++)
	{
		mat4 m = Translate(-w+2*w/n*i,-0.5,0)*Scale(0.02,0.02,h*2);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}

float g_Time = 0;

void display()
{
	glClearColor(0.7, 0.7, 0.7, 1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		
	mat4 ViewMat = myLookAt(vec3(2,2,2), vec3(0,0,0), vec3(0,1,0));

	float aspect = winWidth/(float)winHeight;
	float h = 1;
	
	//mat4 ProjMat = myOrtho(-h*aspect,h*aspect,-h,h,0,100);
	mat4 ProjMat = myPerspective(40, aspect,0.1,100);

	g_Mat = ProjMat*ViewMat;
	
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawAxis();
	DrawGrid();



	glUseProgram(prog_phong);
	GLuint uProjMat = glGetUniformLocation(prog_phong, "uProjMat");
	GLuint uModelMat = glGetUniformLocation(prog_phong, "uModelMat");

	mat4 ModelMat = RotateY(g_Time*30);
	glUniformMatrix4fv(uProjMat, 1, GL_TRUE, ProjMat);
	glUniformMatrix4fv(uModelMat, 1, GL_TRUE, ViewMat * ModelMat);
	
	// 1. define lights : Position, Color(Intensity)
	vec4 lpos = vec4(3*cos(g_Time), 3, 3 * sin(g_Time), 1);	// in World Coord.
	vec4 lcol = vec4(1, 1, 1, 1);	// white 
	
	// 2. material properties (phong coeff.)
	vec4 mAmb = vec4(0.2,0.2,0.3,1);
	vec4 mDif = vec4(0.6, 0.3, 0.3, 1);
	vec4 mSpec = vec4(0.3, 0.3, 0.3, 1);
	float shiness = 100.0;

	GLuint uLPos = glGetUniformLocation(prog_phong, "uLPos");
	GLuint uLCol = glGetUniformLocation(prog_phong, "uLCol");
	GLuint uAmb = glGetUniformLocation(prog_phong, "uAmb");
	GLuint uDif = glGetUniformLocation(prog_phong, "uDif");
	GLuint uSpec = glGetUniformLocation(prog_phong, "uSpec");
	GLuint uShiness = glGetUniformLocation(prog_phong, "uShiness");
	   
	glUniform4f(uLPos, lpos[0], lpos[1], lpos[2], lpos[3]);
	glUniform4f(uLCol, lcol[0], lcol[1], lcol[2], lcol[3]);
	glUniform4f(uAmb, mAmb[0], mAmb[1], mAmb[2], mAmb[3]);
	glUniform4f(uDif, mDif[0], mDif[1], mDif[2], mDif[3]);
	glUniform4f(uSpec, mSpec[0], mSpec[1], mSpec[2], mSpec[3]);
	glUniform1f(uShiness, shiness);


	sphere.Draw(prog_phong);
	
	glutSwapBuffers();
}


void idle()
{
	if(bPlay)
		g_Time += 0.016;
	Sleep(16);					// for vSync
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0,0,w,h);
	glutPostRedisplay();
}

void keyboard(unsigned char c, int x, int y)
{
	if (c == ' ')
		bPlay = !bPlay;

}

int main(int argc, char ** argv)
{	


	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(winWidth,winHeight);

	glutCreateWindow("OpenGL");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n",	glGetString(GL_VERSION),
									glGetString(GL_SHADING_LANGUAGE_VERSION));
		
	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
