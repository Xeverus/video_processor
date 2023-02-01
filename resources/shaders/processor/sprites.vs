#version 460 core

uniform vec2 u_spriteScreenSize;
uniform vec2 u_spriteTextureSize;
uniform float u_spriteRotation;

// instance values
layout(location = 0) in vec2 in_spritePosition;
layout(location = 1) in vec2 in_spriteTextureCoords;

out vec2 v_textureCoords;

vec2 computePosition(vec2 letterPosition, vec2 u_spriteScreenSize, float spriteRotation)
{
    const vec2 positions[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2(1.0, -1.0),
        vec2(-1.0, 1.0),
        vec2(1.0, 1.0)
    );

    // select position
    vec2 position = positions[gl_VertexID];

    // scale
    position *= u_spriteScreenSize * 0.5f;

    // rotate
    float s = sin(u_spriteRotation);
    float c = cos(u_spriteRotation);
    float x = position.x;
    float y = position.y;
    position.x = x * c - y * s;
    position.y = x * s + y * c;

    // translate
    return position + letterPosition;
}

vec2 computeRotation(vec2 textureCoords, vec2 spriteTextureSize)
{
    vec2 spriteTextureHalfSize = spriteTextureSize * 0.5f;

    // 0 = -,-
    // 1 = +,-
    // 2 = -,+
    // 3 = +,+
    float vid = float(gl_VertexID);
    textureCoords.x += spriteTextureHalfSize.x * (mod(vid, 2.0f) * 2.0f - 1.0f);
    textureCoords.y += spriteTextureHalfSize.y * (step(1.5f, vid) * 2.0f - 1.0f);

    return textureCoords;
}

void main()
{
    vec2 position = computePosition(in_spritePosition, u_spriteScreenSize, u_spriteRotation);
    vec2 textureCoords = computeRotation(in_spriteTextureCoords, u_spriteTextureSize);

    gl_Position = vec4(position, 0.0, 1.0);
    v_textureCoords = textureCoords;
}
