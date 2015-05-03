#version 430
/*
struct s_strided_0 {
	vec4 value;
	vec4 stride1;
	vec4 stride2;
	vec4 stride3;
};
struct s_strided_1 {
	vec4 stride1;
	vec4 value;
	vec4 stride2;
	vec4 stride3;
};
struct s_strided_2 {
	vec4 stride1;
	vec4 stride2;
	vec4 value;
	vec4 stride3;
};
struct s_strided_3 {
	vec4 stride1;
	vec4 stride2;
	vec4 stride3;
	vec4 value;
};


layout( std140, binding = 0 ) buffer _VertexTexcoord {
    s_strided_0 ge_VertexTexcoord[];
};

layout( std140, binding = 1 ) buffer _VertexColor {
    s_strided_1 ge_VertexColor[];
};

layout( std140, binding = 2 ) buffer _VertexNormal {
    s_strided_2 ge_VertexNormal[];
};

layout( std140, binding = 3 ) buffer _VertexPosition {
    s_strided_3 ge_VertexPosition[];
};

layout( std140, binding = 10 ) uniform _ProjectionMatrix {
	mat4 ge_ProjectionMatrix;
};

layout( std140, binding = 11 ) uniform _ViewMatrix {
	mat4 ge_ViewMatrix;
};

layout( std140, binding = 12 ) buffer _ObjectMatrix {
	mat4 ge_ObjectMatrix[];
};


out vec4 ge_Color;

void main()
{
// 	ge_Color = ge_Vertex[gl_VertexID].color;
	ge_Color = ge_VertexColor[gl_VertexID].value;

// 	gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * ge_ObjectMatrix[gl_InstanceID] * vec4(ge_Vertex[gl_VertexID].position.xyz, 1.0);
	gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * ge_ObjectMatrix[0] * vec4(ge_VertexPosition[gl_VertexID].value.xyz, 1.0);
	gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * vec4(ge_VertexPosition[gl_VertexID].value.xyz, 1.0);
}
*/

layout(location = 0) in vec3 ge_VertexTexcoord;
layout(location = 1) in vec4 ge_VertexColor;
layout(location = 2) in vec3 ge_VertexNormal;
layout(location = 3) in vec3 ge_VertexPosition;

layout (binding=10, std140) uniform ge_Matrices_0
{
	mat4 ge_ProjectionMatrix;
};

layout (binding=11, std140) uniform ge_Matrices_1
{
	mat4 ge_ViewMatrix;
};

layout (std140, binding=12) uniform ge_Matrices_2
{
	mat4 ge_ObjectMatrix[];
};


out vec4 ge_Color;

void main()
{
	ge_Color = ge_VertexColor;

	gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * ge_ObjectMatrix[0] * vec4(ge_VertexPosition, 1.0);
// 	gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * vec4(ge_VertexPosition, 1.0);
}
