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
	if(d>0.8) d = 1;
	else if(d>0.5) d = 0.8;
	else if(d>0.2) d = 0.5;
	else d = 0;

	float s = pow(max(dot(V,R),0), uShiness);
	if(s>0.2) s= 1.0f;
	else s = 0.0f;

	vec4 amb = 2*uAmb * uLCol ;
	vec4 dif = uDif*uLCol * d;
	vec4 spec = uSpec*uLCol* s;

	fColor = amb + dif + spec;

	if(abs(dot(N, V))<0.2)
		fColor = vec4(0,0,0,1);

}
