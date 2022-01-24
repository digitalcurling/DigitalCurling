/// \file
/// \brief TimeLimit を定義します

#ifndef DIGITAL_CURLING_GAME_MOVES_TIME_LIMIT_HPP
#define DIGITAL_CURLING_GAME_MOVES_TIME_LIMIT_HPP

namespace digital_curling::game::moves {



/// \brief 行動：制限時間超過
///
/// \warning このコマンドはサーバーが発行するコマンドです．
///          ユーザーは使用しないで下さい．
/// \sa Move
struct TimeLimit {};



} // namespace digital_curling::game::moves

#endif // DIGITAL_CURLING_GAME_MOVES_TIME_LIMIT_HPP
