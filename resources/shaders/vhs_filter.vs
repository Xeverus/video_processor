#version 460 core

uniform float uVerticalScale;

out vec2 textureCoords;

void main()
{
    const vec2 positions[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2(1.0, -1.0),
        vec2(-1.0, 1.0),
        vec2(1.0, 1.0)
    );
    const vec2 coords[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );

    textureCoords = coords[gl_VertexID];
    gl_Position = vec4(positions[gl_VertexID] * vec2(1.0, uVerticalScale), 0.0, 1.0);
}
