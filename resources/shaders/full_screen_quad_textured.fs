#version 330 core

uniform sampler2D image;

in vec2 textureCoords;

void main()
{
    // gl_FragColor = vec4(1.0, textureCoords.x, textureCoords.y, 1.0);
    gl_FragColor = texture(image, textureCoords);
}