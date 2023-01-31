#include <vid_lib/sprite/geometry_generator.h>

namespace vid_lib::sprite
{

std::unique_ptr<SpriteBatch> GeometryGenerator::MakeVerticalText(const std::string& characters,
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

    return std::make_unique<SpriteBatch>(std::move(letters), sprite_width, sprite_height);
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

std::unique_ptr<vid_lib::sprite::SpriteBatch>
GeometryGenerator::MakeRandomlyPlacedSprites(const Atlas& atlas,
                                             const int sprite_count,
                                             const float sprite_screen_width,
                                             const float sprite_screen_height,
                                             math::Random& random_source)
{
    std::vector<vid_lib::sprite::Sprite> decals;
    decals.reserve(sprite_count);

    const auto sprite_names = atlas.GetValidSpriteNames();
    for (auto i = 0; i < sprite_count; ++i)
    {
        const char decal_name = sprite_names[random_source.GetNextInt() % sprite_names.size()];
        const auto sprite_description = atlas.GetSpriteDescription(decal_name);
        const auto pos_x = (random_source.GetNextFloat() * 2.0f - 1.0f) * (1.0f - sprite_screen_width * 0.5f);
        const auto pos_y = (random_source.GetNextFloat() * 2.0f - 1.0f) * (1.0f - sprite_screen_height * 0.5f);
        const auto decal = vid_lib::sprite::GeometryGenerator::MakeSprite(sprite_description, pos_x, pos_y,
                                                                          sprite_screen_width,
                                                                          sprite_screen_height);
        decals.push_back(decal);
    }

    return std::make_unique<SpriteBatch>(std::move(decals), sprite_screen_width, sprite_screen_height);
}

}