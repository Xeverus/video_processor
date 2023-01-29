#version 460 core

layout(binding = 0) uniform sampler2D u_image;
layout(binding = 1) uniform sampler2D u_fontImage;

in vec2 v_textureCoords;

layout(location = 0) out vec3 out_color;

void main()
{
    vec3 color = texture(u_fontImage, v_textureCoords).xyz;

    out_color = color;
}