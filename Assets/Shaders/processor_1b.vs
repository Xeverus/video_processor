#version 460 core

layout(location = 0) in vec2 in_letterPosition;
layout(location = 1) in vec2 in_letterTextureCoords;

out vec2 v_textureCoords;

void main()
{
    gl_Position = vec4(in_letterPosition, 0.0, 1.0);
    v_textureCoords = in_letterTextureCoords;
}
