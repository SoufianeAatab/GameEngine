#version 330 core
in vec3 vPos;
in vec2 vUv;
in vec3 vNormal;

out vec4 color;

uniform sampler2D textureSampler;
uniform vec3 Color;

uniform vec3 LightPos = vec3(-30.0,15.0,30.0);
void main( )
{
    vec3 L = LightPos - vPos;
    float D = clamp(dot(normalize(vNormal), L),0.0f,1.0f);
    color = vec4(Color,1.0);
}