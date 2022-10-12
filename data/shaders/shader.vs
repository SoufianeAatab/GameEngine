#version 330 core
in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 uv;
in int TextureIndex;

out vec4 vColor;
out vec3 vPos;
out vec2 vUv;
out vec3 vNormal;
flat out int FragTextureIndex;

uniform mat4 Transform;

void main( )
{
    gl_Position = Transform * vec4( position, 1.0f );
    vColor = vec4(color,1.0);
	//vColor = vec4(UColor,1.0);

    vPos = gl_Position.xyz;
    vUv = uv;
    FragTextureIndex = TextureIndex;
    vNormal = normal;
}