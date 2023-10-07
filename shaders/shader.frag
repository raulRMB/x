#version 450

layout(location = 0) in vec3 inCol;
layout(location = 1) in vec2 inTex;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outCol;

void main()
{
    outCol = texture(texSampler, inTex) * vec4(inCol, 1.0);
}