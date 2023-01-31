#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "vid_lib/opengl/opengl.h"

namespace vid_lib::sprite
{

class Atlas
{
public:
    struct SpriteDescription
    {
        float texture_coord_x_start;
        float texture_coord_x_end;
        float texture_coord_y_start;
        float texture_coord_y_end;
    };

public:
    explicit Atlas(const std::string& filepath);
    explicit Atlas(std::istream& stream);

    const SpriteDescription& GetSpriteDescription(char sprite_name) const;
    std::vector<char> GetValidSpriteNames() const;

    float GetSpriteTextureWidth() const;
    float GetSpriteTextureHeight() const;

private:
    std::unordered_map<char, SpriteDescription> sprite_descriptions_;
    float sprite_texture_width_;
    float sprite_texture_height_;

private:
    void LoadAtlasSpritesDataFromStream(std::istream& stream);
};

}