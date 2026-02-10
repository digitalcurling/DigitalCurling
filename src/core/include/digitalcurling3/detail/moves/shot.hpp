// MIT License
// 
// Copyright (c) 2022 UEC Takeshi Ito Laboratory
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file
/// \brief Shot を定義します

#ifndef DIGITALCURLING3_MOVES_SHOT_HPP
#define DIGITALCURLING3_MOVES_SHOT_HPP

#include <cstdint>
#include "../json/common.hpp"
#include "../vector2.hpp"

namespace digitalcurling3::moves {



/// \brief 行動：ショット
///
/// \sa Move
struct Shot {
    /// \brief ショットの回転方向
    enum class Rotation : std::uint8_t {
        kCCW, ///< 反時計回り
        kCW   ///< 時計回り
    };

    Vector2 velocity; ///< ショットの初速度

    Rotation rotation = Rotation::kCCW; ///< ショットの初期回転方向
};


// json
NLOHMANN_JSON_SERIALIZE_ENUM(Shot::Rotation, {
    {Shot::Rotation::kCCW, "ccw"},
    {Shot::Rotation::kCW, "cw"},
})

} // namespace digitalcurling3::moves

#endif // DIGITALCURLING3_MOVES_SHOT_HPP
