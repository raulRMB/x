#version 450

layout(set = 0, binding = 0) uniform UBOVP
{
    mat4 proj;
    mat4 view;
} uboVP;

// NOT IN USE
layout(set = 0, binding = 1) uniform UboModel
{
    mat4 model;
} uboModel;

layout(push_constant) uniform PushModel
{
    mat4 model;
} pushModel;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 tex;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

layout(location = 0) out vec3 outCol;
layout(location = 1) out vec2 outTex;

void main()
{
    gl_Position = uboVP.proj * uboVP.view * pushModel.model * vec4(pos, 1.0);
    outCol = col;
    outTex = tex;
}