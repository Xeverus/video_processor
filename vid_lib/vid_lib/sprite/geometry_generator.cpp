#include <vid_lib/sprite/geometry_generator.h>

namespace vid_lib::sprite
{

std::vector<GeometryGenerator::SpriteVertex>
GeometryGenerator::MakeVerticalText(const std::string& letters,
                                    const float screen_pos_x, const float screen_pos_y,
                                    float sprite_width, float sprite_height,
                                    const vid_lib::sprite::Atlas& atlas)
{
    std::swap(sprite_width, sprite_height);

    std::vector<SpriteVertex> text;
    text.reserve(letters.size());

    float sprite_index = 0.0f;
    for (const auto letter: letters)
    {
        const auto& sprite_description = atlas.GetSpriteDescription(letter);
        auto sprite = MakeSprite(sprite_description,
                                 screen_pos_x,
                                 screen_pos_y + sprite_index * sprite_height,
                                 sprite_width,
                                 sprite_height);
        text.push_back(sprite);

        ++sprite_index;
    }

    return text;
}

GeometryGenerator::SpriteVertex
GeometryGenerator::MakeSprite(const Atlas::SpriteDescription& sprite_description,
                              const float screen_pos_x, const float screen_pos_y,
                              const float sprite_width, const float sprite_height)
{
    const auto FindMidPoint = [](const float start, const float end)
    {
        return start + (end - start) * 0.5f;
    };

    return
        {
            screen_pos_x + sprite_width * 0.5f,
            screen_pos_y + sprite_height * 0.5f,
            FindMidPoint(sprite_description.texture_coord_x_start, sprite_description.texture_coord_x_end),
            FindMidPoint(sprite_description.texture_coord_y_start, sprite_description.texture_coord_y_end)
        };
}

}