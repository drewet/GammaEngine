// /!\ Shader designed for OpenGL43 backend /!\

#version 420 core
#extension GL_ARB_bindless_texture : require

const float g = 0.990;

const vec3 bR = vec3(5.266626e-006, 1.230601e-005, 3.004397e-005);
const vec3 bM = vec3(21.0e-6);

uniform vec3 v3CameraPos = vec3(0.0);
uniform vec3 v3SunPos = vec3(10000000.0, 5000000.0, 10000000.0);
uniform vec3 v3SunPos2 = vec3(5000000.0, -10000000.0, 10000000.0);
in vec3 vpos;
in vec3 sumR;
in vec3 sumM;

vec3 CalcSun( vec3 SunPosition )
{
	vec3 dir = vpos - v3CameraPos;
	float mu = dot(normalize(dir), normalize(SunPosition));
	float phaseR = (3.0 / (16.0 * 3.141528)) * (1.0 + mu * mu);
	float phaseM = (3.0 / (8.0 * 3.141528)) * ((1.0 - g * g) * (1.0 + mu * mu)) / ((2.0 + g * g) * pow(1.0 + g * g - 2.0 * g * mu, 1.5));
	return 20.0 * (phaseR * sumR * bR * 2.0 + phaseM * sumM * bM);
}

void main()
{

	vec3 color = CalcSun( v3SunPos );
// 	color += CalcSun( v3SunPos2 );
	gl_FragColor = vec4(color, 1.0);
//	gl_FragColor.a = clamp(3.0 * (color.r + color.g + color.b), 0.0, 1.0);
	gl_FragColor.a = clamp(3.0*color.r + 2.0*color.g + 1.5*color.b, 0.4, 1.0);

	float fExposure = 4.0;
	gl_FragColor.rgb = vec3(1.0) - exp2( -fExposure * gl_FragColor.rgb );

	gl_FragColor.a = 1.0;

//	gl_FragDepth = 1.0;
}
