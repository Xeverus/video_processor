#include <vid_lib/sprite/atlas.h>

#include <fstream>
#include <stdexcept>

namespace vid_lib::sprite
{

Atlas::Atlas(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("cannot open font metadata file");
    }

    LoadAtlasSpritesDataFromStream(file);
}

Atlas::Atlas(std::istream& stream)
{
    LoadAtlasSpritesDataFromStream(stream);
}

void Atlas::LoadAtlasSpritesDataFromStream(std::istream& stream)
{
    float image_width = 1.0f;
    float image_height = 1.0f;
    stream >> sprite_texture_width_ >> sprite_texture_height_ >> image_width >> image_height;
    sprite_texture_width_ /= image_width;
    sprite_texture_height_ /= image_height;

    std::string line;
    std::getline(stream, line);

    for (float line_idx = 0.0f; std::getline(stream, line);)
    {
        float sprite_index = 0.0f;
        for (char sprite_name: line)
        {
            sprite_descriptions_[sprite_name] = {
                sprite_index * sprite_texture_width_,
                (sprite_index + 1.0f) * sprite_texture_width_,
                line_idx * sprite_texture_height_,
                (line_idx + 1.0f) * sprite_texture_height_,
            };
            ++sprite_index;
        }
        ++line_idx;
    }
}

const Atlas::SpriteDescription& Atlas::GetSpriteDescription(char sprite_name) const
{
    const auto iter = sprite_descriptions_.find(sprite_name);
    if (iter == sprite_descriptions_.end())
    {
        throw std::runtime_error(std::string("no sprite with name '") + sprite_name + "'");
    }

    return iter->second;
}

float Atlas::GetSpriteTextureWidth() const
{
    return sprite_texture_width_;
}

float Atlas::GetSpriteTextureHeight() const
{
    return sprite_texture_height_;
}

std::vector<char> Atlas::GetValidSpriteNames() const
{
    std::vector<char> sprite_names;
    sprite_names.reserve(sprite_descriptions_.size());
    for (const auto& [name, description] : sprite_descriptions_)
    {
        sprite_names.push_back(name);
    }

    return sprite_names;
}

}