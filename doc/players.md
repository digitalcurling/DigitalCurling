# プレイヤー設定

## identical

乱数を加えたり，速度上限を適用したりせず，入力そのままのショットを行うプレイヤーです．

[IdenticalPlayerFactory](@ref digital_curling::players::IdenticalPlayerFactory)に対応します．

Key | Type | Description
----|------|-------------------
`type` | string | プレイヤーID．`"identical"`

```json
{
    "type": "identical"
}
```

## normal_dist

速度上限を適用した後に，
初速度の大きさと初期角度にそれぞれ正規分布の乱数を加えるプレイヤーです．

[NormalDistPlayerFactory](@ref digital_curling::players::NormalDistPlayerFactory)に対応します．

Key | Type | Description
----|------|-------------------
`type` | string | プレイヤーID．`"normal_dist"`
`max_speed` | float | ショットの最大速度
`stddev_speed` | float | ショットの初速に加わる正規分布乱数の標準偏差．
`stddev_angle` | float | ショットの初期角度に加わる正規分布乱数の標準偏差．
`seed` | int or `null` | 乱数のシード値．`null`を指定するとランダムに選ばれる．

```json
{
    "type": "normal_dist",
    "max_speed": 4.0,
    "stddev_speed": 0.0076,
    "stddev_angle": 0.0018,
    "seed": null
},
```
