#version 330 core

uniform sampler2D image;

layout(location = 0) out vec3 outColor;

in vec2 textureCoords;

void main()
{
    outColor = texture(image, textureCoords).xyz;
}