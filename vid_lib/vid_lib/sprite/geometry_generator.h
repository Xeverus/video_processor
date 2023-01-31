#pragma once

#include <memory>
#include <string>

#include <vid_lib/math/random.h>

#include <vid_lib/sprite/atlas.h>
#include <vid_lib/sprite/sprite.h>
#include <vid_lib/sprite/sprite_batch.h>

namespace vid_lib::sprite
{

class GeometryGenerator
{
public:
    static std::unique_ptr<SpriteBatch> MakeVerticalText(const std::string& letters,
                                                         float screen_pos_x, float screen_pos_y,
                                                         float sprite_width, float sprite_height,
                                                         const Atlas& atlas);

    static std::unique_ptr<vid_lib::sprite::SpriteBatch> MakeRandomlyPlacedSprites(const vid_lib::sprite::Atlas& atlas,
                                                                                   int sprite_count,
                                                                                   float sprite_screen_width,
                                                                                   float sprite_screen_height,
                                                                                   math::Random& random_source);

    static Sprite MakeSprite(const Atlas::SpriteDescription& sprite_description,
                             float screen_pos_x, float screen_pos_y,
                             float sprite_width, float sprite_height);
};

}