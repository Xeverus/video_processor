#version 460 core

uniform sampler2D image;

in vec2 textureCoords;

layout(location = 0) out vec3 outColor;

void main()
{
    vec4 color = texture(image, textureCoords);
    color.y *= 0.9;

    outColor = color.xyz;
}