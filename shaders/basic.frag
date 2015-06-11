// /!\ Shader designed for OpenGL43 backend /!\

#version 420 core
#extension GL_ARB_bindless_texture : require

flat in sampler2D ge_Texture0;
flat in int ge_Texture0Base;
in vec4 ge_Color;
in vec3 ge_TextureCoord;

void main()
{
	gl_FragColor = ge_Color * texture2D( ge_Texture0, ge_TextureCoord.xy );
	gl_FragColor.a = 1.0;
}
