#include <vid_lib/math/film.h>

namespace vid_lib::math
{

std::array<float, 4> Film::CalculateMarginEdges(int image_dimension, int margin_size, int margin_step_size)
{
    const auto image_dimension_f = static_cast<float>(image_dimension);
    const auto margin_edge_start_f = static_cast<float>(margin_size);
    const auto margin_edge_end_f = static_cast<float>(margin_size + margin_step_size);

    return {
        margin_edge_start_f,
        margin_edge_end_f,
        image_dimension_f - margin_edge_end_f,
        image_dimension_f - margin_edge_start_f
    };
}

}