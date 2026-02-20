# 物理シミュレータ設定

# fcv1

シミュレータ Friction-CurlVelocity v1

[SimulatorFCV1Factory](@ref digitalcurling::simulators::SimulatorFCV1Factory) に対応します。

参考: 上原 嘉織，伊藤 毅志，実環境データに近づけるデジタルカーリングのシミュレータの改良，情報処理学会ゲーム情報学研究会，GI-46(18)，pp.1-8 (2021).

Key | Type | Description
----|------|-------------------
`type` | string | シミュレータID (`"fcv1"`)
`seconds_per_frame` | float | フレームレート(フレーム毎秒)

```json
{
    "type": "fcv1",
    "seconds_per_frame": 0.001
}
```

@note
`seconds_per_frame` は 0.001 に設定してください。他の値での動作は保証しません。
