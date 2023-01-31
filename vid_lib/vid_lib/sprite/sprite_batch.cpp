#include <vid_lib/sprite/sprite_batch.h>

namespace vid_lib::sprite
{

SpriteBatch::SpriteBatch(std::vector<Sprite> letters,
                         const float letter_screen_width,
                         const float letter_screen_height)
    : letters_(std::move(letters))
    , letter_screen_width_(letter_screen_width)
    , letter_screen_height_(letter_screen_height)
{
}

const std::vector<Sprite>& SpriteBatch::GetSprites() const
{
    return letters_;
}

float SpriteBatch::GetSpriteScreenWidth() const
{
    return letter_screen_width_;
}

float SpriteBatch::GetSpriteScreenHeight() const
{
    return letter_screen_height_;
}

std::size_t SpriteBatch::GetSpriteCount() const
{
    return letters_.size();
}

}