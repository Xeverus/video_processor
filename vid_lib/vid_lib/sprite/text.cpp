#include <vid_lib/sprite/text.h>

namespace vid_lib::sprite
{

Text::Text(std::vector<Sprite> letters, float letter_screen_width, float letter_screen_height)
    : letters_(std::move(letters))
    , letter_screen_width_(letter_screen_width)
    , letter_screen_height_(letter_screen_height)
{
}

const std::vector<Sprite>& Text::GetLetters() const
{
    return letters_;
}

float Text::GetLetterScreenWidth() const
{
    return letter_screen_width_;
}

float Text::GetLetterScreenHeight() const
{
    return letter_screen_height_;
}

int Text::GetLength() const
{
    return static_cast<int>(letters_.size());
}

}