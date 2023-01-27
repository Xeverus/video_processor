#version 460 core

uniform vec2 aspectRatio;

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
    gl_Position = vec4(positions[gl_VertexID] * aspectRatio, 0.0, 1.0);
}
