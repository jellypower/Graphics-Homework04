#version 330

#define PhongShading 0
#define GouradShading 1

in vec4 vPosition;
in vec4 vColor;
in vec3 vPhongNormal;
in vec3 vGouradNormal;

//out vec4 color;
out vec3 N3;
out vec3 L3;
out vec3 V3;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uLPos;
uniform vec4 uLCol;
uniform vec4 uAmb;
uniform vec4 uDif;
uniform vec4 uSpec;
uniform float uShiness;

uniform uint uShadingType;

void main()
{
	gl_Position  = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1,1,-1,1);	// z축 방향이 반대임

	vec4 lPos = uLPos;
	vec4 vPos = uModelMat * vPosition;
	
	vec4 N;

	switch(uShadingType){
		case PhongShading:
			N = uModelMat*vec4(vPhongNormal,0);
			break;
		case GouradShading:
			N = uModelMat*vec4(vGouradNormal,0);
			break;
		default:
			N = uModelMat*vec4(vPhongNormal,0);
			break;
	}
	vec4 L = lPos - vPos;
	N3 = normalize(N.xyz);
	L3 = normalize(L.xyz);
	V3 = normalize(vec3(0,0,0) - vPos.xyz);

}


