#version 450

layout(set = 0, binding = 0) uniform UBOVP
{
    mat4 proj;
    mat4 view;
} uboVP;

layout(set = 2, binding = 0) uniform BoneTransforms
{
    mat4 bones[100];
} boneTransforms;

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
    mat4 boneTransform = boneTransforms.bones[boneIds[0]] * weights[0];
    boneTransform += boneTransforms.bones[boneIds[1]] * weights[1];
    boneTransform += boneTransforms.bones[boneIds[2]] * weights[2];
    boneTransform += boneTransforms.bones[boneIds[3]] * weights[3];

    gl_Position = uboVP.proj * uboVP.view * pushModel.model * boneTransform * vec4(pos, 1.0);
    outCol = col;
    outTex = tex;
}