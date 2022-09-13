#ifndef DIGITAL_CURLING_SERVER_TRAJECTORY_COMPRESSOR_HPP
#define DIGITAL_CURLING_SERVER_TRAJECTORY_COMPRESSOR_HPP

#include <list>
#include "digital_curling/digital_curling.hpp"

namespace digital_curling_server {

class TrajectoryCompressor {
public:
    struct Difference {
        digital_curling::Team team = digital_curling::Team::k0;
        size_t index = 0;
        std::optional<digital_curling::Transform> value;
        Difference() = default;
        Difference(digital_curling::Team team, size_t index, std::optional<digital_curling::Transform> const& value)
            : team(team), index(index), value(value) {}
    };

    struct Result {
        float seconds_per_frame;
        digital_curling::GameState::Stones start;
        digital_curling::GameState::Stones finish;
        std::list<std::vector<Difference>> frames;
        Result();
        void Reset();
    };

    TrajectoryCompressor();

    /// \brief OnStep() を呼び出す前に呼び出す
    ///
    /// \param steps_per_frame 1フレームのステップ数(>= 1)
    /// \param end 試合の現在のエンド
    void Begin(size_t steps_per_frame, std::uint8_t end);

    /// \brief ステップを記録する
    ///
    /// \note 必ず Begin() と End() の間で呼び出す
    void OnStep(digital_curling::ISimulator const& simulator);

    /// \brief OnStep() の呼び出しが終わった後に呼び出す
    ///
    /// 呼出し以降， GetResult() が呼び出せるようになる．
    ///
    /// \param simulator ApplyMove() 終了直後のシミュレータ
    void End(digital_curling::ISimulator const& simulator);

    /// \brief 結果を取得する
    ///
    /// \return 圧縮の結果．
    Result const& GetResult() const;

private:
    bool active_;
    size_t frame_count_;
    size_t steps_per_frame_;
    std::uint8_t end_;
    digital_curling::GameState::Stones prev_stones_;
    Result result_;

    void SetFirstFrame(digital_curling::ISimulator const& simulator);
    void AddFrameDiff(digital_curling::ISimulator const& simulator);
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    TrajectoryCompressor::Difference,
    team,
    index,
    value 
)

void to_json(nlohmann::json &, TrajectoryCompressor::Result const&);
void from_json(nlohmann::json const&, TrajectoryCompressor::Result &);

} // namespace digital_curling_server

#endif // DIGITAL_CURLING_SERVER_TRAJECTORY_COMPRESSOR_HPP
