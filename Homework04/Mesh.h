#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma once

using namespace std;
#include <vgl.h>
#include <vec.h>
#include <string>
#include <vector>
#include <string.h>
#include <time.h>

enum ShadingType{
	PhongShading,
	GouradShading
};

struct indiceInTriangle {
	GLuint idx[3];

	indiceInTriangle(int a, int b, int c) {
		idx[0] = a;
		idx[1] = b;
		idx[2] = c;
	}
};

struct VertexInfo
{
	vec4 position;
	vec4 color;
	vec3 PhongNormal;
	vec3 GouradNormal;
};

inline vec3 myCross(vec3 a, vec3 b) {
	return
		vec3(a.y*b.z-a.z*b.y, a.z*b.x - a.x * b.z, a.x*b.y - a.y*b.x);
}

void trimStr(char* str) {

	int n = 0;

	for (n = 0; n < 100; n++) {
		if (str[n] != ' ') break;
		n++;
	}

	for (int i = 0; i < 100 - n; i++) {
		str[i] = str[i + n];
	}

}

class Mesh {


public:
	char* const FileName = _fileName;
	Mesh(const char* fileName);
	~Mesh();


	VertexInfo *Vertices;
	int GetVertLen();
	vec3 GetMinPos();
	vec3 GetMaxPos();
	vec3 GetCenterPos();

	

private:
	unsigned int vertLen = 0;

	
	vector<vec3>* tempVert;
	vector<indiceInTriangle>* tempIdx;



	ShadingType ShadeType;

	char _fileName[100];
	
	
	void parseFileToVertObj(FILE *file);
	void readTempVertFromObj(const char* fileName);
	void genVertInfoFromTempVert();
	void releaseTempVertInfo();
	void computeMinMaxPos();
	void setPivotAsCenter();

	vec3 minPos;
	vec3 maxPos;
	vec3 centerPos;
};


Mesh::Mesh(const char* fileName) {

	ShadeType = PhongShading;

	tempVert = new vector<vec3>();
	tempIdx = new vector<indiceInTriangle>();

	clock_t s, f;
	double duration;

	s = clock();
	readTempVertFromObj(fileName);
	f = clock();
	duration = (double)(f - s) / CLOCKS_PER_SEC;
	printf("readTempVertFromObj %f段\n", duration);

	s = clock();
	genVertInfoFromTempVert();
	f = clock();
	duration = (double)(f - s) / CLOCKS_PER_SEC;
	printf("genVertInfoFromTempVert %f段\n", duration);

	s = clock();
	releaseTempVertInfo();
	f = clock();
	duration = (double)(f - s) / CLOCKS_PER_SEC;
	printf("releaseTempVertInfo% f段\n", duration);
	
	s = clock();
	computeMinMaxPos();
	f = clock();
	duration = (double)(f - s) / CLOCKS_PER_SEC;
	printf("computeMinMaxPos %f段\n", duration);
	
	s = clock();
	setPivotAsCenter();
	f = clock();
	duration = (double)(f - s) / CLOCKS_PER_SEC;
	printf("setPivotAsCenter %f段\n", duration);
}

Mesh::~Mesh() {


}

vec3 Mesh::GetMinPos() { return minPos; }
vec3 Mesh::GetMaxPos() { return maxPos; }
vec3 Mesh::GetCenterPos() { return centerPos; }
int Mesh::GetVertLen() { return vertLen; }


void Mesh::parseFileToVertObj(FILE* file) {

	char str[100];

	do {
		float x, y, z;
		int a, b, c;
	
		fgets(str, 100, file);
		
		trimStr(str);

		

		if (str[0] == 'v') {
			sscanf(str, "v %f %f %f", &x, &y, &z);
			tempVert->push_back(vec3(x, y, z));
		}
		else if (str[0] == 'f') {
			sscanf(str, "f %d %d %d", &a, &b, &c);
			tempIdx->push_back(indiceInTriangle(a - 1, b - 1, c - 1));
		}

	} while (!feof(file));


}

void Mesh::readTempVertFromObj(const char* fileName) {
	
	strcpy(_fileName, fileName);
	FILE* meshFile = fopen(fileName, "r");

	parseFileToVertObj(meshFile);


	fclose(meshFile);

}



void Mesh::genVertInfoFromTempVert() {
	
	int tempIdxLen = tempIdx->size();
	vertLen = tempIdxLen * 3;



	Vertices = new VertexInfo[vertLen];
	for (int i = 0; i < vertLen; i++) { // init vertices
		Vertices[i].color = vec4(1,1,1,1);
		Vertices[i].GouradNormal = vec3(0,0,0);
		Vertices[i].PhongNormal = vec3(0, 0, 0);
		Vertices[i].position = vec4(0,0,0,0);
	}


	vec3* phongNormalAcc = new vec3[tempVert->size()]; // phong normal for each vertex
	for (int i = 0; i < tempVert->size(); i++) { phongNormalAcc[i] = vec3(0, 0, 0); }

	for (int i = 0; i < tempIdxLen; i++) { // gouraud normal

		vec3 a = tempVert->at(tempIdx->at(i).idx[0]);
		vec3 b = tempVert->at(tempIdx->at(i).idx[1]);
		vec3 c = tempVert->at(tempIdx->at(i).idx[2]);

		vec3 n = normalize(myCross(b-a, c-a));

		
		for (int j = 0; j < 3; j++) {
			Vertices[3 * i + j].position = tempVert->at(tempIdx->at(i).idx[j]);
			Vertices[3 * i + j].GouradNormal = n; // gourad


			phongNormalAcc[tempIdx->at(i).idx[j]] += n; // phong

		}
	}

	for (int i = 0; i < tempIdxLen; i++) { // phong normal

		for (int j = 0; j < 3; j++) {
			Vertices[3 * i + j].PhongNormal = normalize(phongNormalAcc[tempIdx->at(i).idx[j]]);
		}
	}	



	delete[] phongNormalAcc;
}

void Mesh::releaseTempVertInfo() {
	delete tempVert;
	delete tempIdx;

	tempVert = NULL;
	tempIdx = NULL;
}

void Mesh::computeMinMaxPos() {

	float max = numeric_limits<float>::max();
	float min = numeric_limits<float>::min();

	minPos = vec3(max, max ,max);
	maxPos = vec3(min, min, min);

	for (int i = 0; i < vertLen; i++) {

		if (Vertices[i].position.x < minPos.x) minPos = Vertices[i].position.x;
		if (Vertices[i].position.y < minPos.y) minPos = Vertices[i].position.y;
		if (Vertices[i].position.z < minPos.z) minPos = Vertices[i].position.z;

		if (Vertices[i].position.x > maxPos.x) maxPos = Vertices[i].position.x;
		if (Vertices[i].position.y > maxPos.y) maxPos = Vertices[i].position.y;
		if (Vertices[i].position.z > maxPos.z) maxPos = Vertices[i].position.z;


	}

	centerPos = minPos / 2 + maxPos / 2;

}

void Mesh::setPivotAsCenter() {
	for (int i = 0; i < vertLen; i++) {
		
		Vertices[i].position -= centerPos;
		Vertices[i].position.w = 1;
	}
}
