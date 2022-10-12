#version 330 core
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec3 vPos;
out vec2 vUv;
out vec3 vNormal;

uniform mat4 Transform;

void main( )
{
    gl_Position = Transform * vec4( position, 1.0f );
    vPos = gl_Position.xyz;
    vUv = uv;
    vNormal = normal;
}