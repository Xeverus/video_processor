#include <vid_lib/sprite/geometry_generator.h>

namespace vid_lib::sprite
{

Text GeometryGenerator::MakeVerticalText(const std::string& characters,
                                         float screen_pos_x, const float screen_pos_y,
                                         float sprite_width, float sprite_height,
                                         const vid_lib::sprite::Atlas& atlas)
{
    std::swap(sprite_width, sprite_height);

    std::vector<Sprite> letters;
    letters.reserve(characters.size());

    float sprite_index = 0.0f;
    for (const auto character: characters)
    {
        if (character == '\n')
        {
            screen_pos_x += sprite_height * 1.4f;
            sprite_index = 0;
            continue;
        }

        const auto& sprite_description = atlas.GetSpriteDescription(character);
        auto letter = MakeSprite(sprite_description,
                                 screen_pos_x,
                                 screen_pos_y + sprite_index * sprite_height,
                                 sprite_width,
                                 sprite_height);
        letters.push_back(letter);

        ++sprite_index;
    }

    return {std::move(letters), sprite_width, sprite_height};
}

Sprite GeometryGenerator::MakeSprite(const Atlas::SpriteDescription& sprite_description,
                                     const float screen_pos_x, const float screen_pos_y,
                                     const float sprite_width, const float sprite_height)
{
    const auto FindMidPoint = [](const float start, const float end)
    {
        return start + (end - start) * 0.5f;
    };

    return {
        screen_pos_x + sprite_width * 0.5f,
        screen_pos_y + sprite_height * 0.5f,
        FindMidPoint(sprite_description.texture_coord_x_start, sprite_description.texture_coord_x_end),
        FindMidPoint(sprite_description.texture_coord_y_start, sprite_description.texture_coord_y_end)
    };
}

}