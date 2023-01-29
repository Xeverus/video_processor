#version 460 core

layout(binding = 0) uniform sampler2D u_image;

// 0, 1 = edges for lower film's margin
// 2, 3 = edges for upper film's margin
uniform vec4 u_filmMarginEdges;
uniform vec3 u_filmMarginColor;

in vec2 v_textureCoords;

layout(location = 0) out vec3 out_color;

vec3 applyFilmMargins(vec3 color, vec3 filmMarginColor, vec4 filmMarginEdges, float fragCoordY)
{
    float lower_edge_weight = smoothstep(filmMarginEdges.x, filmMarginEdges.y, fragCoordY);
    float upper_edge_weight = 1.0 - smoothstep(filmMarginEdges.z, filmMarginEdges.w, fragCoordY);

    return mix(filmMarginColor, color, lower_edge_weight * upper_edge_weight);
}

void main()
{
    vec3 color = texture(u_image, v_textureCoords).xyz;
    color = applyFilmMargins(color, u_filmMarginColor, u_filmMarginEdges, gl_FragCoord.y);

    out_color = color;
}