#version 330 core

in vec3 vPos;
in vec4 vColor;
in vec2 vUv;
in vec3 vNormal;

out vec4 color;

uniform sampler2DArray textureSampler;
flat in int FragTextureIndex;

uniform vec3 LightPos = vec3(-30.0,15.0,30.0);
void main( )
{
    vec3 L = LightPos - vPos;
    float D = clamp(dot(normalize(vNormal), L),0.0f,1.0f);
    vec4 col = vColor;
	col.a = 1.0f;
    vec4 TextureInfo = texture(textureSampler, vec3(vUv,float(FragTextureIndex)));
    vec4 textureColor = (TextureInfo * col);
    color = textureColor;
}