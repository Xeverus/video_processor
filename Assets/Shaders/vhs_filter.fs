#version 460 core

uniform sampler2D image;

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
            color += texture(image, baseTextureCoords + imageTexelSize * vec2(i, j)) * blurWeight;
        }
    }

    return color;
}

void main()
{
    vec2 imageTexelSize = 1.0 / textureSize(image, 0);
    vec2 channelOffset = vec2(0.0, imageTexelSize.y * 4.0f);

    vec4 red = makeBlur(textureCoords - channelOffset, imageTexelSize);
    vec4 green = makeBlur(textureCoords, imageTexelSize);
    vec4 blue = makeBlur(textureCoords + channelOffset, imageTexelSize);

    outColor = vec3(red.x, green.y, blue.z);
}