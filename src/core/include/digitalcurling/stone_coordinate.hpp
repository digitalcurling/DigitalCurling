// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief StoneCoordinate を定義

#pragma once

#include <array>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>
#include "digitalcurling/common.hpp"
#include "digitalcurling/coordinate.hpp"
#include "digitalcurling/stone.hpp"
#include "digitalcurling/stone_index.hpp"

namespace digitalcurling {


/// @brief シート上に存在するストーンの座標
class StoneCoordinate : public TeamValue<std::array<std::optional<Stone>, 8>> {
public:
    /// @brief ストーンの最大数
    static constexpr int kStoneMax = 16;

    /// @brief コンストラクタ
    StoneCoordinate() : TeamValue<std::array<std::optional<Stone>, 8>>() {
        for (auto& stone : team0) stone = std::nullopt;
        for (auto& stone : team1) stone = std::nullopt;
    }

    /// @brief コンストラクタ
    /// @param[in] stones ストーンの情報
    StoneCoordinate(std::array<std::array<std::optional<Stone>, 8>, 2> stones)
        : TeamValue<std::array<std::optional<Stone>, 8>>(std::move(stones[0]), std::move(stones[1])) {}

    /// @brief コンストラクタ
    /// @param[in] stones ストーンの情報
    StoneCoordinate(std::array<std::optional<Stone>, kStoneMax> stones)
        : TeamValue<std::array<std::optional<Stone>, 8>>()
    {
        std::move(stones.begin(), stones.begin() + 8, this->team0.begin());
        std::move(stones.begin() + 8, stones.end(), this->team1.begin());
    }

    /// @brief ストーンの情報を取得する
    /// @param[in] index ストーンのインデックス
    /// @returns ストーンの情報
    std::optional<Stone> const& operator[](StoneIndex index) const {
        return GetStone(index);
    }
    using TeamValue<std::array<std::optional<Stone>, 8>>::operator[];

    /// @brief ストーンの情報を取得する
    /// @param[in] index ストーンのインデックス
    /// @returns ストーンの情報
    std::optional<Stone> const& GetStone(StoneIndex index) const {
        return GetTeamStones(index.team)[index.stone];
    }

    /// @brief チームの全ストーンの情報を取得する
    /// @param[in] team チーム
    /// @returns チームの全ストーンの情報
    std::array<std::optional<Stone>, 8> const& GetTeamStones(Team team) const {
        return this->operator[](team);
    }

    /// @brief 全ストーンの情報を取得する
    /// @returns 全ストーンの情報
    std::array<std::optional<Stone>, kStoneMax> GetAllStones() const {
        std::array<std::optional<Stone>, kStoneMax> result;
        auto dest = std::copy(team0.begin(), team0.end(), result.begin());
        std::copy(team1.begin(), team1.end(), dest);
        return result;
    }

    /// @brief 盤面上にある全ての有効なストーンのインデックスを取得する
    /// @returns 有効なストーンのインデックス
    std::vector<StoneIndex> GetActiveStones() const {
        std::vector<StoneIndex> active_stones;
        for (Team team : { Team::k0, Team::k1 }) {
            for (uint8_t i = 0; i < 8; i++) {
                auto idx = StoneIndex(team, i);
                if (GetStone(idx).has_value()) active_stones.push_back(idx);
            }
        }
        return active_stones;
    }

    /// @brief 盤面上にある全てのストーンを、ハウスの中心からの距離でソートして取得する
    /// @returns 有効なストーンのインデックスと距離のペア (ハウスの中心からの距離順)
    std::vector<StoneIndex> GetSortedIndex() const {
        auto stones = GetActiveStones();
        std::sort(stones.begin(), stones.end(), [this](StoneIndex const& a, StoneIndex const& b) {
            return GetStone(a).value().GetDistanceFromTee() < GetStone(b).value().GetDistanceFromTee();
        });
        return stones;
    }

    /// @brief 盤面上にある全てのストーンを、ハウスの中心からの距離でソートして取得する
    /// @returns 有効なストーンの情報と距離のペア (ハウスの中心からの距離順)
    std::vector<Stone> GetSortedStones() const {
        auto const idxs = GetSortedIndex();
        std::vector<Stone> sorted;
        std::transform(idxs.begin(), idxs.end(), std::back_inserter(sorted), [this](StoneIndex const& i) {
            return GetStone(i).value();
        });
        return sorted;
    }
};

} // namespace digitalcurling
