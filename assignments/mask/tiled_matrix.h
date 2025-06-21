#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include <functional>

template<typename T>
class TiledMatrix;

template<typename T>
class TileView {
public:
    using Accessor = std::function<T&(int, int)>;

    TileView(Accessor accessor) : accessor_(std::move(accessor)) {
    }

    T &at(int row, int col) {
        return accessor_(row, col);
    }

private:
    Accessor accessor_;
};

template<typename T = float>
class TiledMatrix {
public:
    TiledMatrix(int seq_len, int q_len, int group_size, int tile_size)
        : seq_len_(seq_len), total_cols_(q_len * group_size), tile_size_(tile_size) {
        num_tiles_ = (seq_len_ + tile_size_ - 1) / tile_size_;
        int padded_total_rows_ = num_tiles_ * tile_size_;
        data_.resize(padded_total_rows_ * total_cols_);
    };

    TileView<T> get_tile(int tile_id) {
        if (tile_id >= num_tiles_ || tile_id < 0) {
            throw std::out_of_range("Tile index out of bounds");
        }
        auto accessor = [this, tile_id](int row, int col) -> T &{
            return this->at(tile_id, row, col);
        };
        return TileView<T>(accessor);
    }

    T &at(int row, int col) {
        if (row >= seq_len_ || row < 0 || col >= total_cols_ || col < 0) {
            throw std::out_of_range("Global access out of bounds.");
        }
        return data_[row * total_cols_ + col];
    }

    [[nodiscard]] int get_total_cols() const {
        return total_cols_;
    }

private:
    T &at(int tile_id, int local_row, int local_col) {
        const int global_row = tile_id * tile_size_ + local_row;
        return data_[global_row * total_cols_ + local_col];
    }

    const int seq_len_;
    const int total_cols_;
    const int tile_size_;
    int num_tiles_;
    std::vector<T> data_;
};
