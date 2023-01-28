#version 460 core

// 0, 1 = edges for lower film's margin
// 2, 3 = edges for upper film's margin
uniform sampler2D uImage;

uniform float uBrightness;
uniform float uContrast;
uniform float uExposure;
uniform float uSaturation;

uniform vec3 uTint;

uniform vec3 uFilmMarginColor;
uniform vec4 uFilmMarginEdges;

in vec2 textureCoords;

layout(location = 0) out vec3 outColor;

vec4 makeBlur(vec2 baseTextureCoords, vec2 imageTexelSize)
{
    const float blurKernel[25] = float[](
        1, 4, 7, 4, 1,
        4, 16, 26, 16, 4,
        7, 26, 41, 26, 7,
        4, 16, 26, 16, 4,
        1, 4, 7, 4, 1
    );

    vec4 color = vec4(0, 0, 0, 0);
    for (int i = -2; i < 3; ++i)
    {
        for (int j = -2; j < 3; ++j)
        {
            float blurWeight = blurKernel[(i + 2) + (j + 2) * 5] / 273.0;
            color += texture(uImage, baseTextureCoords + imageTexelSize * vec2(i, j)) * blurWeight;
        }
    }

    return color;
}

vec3 adjustBrightness(vec3 color, float brightness)
{
    return brightness + color;
}

vec3 adjustContrast(vec3 color, float contrast)
{
    return (1.0 + contrast) * (color - 0.5) + 0.5;
}

vec3 adjustExposure(vec3 color, float exposure)
{
    return (1.0 + exposure) * color;
}

vec3 adjustSaturation(vec3 color, float saturation)
{
    // https://www.w3.org/TR/WCAG21/#dfn-relative-luminance
    const vec3 luminosityFactor = vec3(0.2126, 0.7152, 0.0722);
    vec3 grayscale = vec3(dot(color, luminosityFactor));

    return mix(grayscale, color, 1.0 + saturation);
}

vec3 applyFilmMargins(vec3 color, vec3 filmMarginColor, vec4 filmMarginEdges)
{
    float lower_edge_weight = smoothstep(filmMarginEdges.x, filmMarginEdges.y, gl_FragCoord.y);
    float upper_edge_weight = 1.0 - smoothstep(filmMarginEdges.z, filmMarginEdges.w, gl_FragCoord.y);

    return mix(filmMarginColor, color, lower_edge_weight * upper_edge_weight);
}

void main()
{
    vec2 imageTexelSize = 1.0 / textureSize(uImage, 0);
    vec2 channelOffset = vec2(0.0, imageTexelSize.y * 2.0);

    vec3 color = vec3(0, 0, 0);
    color.x = makeBlur(textureCoords - channelOffset, imageTexelSize).x;
    color.y = makeBlur(textureCoords, imageTexelSize).y;
    color.z = makeBlur(textureCoords + channelOffset, imageTexelSize).z;

    color = applyFilmMargins(color, uFilmMarginColor, uFilmMarginEdges);

    color = adjustSaturation(color, uSaturation);
    color = adjustExposure(color, uExposure);
    color = adjustContrast(color, uContrast);
    color = adjustBrightness(color, uBrightness);

    color *= uTint;

    outColor = color;
}