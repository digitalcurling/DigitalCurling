# 対戦サーバー

思考エンジンどうしを戦わせるための対戦サーバーについて説明します．

## 対戦サーバー実行の流れ

1. [コマンドライン引数](#command-line)を読み込む
2. [コンフィグファイル](#config)を読み込む
3. コンフィグをもとにTCP/IPサーバーを構築
4. 対戦 (通信の流れは[通信プロトコル](protocol.md)参照)

## コマンドライン引数 {#command-line}

Option | Description
-------|-------------
`-h` [`--help`] | ヘルプ
`-C` [`--config`] `arg` | コンフィグファイルのパスを指定します．デフォルトは `./config.json` です．
`--version` | バージョン情報
`-v` [`--verbose`] | 開発者向け．詳細なログを表示します．
`--debug` | 開発者向け．デバッグ用情報を表示します．

### 例

↓実行 (コンフィグファイルとして `./config.json` が読み込まれます)

```
$ digital_curling__server
```

↓コンフィグファイルを指定して実行

```
$ digital_curling__server --config path-to-config-file.json
```

## コンフィグファイル {#config}

サーバーが起動時に読み込む設定ファイルです．
コンフィグファイルのフォーマットは **JSON with Comments (コメント付きJSON)** です．

Key | Type | Description
----|------|------------
`server` | object | サーバーに関する設定を格納します．
&nbsp;&nbsp;&nbsp;&nbsp;`port` | object | 開くポート番号
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team0`<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`team1` | int | 各チームが接続するポート．
&nbsp;&nbsp;&nbsp;&nbsp;`timeout_dc_ok` | float | dcからdc_ok返答までの制限時間(秒)
&nbsp;&nbsp;&nbsp;&nbsp;`send_trajectory` | bool | updateメッセージに軌跡データ(`last_move/trajectory`)を含める場合`true`．
&nbsp;&nbsp;&nbsp;&nbsp;`steps_per_trajectory_frame` | int | updateメッセージ内の軌跡のサンプル間隔．通信およびログデータ量削減のため，updateメッセージ内の軌跡(ログに残す軌跡)は内部のシミュレーションステップから一定間隔でサンプリングしたものになっています．例えばこの値を100とすると，updateメッセージ内の軌跡は100ステップに1回だけ記録されます(シミュレータのフレームレートが0.001second/frameの場合，updateメッセージ内の軌跡のフレームレートは0.1 second/frameになります)．
`game` | object | 試合設定
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
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`0` , ... , `3` | object | 各プレイヤーの設定．[IPlayerFactory](@ref digital_curling::IPlayerFactory)に対応します．詳細は[プレイヤー設定](players.md)を参照してください．
`game_is_ready` | object | 通信プロトコルのis_readyメッセージ内の`game`フィールドは基本的にこのコンフィグ内の`game`と同じものになります．したがって，このコンフィグ内の`game`にシミュレータのシード値といったクライアントに伏せておきたいデータを指定すると，そのようなデータもis_readyメッセージでクライアントに送信されてしまいます．この設定項目はそれを防ぐために存在します．この項目を記述した場合，is_readyメッセージ内の`game`フィールドはこの項目の値で置き換わります(試合自体はこのコンフィグの`game`フィールドに則って進行します)．これによって，クライアントに伏せておきたいデータを守ることができます．`game_is_ready`と`game_is_ready_patch`は省略可能ですが，記述する場合はどちらか一方しか指定できません．
`game_is_ready_patch` | object | この項目のフォーマットは[JSON Patch](http://jsonpatch.com/)に従います．この設定項目が存在する理由は`game_is_ready`と同じです．`game_is_ready`では試合設定全体を書く必要がありますが，こちらはJSON Patchを使用することで，部分的に書き換えることができます．具体的には，is_readyメッセージ内の`game`フィールドはこのコンフィグ内の`game`にこの設定項目のJSON Patchが適用されたものになります(試合自体はこのコンフィグの`game`フィールドに則って進行します)．`game_is_ready`と`game_is_ready_patch`は省略可能ですが，記述する場合はどちらか一方しか指定できません．

### 例

```json
// コンフィグファイルにはコメント(// または /* */ 形式)を含めることができます．
{
    "server": {
        "port": { "team0": 10000, "team1": 10001 },
        "timeout_dc_ok": 10.0,
        "send_trajectory": true,
        "steps_per_trajectory_frame": 100
    },
    "game": {
        "rule": "normal",
        "setting": {
            "max_end": 10,
            "sheet_width": 4.75,
            "five_rock_rule": true,
            "thinking_time": {
                "team0": 219,
                "team1": 219
            },
            "extra_end_thinking_time": {
                "team0": 21.9,
                "team1": 21.9
            }
        },
        "simulator": {
            "type": "fcv1",
            "seconds_per_frame": 0.001
        },
        "players": {
            "team0": [
                { // player 0
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                },
                { // player 1
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                },
                { // player 2
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                },
                { // player 3
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                }
            ],
            "team1": [
                { // player 0
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                },
                { // player 1
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                },
                { // player 2
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                },
                { // player 3
                    "type": "normal_dist",
                    "max_speed": 4.0,
                    "stddev_speed": 0.0076,
                    "stddev_angle": 0.0018,
                    "seed": null // null の場合シード値はランダムに自動設定されます
                }
            ]
        }
    }
    // "game_is_ready_patch": [
    //     { "op": "replace", "path": "/players/team0/0/seed", "value": null },
    //     { "op": "replace", "path": "/players/team0/1/seed", "value": null },
    //     { "op": "replace", "path": "/players/team0/2/seed", "value": null },
    //     { "op": "replace", "path": "/players/team0/3/seed", "value": null },
    //     { "op": "replace", "path": "/players/team1/0/seed", "value": null },
    //     { "op": "replace", "path": "/players/team1/1/seed", "value": null },
    //     { "op": "replace", "path": "/players/team1/2/seed", "value": null },
    //     { "op": "replace", "path": "/players/team1/3/seed", "value": null }
    // ],
    // "game_is_ready": {}
}
```


## ログ

ログは数種類に分けて保存されます．

種類 | ファイル名 | 説明
-----|-----------|-----------
ゲームログ | `(時刻)_(ゲームID)/game.dcl2` | 対戦結果の再生用のログ．
ショットログ | `(時刻)_(ゲームID)/shot_eEEEsSS.json` | 各ショットの軌跡データ．ファイル名の`EEE`はエンド番号(`000`, `001`, ...)，`SS`はショット番号(`00`, `01`, ..., `15`)
サーバーログ | `server.log` | デバッグ用の詳細ログ．対戦ログ，軌跡ログのほかに行われた通信のログ，デバッグ用のログを含む．サーバー起動時に毎回リセットされる．



### ゲームログ

各行がログの各エントリで，改行を含まないJSONで表されます．
各行のJSONのフォーマットを次に示します．

Key | Type | Description
----|------|-----------------
`tag` | string | ログ識別用のタグ．`"gam"`あるいは`"err"`(エラー時)．
`id` | int | ログID
`date_time` | string | 時刻
`thread` | int | スレッドID
`log` | object | ログ．(`tag`が`"gam"`の場合)内容は[通信プロトコル](protocol.md)の各メッセージに準拠したもの加え，通信には用いていないログ専用のメッセージを含む．(`tag`が`"err"`の場合)エラーメッセージ

#### ログの出力順

1. dc
2. meta spec
3. meta config
4. is_ready
5. ready_ok(2チーム分)
6. new_game
7. update
8. move
9. updateとmoveの繰り返し
10. (ゲーム終了後) game_over

#### 通信用メッセージ

`cmd`が`"meta"`ではないメッセージ(通信に用いられたメッセージ)は次のように出力されます．

- dc，new_game，update，game_overは両方のクライアントに同じメッセージが送信されるが，この場合は1度のみ出力する．
- is_readyは`team`を`null`としたものを1回だけ出力する．
- dc_ok，ready_ok，moveはどちらのチームから送られたか識別するためにフィールド`team`が追加される(値は`"team0"`，`"team1"`のいずれか)．

#### ログ用メッセージの詳細

- `cmd`は`"meta"`である．
- 識別用のフィールド`meta`を必ず含む．


##### spec

Key | Type | Description
----|------|-----------------
`cmd` | string | `"meta"`
`meta` | string | `"spec"`
`host_name` | string | コンピュータのホスト名．


##### config

Key | Type | Description
----|------|-----------------
`cmd` | string | `"meta"`
`meta` | string | `"config"`
`config` | object | 入力された試合設定．
`config_all` | object | 実際に使用された試合設定(シード値等が補完された詳細なデータ)．



### ショットログ

Key | Type | Description
----|------|-----------------
`tag` | string | ログ識別用のタグ．ショットログの場合 `"sht"` のみ．
`id` | int | ログID
`date_time` | string | ログ出力時刻
`thread` | int | スレッドID
`log` | object | ログ本体
&nbsp;&nbsp;&nbsp;&nbsp;`game_id` | string | ゲームID
&nbsp;&nbsp;&nbsp;&nbsp;`game_date_time` | string | ゲーム開始日時
&nbsp;&nbsp;&nbsp;&nbsp;`end` | int | エンド番号
&nbsp;&nbsp;&nbsp;&nbsp;`shot` | int | ショット番号
&nbsp;&nbsp;&nbsp;&nbsp;`selected_move` | object | 思考エンジンが選択した行動．
&nbsp;&nbsp;&nbsp;&nbsp;`actual_move` | object | 実際に行われた行動．[Move](@ref digital_curling::Move)に対応する．
&nbsp;&nbsp;&nbsp;&nbsp;`trajectory` | object | 軌跡．[通信プロトコル](protocol.md)のupdateメッセージの`last_move/trajectory`と同じ．



### サーバーログ

各行がログの各エントリで，改行を含まないJSONで表される．
各行のJSONのフォーマットを次に示す．

Key | Type | Description
----|------|-----------------
`tag` | string | ログ識別用のタグ．`"trc"`(通信内容のトレース)，`"dbg"`(デバッグ用)，`"inf"`(標準出力されるインフォ)，`"gam"`(ゲームログ)，`"sht"`(ショットログ)，`"wrn"`(警告)，`"err"`(エラー)のいずれか．
`id` | int | ログID
`date_time` | string | 時刻
`thread` | int | スレッドID
`log` | any | ログ．内容は`tag`による．



