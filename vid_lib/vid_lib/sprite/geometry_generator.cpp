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
    text.reserve(letters.size() * 6);

    float sprite_index = 0.0f;
    for (const auto letter: letters)
    {
        const auto& sprite_description = atlas.GetSpriteDescription(letter);
        auto vertices = MakeSpriteVertices(sprite_description,
                                           screen_pos_x,
                                           screen_pos_y + sprite_index * sprite_height,
                                           sprite_width,
                                           sprite_height);
        RotateSpriteVertices(vertices);

        text.push_back(vertices[0]);
        text.push_back(vertices[1]);
        text.push_back(vertices[2]);
        text.push_back(vertices[0]);
        text.push_back(vertices[2]);
        text.push_back(vertices[3]);

        ++sprite_index;
    }

    return text;
}

std::vector<GeometryGenerator::SpriteVertex>
GeometryGenerator::MakeSprite(const char sprite_name,
                              const float screen_pos_x, const float screen_pos_y,
                              const float sprite_width, const float sprite_height,
                              const Atlas& atlas)
{
    const auto& sprite_description = atlas.GetSpriteDescription(sprite_name);
    const auto vertices = MakeSpriteVertices(sprite_description,
                                             screen_pos_x, screen_pos_y,
                                             sprite_width, sprite_height);
    return {
        vertices[0], vertices[1], vertices[2], vertices[0], vertices[2], vertices[3]
    };
}

std::vector<GeometryGenerator::SpriteVertex>
GeometryGenerator::MakeSpriteVertices(const Atlas::SpriteDescription& sprite_description,
                                      const float screen_pos_x, const float screen_pos_y,
                                      const float sprite_width, const float sprite_height)
{
    const auto topLeft = SpriteVertex
        {
            screen_pos_x,
            screen_pos_y,
            sprite_description.texture_coord_x_start,
            sprite_description.texture_coord_y_start
        };
    const auto topRight = SpriteVertex
        {
            screen_pos_x + sprite_width,
            screen_pos_y,
            sprite_description.texture_coord_x_end,
            sprite_description.texture_coord_y_start
        };
    const auto bottomRight = SpriteVertex
        {
            screen_pos_x + sprite_width,
            screen_pos_y + sprite_height,
            sprite_description.texture_coord_x_end,
            sprite_description.texture_coord_y_end
        };
    const auto bottomLeft = SpriteVertex
        {
            screen_pos_x,
            screen_pos_y + sprite_height,
            sprite_description.texture_coord_x_start,
            sprite_description.texture_coord_y_end
        };

    return {topLeft, topRight, bottomRight, bottomLeft};
}

void GeometryGenerator::RotateSpriteVertices(std::vector<SpriteVertex>& sprite_vertices)
{
    for (auto i = 0; i < 3; ++i)
    {
        std::swap(sprite_vertices[0].texture_coord_x, sprite_vertices[i + 1].texture_coord_x);
        std::swap(sprite_vertices[0].texture_coord_y, sprite_vertices[i + 1].texture_coord_y);
    }
}

}