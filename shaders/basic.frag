#version 430

in vec4 ge_Color;

//out vec4 ge_FragColor;

void main()
{
// 	ge_FragColor = vec4(1.0);
	gl_FragColor = ge_Color;
}
