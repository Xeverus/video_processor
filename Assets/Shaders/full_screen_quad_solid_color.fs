#version 330 core

in vec2 textureCoords;

void main()
{
    gl_FragColor = vec4(1.0, textureCoords.x, textureCoords.y, 1.0);
}