#pragma once
#include "tiled_matrix.h"

using BitMaskVector = const std::vector<uint32_t>;

void apply_mask(TileView<float>& tile, int seq_len, int q_len, int group_size, int tile_size,
    int tile_id, BitMaskVector& mask);