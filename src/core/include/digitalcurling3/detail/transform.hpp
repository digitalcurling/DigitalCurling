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
/// \brief Transform を定義します

#ifndef DIGITALCURLING3_TRANSFORM_HPP
#define DIGITALCURLING3_TRANSFORM_HPP

#include "vector2.hpp"
#include "nlohmann/json.hpp"

namespace digitalcurling3 {



/// \brief 位置，角度を格納します．
struct Transform {
    Vector2 position;  ///< 位置
    float angle;       ///< 角度

    /// \brief 位置(0, 0)，角度0で初期化します
    constexpr Transform() : position(), angle(0.f) {}

    /// \brief 指定された値で初期化します
    ///
    /// \param[in] position 位置
    /// \param[in] angle 角度
    constexpr Transform(Vector2 position, float angle) : position(position), angle(angle) {}
};


// json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    Transform,
    position,
    angle
)

} // namespace digitalcurling3

#endif // DIGITALCURLING3_TRANSFORM_HPP
