#include "impl.h"

void apply_mask(TileView<float>& tile, int seq_len, int q_len, int group_size, int tile_size,
    int tile_id, BitMaskVector& mask) {
    const int mask_start_row = seq_len - q_len;
    const int tile_start_row = tile_id * tile_size;

    const int total_cols = q_len * group_size;

    for (int row = 0; row < tile_size; ++row) {
        const int global_row = tile_start_row + row;
        if (global_row >= seq_len) {
            for (int col = 0; col < total_cols; ++col) {
                tile.at(row, col) = 0.0f;
            }
            continue;
        }

        if (global_row >= mask_start_row) {
            const int mask_i = global_row - mask_start_row;
            const uint32_t row_bitmask = mask[mask_i];
            for (int col = 0; col < total_cols; ++col) {
                const int mask_j = col / group_size;
                const uint32_t col_bit = 1U << mask_j;
                if ((row_bitmask & col_bit) == 0)
                {
                    tile.at(row, col) = 0.0f;
                }

            }
        }
    }
}