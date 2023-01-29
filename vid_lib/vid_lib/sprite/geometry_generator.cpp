#include <vid_lib/sprite/geometry_generator.h>

namespace vid_lib::sprite
{

std::vector<GeometryGenerator::SpriteVertex>
GeometryGenerator::MakeVerticalText(const std::string& letters,
                                    const float screen_pos_x, const float screen_pos_y,
                                    const float sprite_size_x, const float sprite_size_y,
                                    const vid_lib::sprite::Atlas& atlas)
{
    std::vector<SpriteVertex> text;
    text.reserve(letters.size() * 6);

    float sprite_index = 0.0f;
    for (const auto letter: letters)
    {
        const auto letter_sprite = MakeRotatedSprite(letter,
                                              screen_pos_x,
                                              screen_pos_y + sprite_index * sprite_size_y,
                                              sprite_size_x,
                                              sprite_size_y,
                                              atlas);
        text.insert(text.end(), letter_sprite.begin(), letter_sprite.end());

        ++sprite_index;
    }

    return text;
}

std::vector<GeometryGenerator::SpriteVertex>
GeometryGenerator::MakeSprite(const char sprite_name,
                              const float screen_pos_x, const float screen_pos_y,
                              const float sprite_size_x, const float sprite_size_y,
                              const Atlas& atlas)
{
    const auto& sprite_description = atlas.GetSpriteDescription(sprite_name);

    const auto topLeft = SpriteVertex
        {
            screen_pos_x,
            screen_pos_y,
            sprite_description.texture_coord_x_start,
            sprite_description.texture_coord_y_start
        };
    const auto topRight = SpriteVertex
        {
            screen_pos_x + sprite_size_x,
            screen_pos_y,
            sprite_description.texture_coord_x_end,
            sprite_description.texture_coord_y_start
        };
    const auto bottomRight = SpriteVertex
        {
            screen_pos_x + sprite_size_x,
            screen_pos_y + sprite_size_y,
            sprite_description.texture_coord_x_end,
            sprite_description.texture_coord_y_end
        };
    const auto bottomLeft = SpriteVertex
        {
            screen_pos_x,
            screen_pos_y + sprite_size_y,
            sprite_description.texture_coord_x_start,
            sprite_description.texture_coord_y_end
        };

    return {
        topLeft, topRight, bottomRight, topLeft, bottomRight, bottomLeft
    };
}

// Same as MakeSprite but shifts tex coords by one vertex
std::vector<GeometryGenerator::SpriteVertex>
GeometryGenerator::MakeRotatedSprite(const char sprite_name,
                                     const float screen_pos_x, const float screen_pos_y,
                                     const float sprite_size_x, const float sprite_size_y,
                                     const Atlas& atlas)
{
    const auto& sprite_description = atlas.GetSpriteDescription(sprite_name);

    const auto topLeft = SpriteVertex
        {
            screen_pos_x,
            screen_pos_y,
            sprite_description.texture_coord_x_start,
            sprite_description.texture_coord_y_end
        };
    const auto topRight = SpriteVertex
        {
            screen_pos_x + sprite_size_x,
            screen_pos_y,
            sprite_description.texture_coord_x_start,
            sprite_description.texture_coord_y_start
        };
    const auto bottomRight = SpriteVertex
        {
            screen_pos_x + sprite_size_x,
            screen_pos_y + sprite_size_y,
            sprite_description.texture_coord_x_end,
            sprite_description.texture_coord_y_start
        };
    const auto bottomLeft = SpriteVertex
        {
            screen_pos_x,
            screen_pos_y + sprite_size_y,
            sprite_description.texture_coord_x_end,
            sprite_description.texture_coord_y_end
        };

    return {
        topLeft, topRight, bottomRight, topLeft, bottomRight, bottomLeft
    };
}

}