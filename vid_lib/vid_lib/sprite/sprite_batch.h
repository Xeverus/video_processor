#pragma once

#include <vector>

#include <vid_lib/sprite/sprite.h>

namespace vid_lib::sprite
{

class SpriteBatch
{
public:
    SpriteBatch(std::vector<Sprite> sprites,
                float sprite_screen_width,
                float sprite_screen_height);

    const std::vector<Sprite>& GetSprites() const;

    float GetSpriteScreenWidth() const;

    float GetSpriteScreenHeight() const;

    std::size_t GetSpriteCount() const;

private:
    std::vector<Sprite> letters_;
    float letter_screen_width_;
    float letter_screen_height_;
};

}