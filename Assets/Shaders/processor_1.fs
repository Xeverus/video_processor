#version 460 core

uniform sampler2D u_image;

in vec2 v_textureCoords;

layout(location = 0) out vec3 out_color;

vec4 textureWithBlur3x3(sampler2D image, vec2 textureCoords, vec2 imageTexelSize)
{
    const float blurKernel[9] = float[](
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    );
    const float divider = 1.0 / 16.0;

    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            float blurWeight = blurKernel[(i + 1) + (j + 1) * 3] * divider;
            vec2 texCoords = textureCoords + imageTexelSize * vec2(i, j);

            color += texture(image, texCoords) * blurWeight;
        }
    }

    return color;
}

vec4 textureWithBlur5x5(sampler2D image, vec2 textureCoords, vec2 imageTexelSize)
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
            vec2 texCoords = textureCoords + imageTexelSize * vec2(i, j);

            color += texture(image, texCoords) * blurWeight;
        }
    }

    return color;
}

void main()
{
    vec2 imageTexelSize = 1.0 / textureSize(u_image, 0);

    vec4 color = textureWithBlur3x3(u_image, v_textureCoords, imageTexelSize);
    //vec4 color = textureWithBlur5x5(u_image, v_textureCoords, imageTexelSize);

    out_color = color.xyz;
}