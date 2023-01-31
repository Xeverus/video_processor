#version 460 core

uniform float u_verticalScale;

out vec2 v_textureCoords;

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

    vec2 positionXY = positions[gl_VertexID] * vec2(1.0, u_verticalScale);
    gl_Position = vec4(positionXY, 0.0, 1.0);

    v_textureCoords = coords[gl_VertexID];
}
