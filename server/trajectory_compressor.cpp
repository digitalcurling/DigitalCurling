#include "trajectory_compressor.hpp"
#include "util.hpp"

namespace dc = digital_curling;

namespace digital_curling_server {

TrajectoryCompressor::Result::Result()
{
    Reset();
}

void TrajectoryCompressor::Result::Reset()
{
    seconds_per_frame = 0.f;
    for (auto & team_stones : start) {
        for (auto & stone : team_stones) {
            stone = std::nullopt;
        }
    }
    for (auto & team_stones : finish) {
        for (auto & stone : team_stones) {
            stone = std::nullopt;
        }
    }
    frames.clear();
}

TrajectoryCompressor::TrajectoryCompressor()
    : active_(false)
    , frame_count_(0)
    , steps_per_frame_(0)
    , end_(0)
    , result_()
{}

void TrajectoryCompressor::Begin(size_t steps_per_frame, std::uint8_t end)
{
    assert(!active_);

    active_ = true;
    frame_count_ = 0;
    steps_per_frame_ = steps_per_frame;
    end_ = end;

    result_.Reset();
}

void TrajectoryCompressor::OnStep(digital_curling::ISimulator const& simulator)
{
    assert(active_);


    if (frame_count_ == 0) {
        SetFirstFrame(simulator);
    } else if (frame_count_ % steps_per_frame_ == 0 || simulator.AreAllStonesStopped()) {
        // 設定したステップ数ごと(0フレーム目は除く)にフレーム差分を追加する．
        // また，すべてのストーンが停止している場合にもフレームを追加する．
        // 後者の判定では，すべてのストーンが停止した後かつプレイエリア外のストーンの除外処理が入る前の状態になっている(ApplyMoveの実装を参照ください)
        AddFrameDiff(simulator);
    }

    ++frame_count_;
}

void TrajectoryCompressor::End(digital_curling::ISimulator const& simulator)
{
    assert(active_);

    if (frame_count_ == 0) { // OnStepが呼び出されていない
        SetFirstFrame(simulator);
    }

    auto const stones = dc::GameState::StonesFromAllStones(simulator.GetStones(), end_);
    result_.finish = stones;

    active_ = false;
}

TrajectoryCompressor::Result const& TrajectoryCompressor::GetResult() const
{
    assert(!active_);
    return result_;
}

void TrajectoryCompressor::SetFirstFrame(digital_curling::ISimulator const& simulator)
{
    auto const current_stones = dc::GameState::StonesFromAllStones(simulator.GetStones(), end_);
    prev_stones_ = current_stones;
    result_.start = current_stones;
    result_.seconds_per_frame = simulator.GetSecondsPerFrame() * static_cast<float>(steps_per_frame_);
}

void TrajectoryCompressor::AddFrameDiff(digital_curling::ISimulator const& simulator)
{
    auto const current_stones = dc::GameState::StonesFromAllStones(simulator.GetStones(), end_);
    // 差分の構築
    std::vector<Difference> diffs;
    for (size_t i_team = 0; i_team < current_stones.size(); ++i_team) {
        auto const& prev_team_stones = prev_stones_[i_team];
        auto const& current_team_stones = current_stones[i_team];

        assert(prev_team_stones.size() == current_team_stones.size());

        for (size_t i_stone = 0; i_stone < current_team_stones.size(); ++i_stone) {
            auto const& prev_stone = prev_team_stones[i_stone];
            auto const& current_stone = current_team_stones[i_stone];

            if (prev_stone.has_value() != current_stone.has_value() ||
                (prev_stone.has_value() && // current_stone.has_value()はtrueになるので判定しない
                    (prev_stone->position.x != current_stone->position.x ||
                        prev_stone->position.y != current_stone->position.y ||
                        prev_stone->angle != current_stone->angle))) {
                diffs.emplace_back(static_cast<dc::Team>(i_team), i_stone, current_stone);
            }
        }
    }

    result_.frames.emplace_back(std::move(diffs));

    prev_stones_ = current_stones;
}

void to_json(nlohmann::json & j, TrajectoryCompressor::Result const& v)
{
    j["seconds_per_frame"] = v.seconds_per_frame;
    auto & j_start = j["start"];
    for (size_t i = 0; i < v.start.size(); ++i) {
        j_start[dc::ToString(static_cast<dc::Team>(i))] = v.start[i];
    }
    auto & j_finish = j["finish"];
    for (size_t i = 0; i < v.finish.size(); ++i) {
        j_finish[dc::ToString(static_cast<dc::Team>(i))] = v.finish[i];
    }
    j["frames"] = v.frames;
}

void from_json(nlohmann::json const& j, TrajectoryCompressor::Result & v)
{
    j.at("seconds_per_frame").get_to(v.seconds_per_frame);
    auto const & j_start = j.at("start");
    for (size_t i = 0; i < v.start.size(); ++i) {
        j_start.at(dc::ToString(static_cast<dc::Team>(i))).get_to(v.start[i]);
    }
    auto const & j_finish = j.at("finish");
    for (size_t i = 0; i < v.finish.size(); ++i) {
        j_finish.at(dc::ToString(static_cast<dc::Team>(i))).get_to(v.finish[i]);
    }
    j.at("frames").get_to(v.frames);
}


} // namespace digital_curling_server
