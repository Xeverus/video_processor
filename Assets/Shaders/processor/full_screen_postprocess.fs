#version 460 core

layout(binding = 0) uniform sampler2D u_image;
uniform float u_time;

in vec2 v_textureCoords;

layout(location = 0) out vec3 out_color;

vec4 textureWithBlur3x3(sampler2D image, vec2 textureCoords, vec2 imageTexelSize)
{
    const float kernel[9] = float[](
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    );
    const float multiplier = 1.0 / 16.0;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            float weight = kernel[(i + 1) + (j + 1) * 3] * multiplier;
            vec2 texCoords = textureCoords + imageTexelSize * vec2(i, j);

            color += texture(image, texCoords) * weight;
        }
    }

    return color;
}

vec4 textureWithBlur5x5(sampler2D image, vec2 textureCoords, vec2 imageTexelSize)
{
    const float kernel[25] = float[](
        1, 4, 7, 4, 1,
        4, 16, 26, 16, 4,
        7, 26, 41, 26, 7,
        4, 16, 26, 16, 4,
        1, 4, 7, 4, 1
    );
    const float multiplier = 1.0 / 273.0;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = -2; i < 3; ++i)
    {
        for (int j = -2; j < 3; ++j)
        {
            float weight = kernel[(i + 2) + (j + 2) * 5] * multiplier;
            vec2 texCoords = textureCoords + imageTexelSize * vec2(i, j);

            color += texture(image, texCoords) * weight;
        }
    }

    return color;
}

vec4 textureWithSharpening3x3(sampler2D image, vec2 textureCoords, vec2 imageTexelSize)
{
    const float kernel[9] = float[](
        0, -1, 0,
        -1, 5, -1,
        0, -1, 0
    );
    const float multiplier = 1.0;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            float weight = kernel[(i + 1) + (j + 1) * 3] * multiplier;
            vec2 texCoords = textureCoords + imageTexelSize * vec2(i, j);

            color += texture(image, texCoords) * weight;
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

vec2 rotate(vec2 position, float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    return vec2(position.x * c - position.y * s, position.x * s + position.y * c);
}

float hash(vec2 p, float t)
{
    vec3 p3 = vec3(p, t);
    p3 = fract(p3 * 0.1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

float noise(vec2 p, float t)
{
    vec4 b = vec4(floor(p), ceil(p));
    vec2 f = smoothstep(0.0, 1.0, fract(p));
    return mix(mix(hash(b.xy, t), hash(b.zy, t), f.x), mix(hash(b.xw, t), hash(b.zw, t), f.x), f.y);
}

#define num_octaves 4
float fbm(vec2 pos)
{
    const float pi = 3.141592653589793;

    float value = 0.0;
    float scale = 2.0;
    float atten = 0.5;
    float t = 0.0;
    for(int i = 0; i < num_octaves; ++i)
    {
        t += atten;
        value += noise(pos * scale, float(i)) * atten;
        scale *= 12.0;
        atten *= 0.1;
        pos = rotate(pos, 0.125 * pi);
    }
    return value / t;
}

vec3 addLightArtifacts(vec3 color, vec2 textureCoords, float time, float amplitude)
{
    vec2 fbmArg = vec2(48.0, 12.0) * (textureCoords + vec2(time, 0.0));
    return color + fbm(fbmArg) * amplitude;
}

vec2 addVerticalDistortion(vec2 textureCoords, float time, float amplitude)
{
    vec2 fbmArg = vec2(32.0, 0.0) * (textureCoords + vec2(time * 0.04, 0.0));
    textureCoords.y += (fbm(fbmArg) - 0.5) * amplitude;
    return textureCoords;
}

void main()
{
    vec2 imageTexelSize = 1.0 / textureSize(u_image, 0);

    vec2 textureCoords = addVerticalDistortion(v_textureCoords, u_time, imageTexelSize.y * 1.75f);

    vec3 color = textureWithBlur5x5(u_image, textureCoords, imageTexelSize).xyz;
    color = addLightArtifacts(color, textureCoords, u_time, 0.025);

    color *= vec3(1.24, 1.0, 1.24);
    color = adjustSaturation(color, -0.1);
    color = adjustExposure(color, 0.4);
    color = adjustContrast(color, -0.3);
    color = adjustBrightness(color, -0.3);

    out_color = color;
}