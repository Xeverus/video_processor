#pragma once

#include <string>
#include <vector>

#include <vid_lib/sprite/atlas.h>

namespace vid_lib::sprite
{

class GeometryGenerator
{
public:
    struct SpriteVertex
    {
        float screen_position_x;
        float screen_position_y;
        float texture_coord_x;
        float texture_coord_y;
    };

public:
    static std::vector<SpriteVertex>
    MakeVerticalText(const std::string& letters,
                     float screen_pos_x, float screen_pos_y,
                     float sprite_width, float sprite_height,
                     const Atlas& atlas);

    static SpriteVertex
    MakeSprite(const Atlas::SpriteDescription& sprite_description,
               float screen_pos_x, float screen_pos_y,
               float sprite_width, float sprite_height);
};

}