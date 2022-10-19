# 物理シミュレータ設定

新規物理シミュレータの追加については[ライブラリの拡張](extend_library.md)を参照ください．

# fcv1

Friction-CurlVelocity シミュレータv1．[FCV1SimulatorFactory](@ref digitalcurling3::simulators::FCV1SimulatorFactory)に対応します．

参考: 上原 嘉織，伊藤 毅志，実環境データに近づけるデジタルカーリングのシミュレータの改良，情報処理学会ゲーム情報学研究会，GI-46(18)，pp.1-8 (2021).

Key | Type | Description
----|------|-------------------
`type` | string | シミュレータID．`"fcv1"`
`seconds_per_frame` | float | フレームレート(フレーム毎秒)．0.001以外の値での動作は保証しません．

```json
{
    "type": "fcv1",
    "seconds_per_frame": 0.001
}
```

