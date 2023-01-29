#version 460 core

layout(binding = 0) uniform sampler2D u_image;

in vec2 v_textureCoords;

layout(location = 0) out vec3 out_color;

void main()
{
    vec2 imageTexelSize = 1.0 / textureSize(u_image, 0);
    vec2 channelOffset = vec2(0.0, imageTexelSize.y * 2.0);

    vec3 color = vec3(0.0, 0.0, 0.0);
    // thanks to doing more than one pass blur is more efficient if channel
    // offsets are in use because this time sampling is done 3 times but
    // thanks to this blur can be done in one pass instead of 3
    color.x = texture(u_image, v_textureCoords - channelOffset).x;
    color.y = texture(u_image, v_textureCoords).y;
    color.z = texture(u_image, v_textureCoords + channelOffset).z;

    out_color = color.xyz;
}