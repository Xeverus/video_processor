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
                     float sprite_size_x, float sprite_size_y,
                     const Atlas& atlas);

    // sprite = 6 vertices
    static std::vector<SpriteVertex>
    MakeSprite(char sprite_name,
               float screen_pos_x, float screen_pos_y,
               float sprite_size_x, float sprite_size_y,
               const Atlas& atlas);

private:
    // returns 4 vertices
    static std::vector<SpriteVertex> MakeSpriteVertices(const Atlas::SpriteDescription& sprite_description,
                                                        const float screen_pos_x, const float screen_pos_y,
                                                        const float sprite_size_x, const float sprite_size_y);

    static void RotateSpriteVertices(std::vector<SpriteVertex>& sprite_vertices);
};

}