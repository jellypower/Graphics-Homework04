#pragma once

#include <vgl.h>
#include <vec.h>

struct VertexInfo
{
	vec4 position;
	vec4 color;
	vec3 normal;
};

struct Transform {

	vec3 position;
	vec3 rotation;
	vec3 scale;

};

const int NumVertices = 100;


class MyMeshRenderer
{
public:
	MyMeshRenderer(void);
	~MyMeshRenderer(void);


	VertexInfo Vertices[NumVertices];
	GLuint Init();
	void SetPositionAndColorAttribute(GLuint program);

	void ReadObjFile(char *fileName);

	void ColorCube(vec4* vin, vec4* cin);
	void Quad(int a, int b, int c, int d, vec4* vin, vec4* cin);
	vec3 getNormal(vec4 a, vec4 b, vec4 c);

	int NumCurVertices;

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	void Draw(GLuint program);
};

void MyMeshRenderer::ReadObjFile(char* fileName) {

}

MyMeshRenderer::MyMeshRenderer(void)
{
	bInitialized = false;
	NumCurVertices = 0;
}

MyMeshRenderer::~MyMeshRenderer(void)
{
}

vec3 MyMeshRenderer::getNormal(vec4 a, vec4 b, vec4 c)
{
	vec3 p0(a.x, a.y, a.z);
	vec3 p1(b.x, b.y, b.z);
	vec3 p2(c.x, c.y, c.z);

	vec3 p = p1 - p0;
	vec3 q = p2 - p0;
	vec3 n = cross(p, q);
	n = normalize(n);
	return n;

}

void MyMeshRenderer::Quad(int a, int b, int c, int d, vec4* vin, vec4* cin)
{
	vec3 n = getNormal(vin[a], vin[b], vin[c]);
	Vertices[NumCurVertices].position = vin[a]; Vertices[NumCurVertices].color = cin[a];
	Vertices[NumCurVertices].normal = n;
	NumCurVertices++;
	Vertices[NumCurVertices].position = vin[b]; Vertices[NumCurVertices].color = cin[b];
	Vertices[NumCurVertices].normal = n;
	NumCurVertices++;
	Vertices[NumCurVertices].position = vin[c]; Vertices[NumCurVertices].color = cin[c];
	Vertices[NumCurVertices].normal = n;
	NumCurVertices++;

	n = getNormal(vin[a], vin[c], vin[d]);
	Vertices[NumCurVertices].position = vin[a]; Vertices[NumCurVertices].color = cin[a];
	Vertices[NumCurVertices].normal = n;
	NumCurVertices++;
	Vertices[NumCurVertices].position = vin[c]; Vertices[NumCurVertices].color = cin[c];
	Vertices[NumCurVertices].normal = n;
	NumCurVertices++;
	Vertices[NumCurVertices].position = vin[d]; Vertices[NumCurVertices].color = cin[d];
	Vertices[NumCurVertices].normal = n;
	NumCurVertices++;
}

void MyMeshRenderer::ColorCube(vec4* vin, vec4* cin)
{
	Quad(1, 0, 3, 2, vin, cin);
	Quad(2, 3, 7, 6, vin, cin);
	Quad(3, 0, 4, 7, vin, cin);
	Quad(6, 5, 1, 2, vin, cin);
	Quad(4, 5, 6, 7, vin, cin);
	Quad(5, 4, 0, 1, vin, cin);
}


GLuint MyMeshRenderer::Init()
{
	// The Cube should be initialized only once;
	if (bInitialized == true) return vao;

	vec4 vertex_positions[8] = {
		vec4(-0.5, -0.5,  0.5, 1.0),
		vec4(-0.5,  0.5,  0.5, 1.0),
		vec4(0.5,  0.5,  0.5, 1.0),
		vec4(0.5, -0.5,  0.5, 1.0),
		vec4(-0.5, -0.5, -0.5, 1.0),
		vec4(-0.5,  0.5, -0.5, 1.0),
		vec4(0.5,  0.5, -0.5, 1.0),
		vec4(0.5, -0.5, -0.5, 1.0),
	};

	vec4 vertex_colors[8] = {
		vec4(0, 0, 0, 1),
		vec4(1, 0, 0, 1),
		vec4(1, 1, 1, 1),
		vec4(0, 1, 0, 1),
		vec4(0, 0, 1, 1),
		vec4(1, 0, 1, 1),
		vec4(1, 1, 0, 1),
		vec4(0, 1, 1, 1)
	};

	ColorCube(vertex_positions, vertex_colors);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);


	bInitialized = true;
	return vao;
}

void MyMeshRenderer::SetPositionAndColorAttribute(GLuint program)
{


	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4)));

}


void MyMeshRenderer::Draw(GLuint program)
{
	if (!bInitialized) return;			// check whether it is initiazed or not. 

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndColorAttribute(program);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}