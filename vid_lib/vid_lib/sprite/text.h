#pragma once

#include <vector>

#include <vid_lib/sprite/sprite.h>

namespace vid_lib::sprite
{

class Text
{
public:
    Text(std::vector<Sprite> letters, float letter_screen_width, float letter_screen_height);

    const std::vector<Sprite>& GetLetters() const;

    float GetLetterScreenWidth() const;
    float GetLetterScreenHeight() const;
    int GetLength() const;

private:
    std::vector<Sprite> letters_;
    float letter_screen_width_;
    float letter_screen_height_;
};

}