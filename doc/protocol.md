# 通信プロトコル(v2.0)

このページでは，対戦サーバーと思考エンジン(クライアント)間のTCP/IP通信に使用するプロトコルについて解説します．

## プロトコル仕様

- 通常ルール(ミックスダブルスでないルール)のみに対応しています
- 延長戦あり
  - ただし，(最初のエンドを0として)255エンドに到達した場合，強制的に引き分けになります
- コンシードあり
- 時間制限負けあり
- ショットの初速(速さ，角度)がランダムにブレます．また，ショットの初速に上限があります．
  - ショットを行うプレイヤーごとにショットの精度(乱数の分散)，強さ(初速上限)が異なります

## 通信の流れ (クライアント側からの視点)

- `>>` はサーバーからクライアントへのメッセージ．
- `<<` はクライアントからサーバーへのメッセージ．

```
通信開始

>> dc
<< dc_ok  # クライアントはdc_okを速やかに返す必要があります

>> is_ready
# クライアントはこの間に試合前の時間がかかる準備を行います
<< ready_ok

>> new_game  # ゲームを開始します

while true:  # ターン毎にループします(相手のターンも含みます)
    >> update

    if ゲーム終了:
       break

    if 自分のターン:
        << move

>> game_over  # ゲームを終了します

通信終了(クライアント側から接続を解除する)
```

## メッセージのフォーマット

- すべてのメッセージは[JSON](https://www.json.org/json-ja.html)形式をとります．
- 文字コードはUTF-8です．
- **一つひとつのメッセージは1行で送られ，改行文字で区別されます．**
  改行文字が無いとメッセージの送受信の途中と判断されるため， **各メッセージの末尾には必ず改行文字を付与してください．**
  なお，下記の例では改行して見やすくしていますが，実際のプロトコルでは改行は行わないでください．
- **メッセージ内のJSONにはコメントを付与しないでください．**
  下記のメッセージ例にはコメントが含まれていますが，見やすさのためです．

実際のメッセージは下記のようになります(下記はメッセージ`dc`の例)．メッセージの最後には改行文字が付きます．

```
{"cmd":"dc","version":{"major":2,"minor":0},"game_id":"RRRRRRRR-RRRR-4RRR-rRRR-RRRRRRRRRRRR","date_time":"2022-03-11T13:58:57+09:00"}
```

## 各メッセージの詳細

### dc 

Server -> Client．
サーバーの情報を送信します．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"dc"`
`version` | object | プロトコルバージョン．\emoji :warning: Digital Curlingのバージョンとは異なります．
&nbsp;&nbsp;&nbsp;&nbsp;`major` | int | プロトコルバージョン(メジャー)
&nbsp;&nbsp;&nbsp;&nbsp;`minor` | int | プロトコルバージョン(マイナー)
`game_id` | string | ゲームID．UUID v4．
`date_time` | string | ゲーム開始日時．ISO 8601 extended format

```json
{
    "cmd": "dc",
    "version": {
        "major": 2,
        "minor": 0
    },
    "game_id": "RRRRRRRR-RRRR-4RRR-rRRR-RRRRRRRRRRRR",
    "date_time": "2022-03-11T13:58:57+09:00"
}
```

### dc_ok

Client -> Server．
メッセージ dc に対する返答です．
思考エンジンの名前を宣言します．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"dc"`
`name` | string | 思考エンジンの名前

```json
{
    "cmd": "dc_ok",
    "name": "sample ai"
}
```

### is_ready

Server -> Client．
試合の設定を示します．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"is_ready"`
`team` | string | 割り当てられたチームID．`"team0"`，`"team1"`のいずれか．
`game` | object | 試合設定．
&nbsp;&nbsp;&nbsp;&nbsp;`rule` | string | ゲームルール．現在は通常ルールに相当する`"normal"`のみ．
&nbsp;&nbsp;&nbsp;&nbsp;`setting` | object | 試合設定．[GameSetting](@ref digital_curling::GameSetting) に対応します．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`max_end` | int | 試合のエンド数(延長戦を含まない)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`sheet_width` | float | アイスシートの横幅(狭いほうの幅)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`five_rock_rule` | bool | ファイブロックルールを適用するなら`true`
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`thinking_time` | object | 延長戦を含まない思考時間(秒)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | float | 各チームの思考時間(秒)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`extra_end_thinking_time` | object | 延長戦の1エンド毎の思考時間(秒)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | float | 各チームの延長戦の思考時間(秒)
&nbsp;&nbsp;&nbsp;&nbsp;`simulator` | object | 物理シミュレータの設定．[ISimulatorFactory](@ref digital_curling::ISimulatorFactory) に対応します．詳細は[物理シミュレータ設定](simulators.md)を参照してください．
&nbsp;&nbsp;&nbsp;&nbsp;`players` | object | プレイヤーの設定
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | array | 各チームのプレイヤー．各4人．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`0`, ... , `3` | object | 各プレイヤーの設定．[IPlayerFactory](@ref digital_curling::IPlayerFactory)に対応します．詳細は[プレイヤー設定](players.md)を参照してください．

```json
{
    "cmd": "is_ready",
    "team": "team0",
    "game": {
        "rule": "normal",
        "setting": {
            "max_end": 10,
            "sheet_width": 4.75,
            "five_rock_rule": true,
            "thinking_time": {
                "team0": 2280,
                "team1": 2280
            },
            "extra_end_thinking_time": {
                "team0": 270,
                "team1": 270
            }
        },
        "simulator": {
            "type": "fcv1",
            "seconds_per_frame": 0.001
        },
        "players": {
            "team0": [
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null },
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null },
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null },
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null }
            ],
            "team1": [
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null },
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null },
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null },
                { "type": "normal_dist", "stddev_speed": 0.01, "stddev_angle": 0.01, "max_speed": 3.0, "seed": null }
            ]
        }
    }
}
```

### ready_ok

Client -> Server．
is_readyで示された試合設定に対し準備ができたらready_okを送信します．
ready_okではプレイヤーのショット順を示します．

\note
両方の思考エンジンがready_okを送信すると速やかに試合が開始され，その後は残り時間が減っていくため，is_ready受信からready_ok送信の間に時間のかかる準備を行ってください．


Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"ready_ok"`
`player_order` | array | プレイヤーのショット順．
&nbsp;&nbsp;&nbsp;&nbsp;`0` | int | リード(ショット0, 1)を担当するプレイヤー．is_readyメッセージの`game/players/(自チーム)`内のインデックスを指定します．
&nbsp;&nbsp;&nbsp;&nbsp;`1` | int | セカンド(ショット2, 3)を担当するプレイヤー．
&nbsp;&nbsp;&nbsp;&nbsp;`2` | int | サード(ショット4, 5)を担当するプレイヤー．
&nbsp;&nbsp;&nbsp;&nbsp;`3` | int | フォース(ショット6, 7)を担当するプレイヤー．

