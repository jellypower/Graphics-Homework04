#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#pragma once

using namespace std;
#include <vgl.h>
#include <vec.h>
#include <string>
#include <vector>
#include <string.h>


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


class Mesh {


public:
	char* const FileName = _fileName;
	Mesh(const char* fileName, ShadingType ShadeType);
	~Mesh();

	vec3* vertBuffer;
	int GetVertBufLen();
	
	indiceInTriangle* indexBuffer;
	int GetIdxBufLen();

	vec3* normalBuffer;
	int GetNormalBufLen();
	

private:
	unsigned int vertBufLen;
	unsigned int idxBufLen;
	unsigned int normalBufLen;

	vector<vec3>* _vertBuf;
	vector<indiceInTriangle>* _idxBuf;

	ShadingType ShadeType;

	char _fileName[100];
	
	
	void trimStr(char* str);
	void parseFileToVert(FILE *file);
	void readFile(const char* fileName);


};


Mesh::Mesh(const char* fileName, ShadingType ShadeType) {

	_vertBuf = new vector<vec3>();
	_idxBuf = new vector<indiceInTriangle>();

	this->ShadeType = ShadeType;

	readFile(fileName);


}

Mesh::~Mesh() {

	delete []_vertBuf;
	delete []_idxBuf;

}

int Mesh::GetIdxBufLen() {
	return idxBufLen;
}

int Mesh::GetVertBufLen() {
	return vertBufLen;
}

int Mesh::GetNormalBufLen() {
	return normalBufLen;
}

void Mesh::trimStr(char* str) {

	int n = 0;

	for (n = 0; n < 100; n++) {
		if (str[n] != ' ') break;
		n++;
	}

	for (int i = 0; i < 100 - n; i++) {
		str[i] = str[i + n];
	}

}

void Mesh::parseFileToVert(FILE* file) {

	char str[100];

	do {
		float x, y, z;

		fgets(str, 100, file);
		trimStr(str);

		

		if (str[0] == 'v') {
			sscanf(str, "v %f %f %f", &x, &y, &z);
			_vertBuf->push_back(vec3(x, y, z));
		}

	} while (!feof(file));

	rewind(file);

	do {
		int a, b, c;

		fgets(str, 100, file);
		trimStr(str);

		if (str[0] == 'f') {
			sscanf(str, "f %f %f %f", &a, &b, &c);
			_idxBuf->push_back(indiceInTriangle(a, b, c)); // 여어기부터 하자
		}
	} while (!feof(file));


	vertBufLen = _vertBuf->size();
	idxBufLen = _idxBuf->size();

	vertBuffer = _vertBuf->data();
	indexBuffer = _idxBuf->data();

	normalBufLen = idxBufLen * 3;


}

void Mesh::readFile(const char* fileName) {
	
	strcpy(_fileName, fileName);
	FILE* meshFile = fopen(fileName, "r");

	parseFileToVert(meshFile);


	fclose(meshFile);

}
