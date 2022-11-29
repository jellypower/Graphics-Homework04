#version 330

//in  vec4 color;
in vec3 N3;
in vec3 L3;
in vec3 V3;

out vec4 fColor;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uLPos;
uniform vec4 uLCol;
uniform vec4 uAmb;
uniform vec4 uDif;
uniform vec4 uSpec;
uniform float uShiness;

void main()
{
	vec3 N = normalize(N3);
	vec3 L = normalize(L3);
	vec3 V = normalize(V3);
	vec3 R = 2*dot(L, N)*N - L;	

	float d =  max(dot(N, L),0);


	float s = pow(max(dot(V,R),0), uShiness);

	vec4 amb = 2*uAmb * uLCol ;
	vec4 dif = uDif*uLCol * d;
	vec4 spec = uSpec*uLCol* s;

	fColor = amb + dif + spec;


}
