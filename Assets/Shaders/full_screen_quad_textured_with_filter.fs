#version 330 core

uniform sampler2D image;

in vec2 textureCoords;

void main()
{
    vec4 color = texture(image, textureCoords);
    color.y *= 0.9;

    gl_FragColor = color;
}