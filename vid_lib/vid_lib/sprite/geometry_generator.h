#pragma once

#include <string>
#include <vector>

#include <vid_lib/sprite/atlas.h>
#include <vid_lib/sprite/sprite.h>
#include <vid_lib/sprite/text.h>

namespace vid_lib::sprite
{

class GeometryGenerator
{
public:
    static Text MakeVerticalText(const std::string& letters,
                                 float screen_pos_x, float screen_pos_y,
                                 float sprite_width, float sprite_height,
                                 const Atlas& atlas);

    static Sprite MakeSprite(const Atlas::SpriteDescription& sprite_description,
                             float screen_pos_x, float screen_pos_y,
                             float sprite_width, float sprite_height);
};

}