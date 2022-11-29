#pragma once

#include <vgl.h>
#include <vec.h>
#include "Mesh.h"



class MyMeshRenderer
{
public:
	MyMeshRenderer(void);
	~MyMeshRenderer(void);


	GLuint Init(const char *fileName);
	void SetPositionAndOtherAttribute(GLuint program);

	Mesh* MeshInfo;

	ShadingType ShadeType;

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	void Draw(GLuint program);
};

MyMeshRenderer::MyMeshRenderer(void)
{
	bInitialized = false;
}

MyMeshRenderer::~MyMeshRenderer(void)
{
	delete MeshInfo;
}






GLuint MyMeshRenderer::Init(const char* fileName)
{
	// The Cube should be initialized only once;
	if (bInitialized == true) return vao;

	MeshInfo = new Mesh(fileName);
	ShadeType = PhongShading;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, MeshInfo->GetVertLen() * sizeof(VertexInfo), MeshInfo->Vertices, GL_STATIC_DRAW);


	bInitialized = true;
	return vao;
}

void MyMeshRenderer::SetPositionAndOtherAttribute(GLuint program)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vPhongNormal = glGetAttribLocation(program, "vPhongNormal");
	glEnableVertexAttribArray(vPhongNormal);
	glVertexAttribPointer(vPhongNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4)));

	GLuint vGouradNormal = glGetAttribLocation(program, "vGouradNormal");
	glEnableVertexAttribArray(vGouradNormal);
	glVertexAttribPointer(vGouradNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4) + sizeof(vec3)));

}


void MyMeshRenderer::Draw(GLuint program)
{
	if (!bInitialized) return;			// check whether it is initiazed or not. 

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndOtherAttribute(program);

	glDrawArrays(GL_TRIANGLES, 0, MeshInfo->GetVertLen());
}