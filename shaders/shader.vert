#version 450

layout(binding = 0) uniform UBOVP
{
    mat4 proj;
    mat4 view;
} uboVP;

// NOT IN USE
layout(binding = 1) uniform UboModel
{
    mat4 model;
} uboModel;

layout(push_constant) uniform PushModel
{
    mat4 model;
} pushModel;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 outCol;

void main()
{
    gl_Position = uboVP.proj * uboVP.view * pushModel.model * vec4(pos, 1.0);
    outCol = col;
}
