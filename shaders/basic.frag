// /!\ Shader designed for OpenGL43 backend /!\

#version 420 core
#extension GL_ARB_bindless_texture : require

flat in sampler2D ge_Texture0;
flat in int ge_Texture0Base;
flat in float ge_HasTexture;
in vec4 ge_Color;
in vec3 ge_TextureCoord;
in vec3 ge_Normal;
in vec3 ge_Position;

out vec4 ge_FragColor;
out uint ge_FragDepth;
out vec3 ge_FragNormal;
out vec3 ge_FragPosition;

void main()
{
	if ( ge_HasTexture != 0.0 ) {
		ge_FragColor = ge_Color * texture2D( ge_Texture0, ge_TextureCoord.xy );
	} else {
		ge_FragColor = ge_Color;
	}

	ge_FragDepth = uint( gl_FragCoord.z * 65535.0 );
	ge_FragNormal = ge_Normal * 0.5 + vec3(0.5);
	ge_FragPosition = ge_Position;
}
