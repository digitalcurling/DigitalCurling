// Copyright (c) 2022-2026 UEC Takeshi Ito Laboratory
// SPDX-License-Identifier: MIT

/// @file
/// @brief GameScores を定義

#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>
#include "digitalcurling/common.hpp"
#include "digitalcurling/team.hpp"

namespace digitalcurling {


/// @brief 試合のスコア情報
struct GameScores : public TeamValue<std::vector<std::optional<std::uint8_t>>> {
private:
    /// @brief エンド数
    std::uint8_t end;

public:
    /// @brief コンストラクタ
    GameScores() : end(0) { };

    /// @brief コンストラクタ
    /// @param[in] max_end エンド数 (延長戦を含まない)
    GameScores(std::uint8_t max_end) :
        end(max_end),
        TeamValue<std::vector<std::optional<std::uint8_t>>>(std::vector<std::optional<std::uint8_t>>(max_end + 1))
        { }

    /// @brief コンストラクタ
    /// @param[in] scores スコア (team0, team1)
    /// @note スコアのサイズは同じでなければなりません。
    /// @note また、格納されている( `std::nullopt` でない)後攻のデータ数は、先行チームと同じか、1つ少ない必要があります。
    GameScores(std::array<std::vector<std::optional<std::uint8_t>>, 2> scores)
        : TeamValue<std::vector<std::optional<std::uint8_t>>>(std::move(scores[0]), std::move(scores[1]))
    {
        assert(team0.size() == team1.size());
        assert(team0.size() >= 2);

        end = static_cast<std::uint8_t>(team0.size() - 1);
        bool none = false;
        for (int i = 0; i < team0.size(); i++) {
            if (team0[i].has_value()) {
                assert(!none && team1[i].has_value());
            } else {
                if (!none) none = true;
                else assert(!team1[i].has_value());
            }
        }
    }

    /// @brief チームのスコアを得る
    /// @param[in] team チーム
    /// @param[in] end エンド
    /// @return 点数 (未確定の場合は `std::nullopt`)
    /// @exception std::out_of_range `end` が範囲外の場合
    std::optional<std::uint32_t> GetEndScore(Team team, std::uint8_t end) const {
        if (end < 0 || end > this->end) {
            throw std::out_of_range("end is out of range.");
        }
        return (*this)[team][end];
    }

    /// @brief チームのエクストラエンドにおけるスコアを得る
    /// @param[in] team チーム
    /// @return 点数 (未確定の場合は `std::nullopt`)
    std::optional<std::uint32_t> GetExtraEndScore(Team team) const {
        return (*this)[team][this->end];
    }


    /// @brief 合計スコアを得る
    /// @return 合計スコア (team0, team1)
    TeamValue<std::uint32_t> GetTotalScore() const {
        return { GetTotalScore(Team::k0), GetTotalScore(Team::k1) };
    }
    /// @brief チームの合計スコアを得る
    /// @param[in] team チーム
    /// @return 合計スコア
    std::uint32_t GetTotalScore(Team team) const {
        int total = 0;
        for (auto const& score : (*this)[team]) {
            if (!score.has_value()) break;
            total += score.value();
        }
        return total;
    }
};

} // namespace digitalcurling
