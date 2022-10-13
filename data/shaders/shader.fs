#version 330 core

in vec3 vPos;
in vec4 vColor;
in vec2 vUv;
in vec3 vNormal;

out vec4 color;

uniform sampler2DArray textureSampler;
flat in int FragTextureIndex;

uniform vec3 LightPos1 = vec3(32.0,0.0,-10.0);
uniform vec3 LightPos3 = vec3(-22.0,0.0,-10.0);
uniform vec3 LightPos = vec3(-22.0,0.0,26.0);
uniform vec3 LightPos2 = vec3(32.0,.0,26.0);

uniform vec3 SpotLight = vec3(0.0,0.0,0.0);

uniform float r0 = 5.0;

// BaseColor * dist to light
// color = color0 * distance_fall_off
vec3 PointLight()
{
	// fwin(r) = clamp(0,(1 - (r/rmax)^4))^2
	return vec3(1.0,1.0,1.0);
}

void main( )
{
    vec3 L = LightPos - vPos;
    float D = clamp(dot(normalize(vNormal), L),0.0f,1.0f);
    vec4 col = vColor;
	col.a = 1.0f;
    vec4 TextureInfo = texture(textureSampler, vec3(vUv,float(FragTextureIndex)));
    vec4 textureColor = (TextureInfo );

    vec3 cLight0 = TextureInfo.xyz * 0.2; 

	vec3 d = LightPos - vPos;
	float r = sqrt(dot(d,d));

	vec3 d1 = LightPos1 - vPos;
	float r1 = sqrt(dot(d1,d1));

	vec3 d2 = LightPos2 - vPos;
	float r2 = sqrt(dot(d2,d2));

	vec3 d3 = LightPos3 - vPos;
	float r3 = sqrt(dot(d3,d3));
	
	vec3 cLight = cLight0 * ((r0*r0)/(r*r) + 0.1);
	cLight += cLight0 * ((r0*r0)/(r1*r1) + 0.1);
	cLight += cLight0 * ((r0*r0)/(r2*r2) + 0.1);
	cLight += cLight0 * ((r0*r0)/(r3*r3) + 0.1);
	

	// SPOTLIGHT
	vec3 d4 = SpotLight - vPos;
	float os = dot(vec3(0.0,-1.0,0.0), -normalize(d4));
// Inner Cutoff
	float op = radians(12.5);
// Outer Cufoff	
	float ou = radians(17.5);
	float t = clamp((cos(os) - cos(ou))/(cos(op) - cos(ou)), 0.0, 1.0);
// op - ou (inner cone - outer cone)
	
	float fdir = t*t;// * (3.0-2.0*t);

	float r4 = sqrt(dot(d4,d4));
	
	cLight += cLight0 * ((r0*r0)/(r4*r4)+0.1) * fdir;
	
	color = vec4(cLight, TextureInfo.a);
}