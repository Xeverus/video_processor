#version 460 core

uniform sampler2D image;

in vec2 textureCoords;

layout(location = 0) out vec3 outColor;

vec4 makeBlur(vec2 baseTextureCoords, vec2 blurStep)
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
            color += texture(image, baseTextureCoords + blurStep * vec2(i, j)) * blurWeight;
        }
    }

    return color;
}

void main()
{
    ivec2 imageSize = textureSize(image, 0);
    vec2 blurStep = 1.0 / imageSize;

    vec4 color = makeBlur(textureCoords, blurStep);

    outColor = color.xyz;
}