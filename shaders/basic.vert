// /!\ Shader designed for OpenGL43 backend /!\

#version 420 core
#extension GL_ARB_shader_draw_parameters : require
#extension GL_ARB_bindless_texture : require

#define geTexture2D(x) sampler2D( ge_Textures[ ge_TextureBase + x ].xy )
#define geTexture3D(x) sampler3D( ge_Textures[ ge_TextureBase + x ].xy )

layout(location = 0) in vec3 ge_VertexTexcoord;
layout(location = 1) in vec4 ge_VertexColor;
layout(location = 2) in vec3 ge_VertexNormal;
layout(location = 3) in vec3 ge_VertexPosition;
layout(location = 7 /* 8 9 10 */) in mat4 ge_ObjectMatrix;
layout(location = 11) in int ge_TextureBase;

layout (binding=0, std140) uniform ge_Matrices_0
{
	mat4 ge_ProjectionMatrix;
};

layout (binding=1, std140) uniform ge_Matrices_1
{
	mat4 ge_ViewMatrix;
};

layout (binding=2, std140) uniform ge_Textures_0
{
	uvec4 ge_Textures[256];
};

flat out sampler2D ge_Texture0;
flat out int ge_Texture0Base;
out vec4 ge_Color;
out vec3 ge_TextureCoord;

void main()
{
	ge_Texture0Base = ge_TextureBase;
	ge_Texture0 = geTexture2D(0);
	ge_Color = ge_VertexColor;
	ge_TextureCoord = ge_VertexTexcoord;

	gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * ge_ObjectMatrix * vec4(ge_VertexPosition, 1.0);
}
