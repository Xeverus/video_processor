#pragma once

#include <string>
#include <unordered_map>

#include "vid_lib/opengl/opengl.h"

namespace vid_lib::sprite
{

class Atlas
{
public:
    struct Sprite
    {
        float letter_coord_x_start;
        float letter_coord_x_end;
        float letter_coord_y_start;
        float letter_coord_y_end;
    };

public:
    explicit Atlas(const std::string& filepath);
    explicit Atlas(std::istream& stream);

    const Sprite& GetSprite(char sprite_name) const;

private:
    std::unordered_map<char, Sprite> sprites_;
    float sprite_width_;
    float sprite_height_;

private:
    void LoadAtlasSpritesDataFromStream(std::istream& stream);
};

}