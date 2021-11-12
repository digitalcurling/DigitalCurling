#ifndef DIGITAL_CURLING_GAME_MOVES_TIME_LIMIT_HPP
#define DIGITAL_CURLING_GAME_MOVES_TIME_LIMIT_HPP

namespace digital_curling::game::moves {



/// <summary>
/// 行動：制限時間超過
/// </summary>
struct TimeLimit {
    static constexpr char kType[] = "time_limit";
};



} // namespace digital_curling::game::moves

#endif // DIGITAL_CURLING_GAME_MOVES_TIME_LIMIT_HPP
