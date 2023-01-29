#include <vid_lib/sprite/raster_text.h>

namespace vid_lib::sprite
{

std::vector<RasterText::Letter>
RasterText::MakeVerticalText(const std::string& letters,
                             const float screen_pos_x, const float screen_pos_y,
                             const float letter_size_x, const float letter_size_y,
                             const vid_lib::sprite::Atlas& atlas)
{
    std::vector<Letter> text;
    text.reserve(letters.size());

    float letter_index = 0.0f;
    for (const auto letter: letters)
    {
        const auto letter_metadata = atlas.GetSprite(letter);
        const auto topLeft = Letter
            {
                screen_pos_x,
                screen_pos_y + letter_index * letter_size_x,
                letter_metadata.letter_coord_x_start,
                letter_metadata.letter_coord_y_end
            };
        const auto topRight = Letter
            {
                screen_pos_x,
                screen_pos_y + (letter_index + 1.0f) * letter_size_x,
                letter_metadata.letter_coord_x_end,
                letter_metadata.letter_coord_y_end
            };
        const auto bottomRight = Letter
            {
                screen_pos_x + letter_size_y,
                screen_pos_y + (letter_index + 1.0f) * letter_size_x,
                letter_metadata.letter_coord_x_end,
                letter_metadata.letter_coord_y_start
            };
        const auto bottomLeft = Letter
            {
                screen_pos_x + letter_size_y,
                screen_pos_y + letter_index * letter_size_x,
                letter_metadata.letter_coord_x_start,
                letter_metadata.letter_coord_y_start
            };
        text.push_back(topLeft);
        text.push_back(topRight);
        text.push_back(bottomRight);
        text.push_back(topLeft);
        text.push_back(bottomRight);
        text.push_back(bottomLeft);

        ++letter_index;
    }

    return text;
}

}