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

uniform float uRandomSeed;

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
    const float divider = 1.0 / 273.0;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = -2; i < 3; ++i)
    {
        for (int j = -2; j < 3; ++j)
        {
            float blurWeight = blurKernel[(i + 2) + (j + 2) * 5] * divider;
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

vec3 applyFilmMargins(vec3 color, vec3 filmMarginColor, vec4 filmMarginEdges, float fragCoordY)
{
    float lower_edge_weight = smoothstep(filmMarginEdges.x, filmMarginEdges.y, fragCoordY);
    float upper_edge_weight = 1.0 - smoothstep(filmMarginEdges.z, filmMarginEdges.w, fragCoordY);

    return mix(filmMarginColor, color, lower_edge_weight * upper_edge_weight);
}

float makeNoise(float seed)
{
    float f = fract(seed);
    float s = sign(fract(seed * 0.5) - 0.5);
    float k = fract(floor(seed) * 0.1731);

    return s * f * (f - 1.0) * ((16.0 * k - 4.0) * f * (f - 1.0) - 1.0);
}

void main()
{
    vec2 imageTexelSize = 1.0 / textureSize(uImage, 0);
    vec2 channelOffset = vec2(0.0, imageTexelSize.y * 2.0);

    float noise = makeNoise(gl_FragCoord.x * imageTexelSize.x * 32.0 + uRandomSeed * 0.2);
    vec2 texCoords = textureCoords + noise * imageTexelSize.x;

    vec3 color = vec3(0, 0, 0);
    color.x = makeBlur(texCoords - channelOffset, imageTexelSize).x;
    color.y = makeBlur(texCoords, imageTexelSize).y;
    color.z = makeBlur(texCoords + channelOffset, imageTexelSize).z;

    color = applyFilmMargins(color, uFilmMarginColor, uFilmMarginEdges, gl_FragCoord.y + noise);

    color = adjustSaturation(color, uSaturation);
    color = adjustExposure(color, uExposure);
    color = adjustContrast(color, uContrast);
    color = adjustBrightness(color, uBrightness);

    color *= uTint;

    outColor = color;
}