```json
{
    "cmd": "ready_ok",
    "player_order": [
        0,  // リード
        1,  // セカンド
        3,  // サード
        2   // フォース
    ]
}
```

\note player_orderに同じインデックスを指定するとエラーになります．

### new_game

Server -> Client．
試合開始の合図です．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"new_game"`
`name` | object | チーム名
&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;`team1` | string | 各チームの名前

```json
{
    "cmd": "new_game",
    "name": {
        "team0": "team 0 name",
        "team1": "team 1 name"
    }
}
```

### update

Server -> Client．
試合情報を更新します．
相手のターン，自分のターン関わらず送信されます．自分のターンの際は制限時間内にmoveメッセージを送信する必要があります(制限時間がなくなると時間切れ負けになります)．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"update"`
`next_team` | string or `null` | 次に行動するチーム． `"team0"`，`"team1"`，`null`(試合終了時)のいずれか．
`state` | object | 現在の試合の状態．[GameState](@ref digital_curling::GameState)に対応します．
&nbsp;&nbsp;&nbsp;&nbsp;`end` | int | 現在のエンド番号．0 <= `end` <= 255 (`end` == 255で強制的に引き分け)
&nbsp;&nbsp;&nbsp;&nbsp;`shot` | int | 現在のショット番号．0 <= `shot` <= 15
&nbsp;&nbsp;&nbsp;&nbsp;`hammer` | string or `null` | 現在のエンドのハンマー(後攻)．ゲーム終了後は `null` になります．
&nbsp;&nbsp;&nbsp;&nbsp;`stones` | object | ストーンの配置．座標系については[座標系](coordinate.md)を参照してください．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | array | 各チームのストーン．各8個．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`0`, ... , `7` | object or `null` | 各ストーンの配置．ストーンがシート上に無い(未ショットまたはシート外に出て除外された)場合，`null`．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`position` | object | 位置
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`x` | float | x座標
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`y` | float | y座標
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`angle` | float | 角度
&nbsp;&nbsp;&nbsp;&nbsp;`scores` | object | エクストラエンドを含まない各エンドのスコア
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | array | 各チームのスコア．arrayのサイズは is_ready メッセージの `game/setting/max_end` と等しい．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`0`, ... | int or `null` | 各エンドのスコア．未定の場合`null`．
&nbsp;&nbsp;&nbsp;&nbsp;`extra_end_scores` | object | エクストラエンド(延長戦)のスコア
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | int or `null` | 各チームの延長エンドのスコア．未定の場合`null`．
&nbsp;&nbsp;&nbsp;&nbsp;`thinking_time_remaining` | object | 残り思考時間
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | float | 各チームの残り思考時間(秒)
&nbsp;&nbsp;&nbsp;&nbsp;`game_result` | object or `null` | 試合結果．未定(試合中)の場合は`null`．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`winner` | string or `null` | 勝利チーム．`"team0"`，`"team1"`，`null`(引き分け)のいずれか．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`reason` | string | 勝敗の理由．`"score"`(スコアによる勝敗)，`"concede"`(コンシードによる勝敗)，`"time_limit"`(制限時間切れによる勝敗)，`"draw"`(引き分け)のいずれか．
`last_move` | object or `null` | 前回のショットの結果です．最初のupdateの際は`null`になります．
&nbsp;&nbsp;&nbsp;&nbsp;`actual_move` | object | 実際に行われた行動．ショットの場合，思考エンジンがmoveメッセージで送信した値に乱数や速度上限が適用されたものになります．フォーマットはmoveメッセージの`move`と同じです．
&nbsp;&nbsp;&nbsp;&nbsp;`free_guard_zone_foul` | bool | フリーガードゾーンルール違反によって，ストーンの配置がショット前に戻った場合`true`になります．
&nbsp;&nbsp;&nbsp;&nbsp;`trajectory` | object | ストーンの軌跡．[詳細](#trajectory)

#### trajectory {#trajectory}

Key | Type | Description
----|------|------------------
`seconds_per_frame` | float | 軌跡のフレーム毎の秒数
`start` | object | ショット開始時のストーンの配置．フォーマットは `state/stones` と同様．
`finish` | object | ショット終了時のストーンの配置(全ストーンの停止後，プレイエリア外のストーンが除去された状態．フリーガードゾーンルールが適用された場合は適用済みの状態)．フォーマットは `state/stones` と同様．
`frames` | array | `seconds_per_frame` で示される時間間隔毎のストーンの位置と角度．各フレームは一つ前のフレームに対する差分として表現されます(最初のフレームは`start`です)．したがって，`start`で表される初期状態に対しフレームごとの差分を順々に適用することで，すべてのフレームのストーンの配置を得ることができます．
&nbsp;&nbsp;&nbsp;&nbsp;`0`, ... | array | 各フレーム間の差分．i番目はフレームiとi+1の差分になります(フレーム0は `start` です)．フレーム間の差分は各ストーンについての差分の集合として表現されます．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`0`, ... | object | 各ストーンについての差分．差分はどのストーンに変化があったかの情報(`team`，`index`)と変化後のそのストーンの情報(`value`)で表されます．それぞれのストーンについて，一つ前のフレームと比較して，そのパラメータ(座標，角度，存在の有無)に変化があった場合のみ，差分が発生します．ストーンのパラメータに一切変化が無かった場合，差分は発生しません．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team` | string | パラメータに変化があったストーンを特定するための情報．ストーンを投げたチームを表します．`"team0"`，`"team1"`のいずれか．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`index` | int | パラメータに変化があったストーンを特定するための情報．ストーンのインデックスを表します．0 <= `index` <= 7
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`value` | object or `null` | パラメータ変化後のストーンの情報．ストーンが移動した場合はその位置と角度を格納した object，ストーンが除外された場合は `null` になります．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`position` | object | ストーンの位置．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`x` | float | x座標．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`y` | float | y座標．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`angle` | float | ストーンの角度

\note
`frames` の最終フレームは全ストーンが停止した直後の状態です．
これにプレイエリア外のストーンの除去処理やフリーガードゾーンルールを適用した状態が `finish` です．


試合中(4エンドゲーム)

```json
{
    "cmd": "update",
    "next_team": "team0",
    "state": {
        "end": 0,
        "shot": 2,
        "hammer": "team1",
        "stones": {
            "team0": [
                { "position": { "x": 0.3040076792240143, "y": 38.32698059082031 }, "angle": 38.05548095703125 },
                null,
                null,
                null,
                null,
                null,
                null,
                null
            ],
            "team1": [
                { "position": { "x": -0.03419952467083931, "y": 38.498924255371094 }, "angle": 38.14583969116211 },
                null,
                null,
                null,
                null,
                null,
                null,
                null
            ]
        },
        "scores": {
            "team0": [ null, null, null, null ],
            "team1": [ null, null, null, null ]
        },
        "extra_end_score": {
            "team0": null,
            "team1": null
        },
        "thinking_time_remaining": {
            "team0": 21.897,
            "team1": 21.882
        },
        "game_result": null
    },
    "last_move": {
        "actual_move": {
            "type": "shot",
            "velocity": { "x": 0.12980346381664276, "y": 2.4026544094085693 },
            "rotation": "ccw"
        },
        "free_guard_zone_foul": false,
        "trajectory": {
            "seconds_per_frame": 0.10000000149011612,
            "start": {
                "team0": [
                    { "position": { "x": 0.3040076792240143, "y": 38.32698059082031 }, "angle": 38.05548095703125 },
                    { "position": { "x": 0.0, "y": 0.0 }, "angle": 0.0 },
                    null,
                    null,
                    null,
                    null,
                    null,
                    null
                ],
                "team1": [
                    { "position": { "x": -0.03419952467083931, "y": 38.498924255371094 }, "angle": 38.14583969116211 },
                    null,
                    null,
                    null,
                    null,
                    null,
                    null,
                    null
                ]
            },
            "finish": {
                "team0": [
                    { "position": { "x": 0.3040076792240143, "y": 38.32698059082031 }, "angle": 38.05548095703125 },
                    { "position": { "x": 1.1429188251495361, "y": 36.85197448730469 }, "angle": 37.2751579284668 },
                    null,
                    null,
                    null,
                    null,
                    null,
                    null
                ],
                "team1": [
                    { "position": { "x": -0.03419952467083931, "y": 38.498924255371094 }, "angle": 38.14583969116211 },
                    null,
                    null,
                    null,
                    null,
                    null,
                    null,
                    null
                ]
            },
            "finish_time": 28.15300178527832,
            "frames" : [
                [
                    {
                        "team": "team0",
                        "index": 1,
                        "value": { "position": { "x": 0.020044805482029915, "y": 0.23464012145996094 }, "angle": 0.15702606737613678 }
                    }
                ],
                // ...
            ]
        }
    }
}
```

ゲーム終了時(4エンドゲーム)

```json
{
    "cmd": "update",
    "next_team": null,
    "state": {
        "end": 4,
        "shot": 0,
        "hammer": null,
        "stones": {
            "team0": [
                null,
                null,
                null,
                null,
                null,
                null,
                null,
                null
            ],
            "team1": [
                null,
                null,
                null,
                null,
                null,
                null,
                null,
                null
            ]
        },
        "scores": {
            "team0": [ 0, 2, 1, 0 ],
            "team1": [ 1, 0, 0, 1 ]
        },
        "extra_end_score": {
            "team0": null,
            "team1": null
        },
        "thinking_time_remaining": {
            "team0": 21.347,
            "team1": 21.335
        },
        "game_result": {
            "winner": "team0",
            "reason": "score"
        }
    },
    "last_move": {
        "actual_move": {
            "type": "shot",
            "velocity": { "x": 0.1411837339401245, "y": 2.373765468597412 },
            "rotation": "ccw"
        },
        "free_guard_zone_foul": false,
        "trajectory": {
            // ...
        }
    }
}
```


### move

Client -> Server．
次に行う行動を示します．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"move"`
`move` | object | クライアントが行う行動．[Move](@ref digital_curling::Move)に対応します．
&nbsp;&nbsp;&nbsp;&nbsp;`type` | string | ショットを行う場合`"shot"`，コンシードを行う場合`"concede"`を指定します．
&nbsp;&nbsp;&nbsp;&nbsp;`velocity` | object | (typeが`"shot"`の場合のみ) ショットの初速度．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`x` | float | ショットの初速度(x方向)．
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`y` | float | ショットの初速度(y方向)．
&nbsp;&nbsp;&nbsp;&nbsp;`rotation` | string | (typeが`"shot"`の場合のみ) ショットの回転方向．時計回りの場合`"cw"`，反時計回りの場合`"ccw"`を指定します．

ショットを行う場合

```json
{
    "cmd": "move",
    "move": {
        "type": "shot",
        "velocity": { "x": 0.1, "y": 2.0 },
        "rotation": "ccw"
    }
}
```

コンシードを行う場合

```json
{
    "cmd": "move",
    "move": {
        "type": "concede"
    }
}
```

### game_over

Server -> Client．
試合終了を通告します．

Key | Type | Description
----|------|------------------
`cmd` | string | メッセージID．`"game_over"`

```json
{
    "cmd": "game_over"
}
```

