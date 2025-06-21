#include "gtest/gtest.h"

#include "tiled_matrix.h"
#include "impl.h"

class MaskTest : public ::testing::Test {
protected:
    int seq_len;
    int q_len;
    int group_size;
    int tile_size;

    std::unique_ptr<TiledMatrix<float> > matrix;

    void fill_matrix(float value) {
        for (int r = 0; r < seq_len; ++r) {
            for (int c = 0; c < matrix->get_total_cols(); ++c) {
                matrix->at(r, c) = value;
            }
        }
    }
};

TEST(TiledMatrixTest, TiledAccessWriteAndVerifyWithGlobalRead) {
    TiledMatrix<float> matrix(50, 10, 1, 16);

    // 全局坐标(35, 8)对应:
    // tile_id = 35 / 16 = 2
    // local_row = 35 % 16 = 3
    int tile_id = 2;
    int local_row = 3;
    int col = 8;

    auto tile = matrix.get_tile(tile_id);
    tile.at(local_row, col) = 999.9f;

    EXPECT_FLOAT_EQ(matrix.at(35, 8), 999.9f);
}

// 测试用例1: 一个Tile完全位于需要掩码的区域内
TEST_F(MaskTest, TileFullyInMaskedRegion) {
    // 设置: 一个tile正好等于掩码区域
    // seq_len = 64, q_len = 32, tile_size = 32
    // 掩码区域为最后32行，即 [32, 63]
    // Tile 1 的区域也是 [32, 63]
    seq_len = 64;
    q_len = 32;
    group_size = 1;
    tile_size = 32;
    matrix = std::make_unique<TiledMatrix<float> >(
        seq_len, q_len, group_size, tile_size);
    fill_matrix(1.0f);

    std::vector<uint32_t> causal_mask(q_len, 0);
    uint32_t row_mask_value = 0;
    for (int j = 0; j < q_len; j += 2) {
        row_mask_value |= (1U << j);
    }
    for (int i = 0; i < q_len; ++i) {
        causal_mask[i] = row_mask_value;
    }

    int tile_id = 1;
    auto tile_view = matrix->get_tile(tile_id);

    apply_mask(tile_view, seq_len, q_len, group_size, tile_size, tile_id, causal_mask);

    for (int r = 0; r < tile_size; ++r) {
        for (int c = 0; c < matrix->get_total_cols(); ++c) {
            int mask_j = c / group_size;
            if (mask_j % 2 == 0) {
                // 偶数列块，应保留
                EXPECT_FLOAT_EQ(tile_view.at(r, c), 1.0f);
            } else {
                // 奇数列块，应置零
                EXPECT_FLOAT_EQ(tile_view.at(r, c), 0.0f);
            }
        }
    }
}

// 测试用例2: Tile跨越了掩码开始的边界
TEST_F(MaskTest, TileStraddlingMaskBoundary) {
    // 设置: seq_len = 50, q_len = 30, tile_size = 32
    // 掩码区域为最后30行, 即 [20, 49]
    // Tile 0 的区域是 [0, 31]。其中[0,19]行不应被mask, [20,31]行应被mask
    seq_len = 50;
    q_len = 30;
    group_size = 2;
    tile_size = 32;
    matrix = std::make_unique<TiledMatrix<float> >(
        seq_len, q_len, group_size, tile_size);
    fill_matrix(1.0f);

    // 掩码: 全部置零 (all bits are 0)
    std::vector<uint32_t> causal_mask(q_len, 0);

    int tile_id = 0;
    auto tile_view = matrix->get_tile(tile_id);

    // 执行
    apply_mask(tile_view, seq_len, q_len, group_size, tile_size, tile_id, causal_mask);

    // 验证
    for (int r = 0; r < tile_size; ++r) {
        const int global_row = tile_id * tile_size + r;
        for (int c = 0; c < matrix->get_total_cols(); ++c) {
            if (global_row < 20) {
                // 在掩码区之前
                EXPECT_FLOAT_EQ(tile_view.at(r, c), 1.0f) << "Row " << r << " should not be masked.";
            } else {
                // 在掩码区之内
                EXPECT_FLOAT_EQ(tile_view.at(r, c), 0.0f) << "Row " << r << " should be masked.";
            }
        }
    }
}

// 测试用例3: 最后一个Tile，同时包含有效数据和Padding
TEST_F(MaskTest, LastTileWithPadding) {
    // 设置: seq_len = 50, q_len = 30, tile_size = 32
    // 掩码区域为 [20, 49]
    // Tile 1 的区域是 [32, 63]。其中 [32,49] 是有效数据且在掩码区，[50,63]是padding
    seq_len = 50;
    q_len = 30;
    group_size = 1;
    tile_size = 32;
    matrix = std::make_unique<TiledMatrix<float> >(
        seq_len, q_len, group_size, tile_size);
    fill_matrix(1.0f);

    // 掩码: 只保留第0列 (j=0)
    std::vector<uint32_t> causal_mask(q_len, 1U << 0);

    int tile_id = 1;
    auto tile_view = matrix->get_tile(tile_id);

    // 执行
    apply_mask(tile_view, seq_len, q_len, group_size, tile_size, tile_id, causal_mask);

    // 验证
    for (int r = 0; r < tile_size; ++r) {
        const int global_row = tile_id * tile_size + r;
        for (int c = 0; c < matrix->get_total_cols(); ++c) {
            if (global_row >= seq_len) {
                // Padding区域
                EXPECT_FLOAT_EQ(tile_view.at(r, c), 0.0f) << "Padding at row " << r << " should be zero.";
            } else {
                // 有效数据区域 (全都在掩码区内)
                int mask_j = c / group_size;
                if (mask_j == 0) {
                    // 第0列保留
                    EXPECT_FLOAT_EQ(tile_view.at(r, c), 1.0f);
                } else {
                    // 其他列置零
                    EXPECT_FLOAT_EQ(tile_view.at(r, c), 0.0f);
                }
            }
        }
    }
}


// 测试用例4: Tile完全在掩码区域之前，不应有任何改变
TEST_F(MaskTest, TileBeforeMaskRegion) {
    // 设置: seq_len = 100, q_len = 32, tile_size = 32
    // 掩码区域为 [68, 99]
    // Tile 0 的区域是 [0, 31], 完全在掩码区之前
    seq_len = 100;
    q_len = 32;
    group_size = 1;
    tile_size = 32;
    matrix = std::make_unique<TiledMatrix<float> >(
        seq_len, q_len, group_size, tile_size);
    fill_matrix(1.0f);

    // 掩码: 全部置零 (一个最强的掩码)
    std::vector<uint32_t> causal_mask(q_len, 0);

    int tile_id = 0;
    auto tile_view = matrix->get_tile(tile_id);

    // 执行
    apply_mask(tile_view, seq_len, q_len, group_size, tile_size, tile_id, causal_mask);

    // 验证: tile中所有值都应该保持不变
    for (int r = 0; r < tile_size; ++r) {
        for (int c = 0; c < matrix->get_total_cols(); ++c) {
            EXPECT_FLOAT_EQ(tile_view.at(r, c), 1.0f) << "Value at (" << r << "," << c << ") should not be changed.";
        }
    }
}
