#pragma once

#include <string>
#include <vector>

#include <vid_lib/sprite/atlas.h>

namespace vid_lib::sprite
{

class RasterText
{
public:
    struct Letter
    {
        float screen_pos_x;
        float screen_pos_y;
        float letter_coord_x;
        float letter_coord_y;
    };

public:
    static std::vector<Letter>
    MakeVerticalText(const std::string& letters,
                     float screen_pos_x, float screen_pos_y,
                     float letter_size_x, float letter_size_y,
                     const vid_lib::sprite::Atlas& atlas);
};

}