# 通信層の boost::asio 移行と TCP / WebSocket 対応 設計メモ

- 状態: Phase 1〜6 完了（2026-11-06）。8章の未決事項で残っているのは #16（マニュアルの更新）だけ。
- ブランチ: `feature/asio-migration`。develop にマージ済み（PR #155、2026-11-18）。master には次のメジャーリリース（20.0.0）のときに入る。
- 英語版: [2026-10_asio-migration.en.md](2026-10_asio-migration.en.md)。両方を同じ内容に保つ。

## 0. 進捗

| Phase | 内容 | コミット | 状態 |
|---|---|---|---|
| 1 | Connection の導入（asio なし） | d75f6f8 | 完了 |
| 2 | Acceptor の導入 | 110bc2a | 完了 |
| 3 | asio と TCP の導入 | c790844 | 完了 |
| 4 | WebSocket の導入 | 5ae4acc | 完了 |
| 5 | UDP の asio 化 | 572d6b3、修正 8537a9a | 完了 |
| 6 | 待ちのイベント駆動化 | 79cb8f3 | 完了 |

計画の後に行ったこと:

- bf327da: rcssserver と rcssclient が rcss::net をリンクしないようにした。rcssclient も asio の UDP ソケットに移した。
- bad983d: rcssclient の `select()` のループを asio のイベント処理に置き換えた。
- 9996511: `Stadium::init()` から Acceptor を開く処理を `openAcceptors()` などに切り出した。
- eaa4625: メッセージの圧縮 `(compression N)` のサポートをやめた（8章 #9）。
- 3f8041b: TCP のポートに -1（対応する UDP のポートと同じ番号）を指定できるようにした（8章 #14）。
- f1faefe: rcss/net を削除した（8章 #15）。

1章の目的に対する状況:

- rcss::net の置き換え: 完了。rcss/net は削除した。コードは rcssserver-19.0.0 のタグに残っている（8章 #15）。
- TCP と WebSocket での接続: 完了。プレイヤ、コーチ、オンラインコーチ、モニタのすべてが使える。
- 既存の UDP クライアント: 各 Phase の回帰確認で、挙動の差がないことを確かめた（6章）。

## 1. 目的と範囲

**目的**

- 手書きのソケット層（`rcss::net`）を boost::asio に置き換える。
- UDP に加えて、TCP と WebSocket でもクライアント（プレイヤ、コーチ、モニタ）が接続できるようにする。
- 既存の UDP クライアントは無変更のまま動き続けるようにする。

**範囲外**

- プロトコルのメッセージ内容（S式 / JSON）の変更
- シミュレーションの意味の変更、マルチスレッド化
- TLS（wss）。必要ならリバースプロキシで終端する
- `rcssclient`（[src/client.cpp](../../src/client.cpp)）の移行。当初は範囲外としたが、rcss::net を取り除けるようにするため、計画の後に移した（8章 #8）。

## 2. 移行前の構成

移行前（19.0.0）の構成を、経緯として残す。

### 待受と接続確立

- Stadium が UDP の待受ソケットを3つ持つ。プレイヤ用 6000（モニタも同居）、オフラインコーチ用 6001、オンラインコーチ用 6002。
- 未知のアドレスから届いたメッセージは init として解釈される（`parsePlayerInit()` / `parseMonitorInit()` / `parseCoachInit()` / `parseOnlineCoachInit()`）。
- 受理すると、そのクライアントの `RemoteClient` が新しい UDP ソケットを開いて相手のアドレスに connect し、以後はそのソケットから送る。クライアントは返信元のポート（専用ポート）へ送るようになる。
- 拒否時のエラー応答は待受ソケットから送る（`sendToPlayer()` / `sendToCoach()` / `sendToOnlineCoach()`）。
- 受理済みのクライアントが待受ポートへ送ったメッセージは、アドレスの一致で送り主を探して `RemoteClient::undedicatedRecv()` に渡す。専用ポートを強制するクライアント（プレイヤとコーチは v8 以上、モニタは v2 以上と JSON）にはエラーを返す。
- クライアントの同一性はアドレス（`RemoteClient::getDest()`）で判定している。`getDest()` を使っているのは、この振り分けの4箇所だけ。

### 送信

- senders は `RemoteClient::getTransport()`（`std::ostream`）に書き込み、`<< std::ends << std::flush` で1メッセージを確定させる（56箇所）。`Player::send()` なども同じ ostream を通る。
- ostream の下は `SocketStreamBuf`（圧縮時はその上に `gzstreambuf`）で、flush 1回が UDP データグラム1つになる。バッファ（8192 バイト）があふれるとその時点で送信されるため、大きなメッセージは複数のデータグラムに分かれる。

### 受信と処理のタイミング

- `StandardTimer` が recv_step（既定 10ms）ごとに `Stadium::doRecvFromClients()` を呼ぶ。
- 処理は次の順で行う。各グループの中ではクライアントの順番をシャッフルし、1クライアントずつ受信データがなくなるまで処理する（`recv_from_clients()`）。
  1. プレイヤ → モニタ → 6000 番の待受ソケット
  2. オンラインコーチ → 6002 番の待受ソケット
  3. オフラインコーチ → 6001 番の待受ソケット
  4. `removeDisconnectedClients()`
- 受信したメッセージは、その場でパースされて各クライアントの状態に反映される（`RemoteClient::processMsg()` → `parseMsg()`）。

### sync モード

- `SyncTimer` はスリープせずにループする。`Stadium::doSendThink()` で `(think)` を送ったあと、`synch_micro_sleep` ごとに `doRecvFromClients()` を呼んで全員の `(done)` を待つ（上限 1250ms）。
- sync モードの受信は、すべてこの待ちループの中で行われる。

### 既存の問題（移行の中で直した）

- `udp_recv_message()` など3箇所で、ちょうど MaxMesg バイトのデータグラムを受け取ると、`message[len] = '\0'` が配列の1バイト外に書き込む。受理済みクライアントが待受ポートへ送った場合も、`parseMsg()` が同じ配列の1バイト外に書き込む。19.0.0 を ASan 付きでビルドして確認した。→ Phase 2 で解消。
- `RemoteClient::close()` が transport の ostream を delete するが、senders はその参照を持ったままになる。送信エラーでクライアントが閉じられた直後に、同じ処理の中で別の送信（審判メッセージなど）が走ると、解放済みの ostream に書き込んでクラッシュする。19.0.0 で、圧縮を有効にしたプレイヤがいるときにキックオフで再現した。→ Phase 1 で解消（ostream を常駐させ、`close()` は streambuf を外すだけにした）。
- `Addr` は `sockaddr_in` そのもので、IPv4 専用。→ TCP / WS は IPv6 に対応した。UDP は IPv4 のまま（8章 #7）。

## 3. 守るべき互換性（不変条件）

すべての Phase で守った。

- **I1 UDP プロトコル**: 次の点を変えない。
  - ポート番号と専用ポート方式
  - 待受ポートへ送られたメッセージの扱い（v8 未満は処理し、v8 以上にはエラーを返す）
  - エラー応答の送信元
  - `'\0'` 終端のメッセージ形式
- **I2 処理のタイミング**: クライアントからのメッセージは `doRecvFromClients()` の中でだけ処理する。I/O のハンドラの中ではパースしない。
- **I3 処理の順序**: 2章の順序（グループの順番と、グループ内のシャッフル）を保つ。
- **I4 sync モード**: think / done の手順とタイムアウトの扱いを保つ。Phase 6 で、ポーリングをメッセージの到着待ちに置き換えたが、手順とタイムアウトは変えていない。
- **I5 メッセージ境界**: `transport()` の flush 1回を1メッセージとする。senders は変更しない。
- **I6 スレッド**: シミュレーションと I/O はすべて同じスレッドで動かし、ロックは導入しない。

## 4. 設計

### 4.1 抽象の境界

ソケット単位ではなく、次の2つで抽象化した。

- **Connection**: クライアント1つ分のメッセージの通り道。`RemoteClient` が所有する。
- **Acceptor**: サービス（プレイヤ / オフラインコーチ / オンラインコーチ）とトランスポートの組ごとに1つ。まだクライアントに結び付いていないメッセージを Stadium に渡し、受理されたら Connection を作る。Stadium が所有する。

名前は、既存の `rcss::Listener`（音声の受け手、[src/audio.h](../../src/audio.h)）との混同を避けて Acceptor とした。

クライアントの同一性は Connection オブジェクトで表す。

```cpp
// クライアント1つ分のメッセージの通り道（UDP / TCP / WebSocket で実装）
class Connection {
public:
    virtual ~Connection() = default;

    // 送信: ここに書き込み、pubsync()（std::flush）1回で1メッセージ。
    // RemoteClient はこの上に ostream を重ねる。
    virtual std::streambuf & outbuf() = 0;

    // 受信: 受信済みのメッセージを1つ取り出す。なければ false を返す（ブロックしない）。
    // 長さは MaxMesg 以下。
    virtual bool popMessage( std::string & msg ) = 0;

    virtual bool isOpen() const = 0;
    virtual void close() = 0;

    // メッセージがどのバイトも含めるか。TCP は '\0' を区切りに使うので false。
    // false なら、バイナリのモニタ（v1 / v2）は使えない。
    virtual bool isBinarySafe() const = 0;
};

// 待受ポートに届いた、まだクライアントに結び付いていないメッセージ
struct Inbound {
    std::string message;
    std::shared_ptr< Connection > reply;  // 返信用。UDP: 待受ソケットから送る / TCP・WS: その接続自身
    const Connection * known = nullptr;   // UDP のみ: 受理済みクライアントが待受ポートへ送った場合、その Connection
};

class Acceptor {
public:
    virtual ~Acceptor() = default;

    // 未処理の Inbound を1つ取り出す。なければ false を返す（ブロックしない）。
    virtual bool popInbound( Inbound & in ) = 0;

    // in の送り主を受理し、専用の Connection を返す（失敗したら nullptr）。
    // UDP: 新しいソケットを開いて相手に connect する / TCP・WS: その接続をそのまま返す
    virtual std::shared_ptr< Connection > accept( const Inbound & in ) = 0;
};
```

補足:

- 当初の案にあった `Connection::peerName()` と `Acceptor::close()` は、使う場所がなかったので実装していない。
- `isBinarySafe()` は Phase 3 で足した。
- `known` は、UDP の振り分け（アドレスの一致で受理済みクライアントを探し、`undedicatedRecv()` に渡す）をそのまま再現するためのもの。Stadium は `known` の Connection を持つクライアントを探して `undedicatedRecv()` を呼ぶ。TCP / WS では常に `nullptr`。
- TCP / WS では、未受理の接続から届いたメッセージを1つずつ Inbound として渡す。
  - 受理されたら、それ以降のメッセージはその Connection の受信キューに入る。init の直後に送られたコマンドを、init として解釈しないためである。
  - 受理されなければ、次のメッセージを次の Inbound として渡す。UDP で未知のアドレスから届いた場合と同じ扱いになる。
- Connection は `std::shared_ptr` で共有する。asio の非同期処理のハンドラが Connection を保持する必要があるため（`shared_from_this()`）。
- `RemoteClient` は `open()` / `connect()` / `getDest()` をやめて、`attach( std::shared_ptr< Connection > )` で Connection を受け取る。reconnect も同じ経路で処理する。
- `parse*Init()` / `init*()` の引数は `const Addr &` から `const Inbound &` に変わった。`sendToPlayer( msg, addr )` などは `in.reply` への送信に置き換わった。

### 4.2 スレッドとイベントループ

- `io_context` は1つだけ用意し、メインスレッドで回す（I6）。Stadium が所有する。
- **Phase 3〜5**: 既存のタイマーはそのまま使い、`doRecvFromClients()` の先頭で `io_context.poll()` を呼んだ。
  - 待ち中の非同期処理がないと、`poll()` は io_context を停止状態にする。そこで、停止していれば `restart()` してから `poll()` する（ワークガードは使わない）。
- **Phase 6**: タイマーのループはそのまま残し、待ち（`sleep_for` と `synch_micro_sleep` のポーリング）を、io_context を回しながら待つ処理に置き換えた（4.6）。待っている間も I/O のハンドラが動く。
- どの段階でも、I/O のハンドラは受信データをキューに積むだけにする（I2）。

### 4.3 受信処理の流れ

```
Stadium::doRecvFromClients()
  ├─ 遅延効果（doLongKick）
  ├─ io_context.poll()
  ├─ recvFromPlayerPort()
  │    ├─ プレイヤ → モニタ（それぞれシャッフル）: RemoteClient::recv() → Connection::popMessage()
  │    └─ プレイヤ用 Acceptor 群（UDP → TCP → WS /player）: popInbound()
  │         ├─ known あり → そのクライアントの undedicatedRecv()
  │         └─ known なし → parseMonitorInit() / parsePlayerInit() → accept() するか、in.reply にエラーを返す
  ├─ recvFromMonitorPort(): モニタ専用の Acceptor（WS /monitor）
  ├─ recvFromOnlineCoachPort(): オンラインコーチ（シャッフル）→ オンラインコーチ用 Acceptor 群
  ├─ recvFromOfflineCoachPort(): オフラインコーチ（シャッフル）→ オフラインコーチ用 Acceptor 群
  └─ removeDisconnectedClients()
```

各サービスの Acceptor は UDP、TCP、WS の順に並べ（`Stadium::openAcceptors()`）、この順に処理する。

### 4.4 送信とバッファ管理

- **UDP**: 同期の送信（`send` / `send_to`）にした。
  - asio の `async_send_to` は使わない。完了までバッファを保持し続ける必要があるうえ、UDP では利点がないため。
  - `SocketStreamBuf` の代わりに `DatagramBuf` を作った（Phase 5）。8192 バイトのバッファがあふれた時点で送る挙動も同じにしてある（I1）。
  - 送信バッファが詰まっているとき（EWOULDBLOCK）は、`poll()` で送れるようになるまで待つ。rcss::net は送れるまで再試行していた（ビジーループ）。Phase 5 ではこれを `basic_socket::wait()` に置き換えたが、asio の `wait()` は非ブロッキングのソケットでは待たずに失敗するため、クライアントを切断していた（8537a9a で修正）。
  - 送信に失敗したら、ストリームをエラー状態にし、errno に理由を入れる（`RemoteClient::send()` が ECONNREFUSED を見分けるため）。失敗したデータはバッファに残る。これも rcss::net と同じ。
- **TCP / WS**: Connection ごとに送信キューを持ち、実行中の非同期書き込みは常に1つだけにする。
  - TCP では、非同期書き込みが進行中でなければ、まずその場で非ブロッキング送信を試み、送り切れなかった分だけキューに入れる。
  - WS（Beast）は同期の非ブロッキング送信ができないため、常に非同期書き込みにする。Phase 5 までは同じ tick の2通目以降が次の `poll()` まで（最大 recv_step）遅れたが、Phase 6 で待ちの間もハンドラが動くようになり、遅れはなくなった。
  - Nagle アルゴリズムによる遅延を避けるため、`TCP_NODELAY` を設定する（TCP / WS とも）。設定しないと、sync モードで1サイクルに約 40ms かかる（WS で実測）。
  - キューにはメッセージごとのコピーを入れ、キューが所有する。合計が 1 MiB を超えたら接続を閉じ、ログに残す。
- TCP / WS 用の streambuf（`MessageBuf`）は、`sync()` が呼ばれるまでバッファを伸ばし続け、途中では送信しない。メッセージ境界は `sync()` だけで決まる（I5）。

### 4.5 トランスポートごとの仕様

| | UDP | TCP | WebSocket |
|---|---|---|---|
| 待受 | 6000 / 6001 / 6002（`server::port` / `coach_port` / `olcoach_port`）。IPv4 のみ | `server::tcp_port` / `tcp_coach_port` / `tcp_olcoach_port`。既定は 0（無効）。-1 なら対応する UDP のポートと同じ番号（8章 #14）。IPv6 のデュアルスタックで待ち受け、使えなければ IPv4 のみ | `server::ws_port` の1ポート。既定は 0（無効）。URL のパス（`/player` `/monitor` `/coach` `/olcoach`）でサービスを振り分ける。未知のパスには 404、Upgrade でない要求には 400 を返す。待受アドレスは TCP と同じ |
| 接続確立 | 専用ポート方式 | accept した接続がそのまま専用になる | HTTP アップグレードした接続がそのまま専用になる |
| サーバ → クライアント | データグラム1つが1メッセージ（末尾 `'\0'`） | メッセージを `'\0'` 区切りで連結 | テキストフレーム1つが1メッセージ。末尾の `'\0'` は除く |
| クライアント → サーバ | データグラム1つが1メッセージ | `'\0'` または `'\n'` 区切り。末尾の `'\r'` は取り除く | フレーム1つが1メッセージ。末尾の `'\0'`、`'\n'`、`'\r'` は取り除く |
| 受信の最大長 | MaxMesg | MaxMesg。超えたら切断 | MaxMesg（Beast の `read_message_max`）。超えたら切断 |
| 圧縮 `(compression N)` | 不可（8章 #9）。`(warning compression_unsupported)` を返す | 不可。`(warning compression_unsupported)` を返す | 不可。`(warning compression_unsupported)` を返す |
| モニタのバージョン | すべて | v3 以上と JSON | v3 以上と JSON |
| 専用ポートの強制 | これまでどおり | 不要 | 不要 |
| 未受理の接続 | - | 10秒以内に受理されなければ閉じる。同時に 64 本まで | ハンドシェイク中を含めて、TCP と同じ |

- モニタ v1 / v2 は TCP / WS では受け付けない（`(error illegal_client_version)`）。表示データをバイナリの構造体（`dispinfo_t` / `dispinfo_t2`）のまま送るため、`'\0'` 区切りにも WS のテキストフレームにも載らない（`Connection::isBinarySafe()`）。圧縮も同じ理由で TCP / WS では不可にしていたが、その後、どのトランスポートでもサポートをやめた（8章 #9）。
- モニタは UDP と同じく、プレイヤ用のサービス（TCP のプレイヤ用ポート、WS の `/player`）へ `dispinit` を送る。主な用途としては、JSON モニタ（dispinit の version が負）を WS で使うことを想定している。
- WS では、ウェブモニタの作者に分かりやすいように、モニタ専用のパス `/monitor` も用意した。`/monitor` は `dispinit` だけを受け付け、それ以外（プレイヤの init や reconnect など）には `(error unknown_command)` を返す。接続は切らないので、続けて `dispinit` を送れば受理される。
  - Stadium にモニタ専用の Acceptor の列 `M_monitor_acceptors` と `recvFromMonitorPort()` を追加した。受理したモニタからの受信は、これまでどおり `recvFromPlayerPort()` で行う。
- WS の Origin は `server::ws_allowed_origins`（カンマ区切り）で制限できる。空なら全許可。Origin ヘッダのない要求（ブラウザ以外のクライアント）は常に許可する。許可されない Origin には 403 を返す。コマンドラインでは値を引用符で囲み、空白を入れない（rcssserver のパーサの制約）。
- WS の実装は、ポートを持つ `WSServer` と、サービスごとの窓口の `WSAcceptor` に分かれる。`WSServer` はハンドシェイク中と未受理の接続をパスごとに持ち、4つの `WSAcceptor` が共有する。
- TCP と WS に共通の部品として、送信メッセージを `sync()` まで溜める `MessageBuf`、未受理の接続を管理する `PendingConnections`、デュアルスタックで待ち受ける `listen_tcp()` がある。
- WS で送るテキストメッセージは UTF-8 として正しい必要がある。サーバの出力はほぼ ASCII だが、クライアントが送った文字列（コーチの freeform など）に不正な UTF-8 が含まれると、ブラウザは接続を閉じる（1007）。
- TCP / WS のポートは既定では無効にし、明示的に有効にしたときだけ待ち受ける（8章 #14）。サービスごとにポートを分けるのは、最初のメッセージからサービスを判別できないためである（プレイヤとオンラインコーチの init は同じ形で、オフラインコーチは init なしのコマンドも実行する）。

### 4.6 Phase 6: 待ちのイベント駆動化

**方式**: 当初は、メインループを `io_context.run()` にし、sync モードの think / done を状態機械にする案だった（AsioTimer を使う）。実装では、StandardTimer / SyncTimer のループと `doSendThink()` の手順はそのまま残し、待ちだけを置き換えた。

- 理由: 変更が小さく、処理の順序が変わらないので、挙動の差を確かめやすい。シミュレーションの処理は I/O のハンドラの外で動くので、「ハンドラの中でブロックしない」も自然に守られる。
- 待ちは `Timeable::waitUntil()` で、Stadium が実装する（`Stadium::runIO()`）。
  - `steady_timer` で期限まで待ちながら、`run_one()` で I/O のハンドラを動かす。期限の精度は timerfd による（`run_until()` は epoll のタイムアウトがミリ秒に切り上げられるので使わない）。
  - シグナルで Stadium が無効になったら、期限の前でも戻る。

**通常モード（StandardTimer）**

- tick は `steady_clock` の絶対時刻で、TIMEDELTA（10ms）ごとに決まった時刻に来る。これまでは「10ms − 処理時間」だけ sleep していたため、sleep の遅れの分だけ周期が伸びていた（1 サイクル 100.6ms）。
- lcmt は tick ごとに TIMEDELTA を足す。処理が TIMEDELTA より長引いて過ぎてしまった tick は、追いかけずに飛ばし、その分も lcmt に足す。

**sync モード**

- SyncTimer は変更しない。
- `doSendThink()` の (done) 待ちは、受信 → 判定 → (done) がそろっていなければ、メッセージの到着か 1250ms の期限まで待つ（`runIO( ..., true )`）、を繰り返す。到着ごとに処理するのではなく、起きたら `doRecvFromClients()`（シャッフル順の一括処理）を呼ぶ（I2、I3）。
- `synch_micro_sleep` の待ちは残す。sync モードのまま、人が観察できるように試合をわざと遅くするのに使われているため。
  - 1回目の受信は `(think)` の `synch_micro_sleep` 後。2回目以降は、前の受信の `synch_micro_sleep` 後と、メッセージの到着の、遅いほう。メッセージが待ちより前に届く限り、これまでと同じ時刻になる。
  - 0 以下なら待ちを飛ばす。既定値は 1 から 0 に変えた（1µs の待ちはポーリングの間隔としては意味があったが、今は受信の前にタイマーを1つ足すだけで、TCP の sync モードで約2割のサイクルを失っていた）。
  - `ServerParam::synchMicroSleep()` は `unsigned int` に変換して返していたので、負の値で受信のたびに約 71 分待っていた。`int` を返すように直した。
- 試合開始前の 50ms と、モニタ接続時の 20ms の `sleep_for` も、I/O を回しながら待つ `runIO()` に置き換えた。

**UDP の到着の通知**

- ソケットの `async_wait( wait_read )` で知る。ハンドラは待ちのフラグを下ろすだけで、読むのはこれまでどおり `doRecvFromClients()` の中。
- `async_wait` は、ソケットを読み切った（would_block になった）時点と、ソケットを開いた時点にだけ登録する。読み切る前に登録すると、すぐにまた完了して、読むまでループになる。
- asio（Boost 1.83 で確認）の epoll リアクタは、空の待ち行列に待ちを登録するときに `EPOLL_CTL_MOD` で登録し直すので、未読のデータがあれば待ちはすぐ完了する。また Linux の epoll は、通知を取り出す時点の状態を見直すので、読み終えたデータの通知は報告されない。小さなプログラムで、次の4点を確かめた。
  1. 待ちの間に届けば完了する。
  2. 届いてからリアクタが動く前に読み切った場合は、完了しない。
  3. 未読のデータがあるときに登録すると、すぐ完了する。
  4. ハンドラの中で未読のまま登録し直すと、すぐまた完了する。
- UDPConnection の待ちは weak_ptr で接続を参照し、接続を生かし続けない。UDPAcceptor は TCPAcceptor と同じく `this` を参照し、デストラクタでソケットを閉じて待ちを取り消す。

**シグナルと終了**

- SIGINT / SIGTERM / SIGHUP は `boost::asio::signal_set` で受け、ハンドラ（I/O のハンドラと同じく、待ちの中で動く）で `finalize()` を呼ぶ。これまでの `sigHandle()` は、シグナルハンドラの中で `finalize()` を呼んでいて、非同期シグナル安全ではなかった。
- signal_set は main で、`Std.init()` のあとに作る。io_context が init の中で作られるためである。init の途中でシグナルを受けると、既定の動作（その場で終了）になる。
- `alive()` が false になれば、タイマーのループが終わる。`io_context.stop()` や、Acceptor を明示的に閉じる処理は要らなかった。

**計測**（Release ビルド。比較は Phase 5 の 572d6b3）

| 項目 | Phase 5 | Phase 6 |
|---|---|---|
| 通常モードの周期（20秒間） | 100.607ms（20秒で 120ms 遅れる） | 100.000ms |
| 通常モードの tick 間隔の標準偏差 | 0.125ms | 0.047ms |
| sync モード、1人が (done) を送らないときの周期 | 1250ms | 1250ms |
| 上記のときのサーバの CPU 使用率 | 3% | 0% |
| sync モードの (done) 待ちで SIGINT を受けてから終了まで | 1.07秒 | 0.00秒 |
| 22人の UDP プレイヤで sync モードの前半（3000 サイクル） | 1.26秒 | 1.12秒、取りこぼしなし |
| `synch_micro_sleep=100000` の sync モードの周期（すぐ答える / 150ms 遅れて答える） | 100.1ms / 200.2ms | 100.1ms / 200.1ms |
| 一定時間に TCP / WS のクライアントが答えた think の数 | 約 11 万 / 約 6 万 | 約 35 万 / 約 30 万 |

TCP / WS の sync モードが速くなったのは、メッセージが届くとすぐ起きるようになったためである（Phase 5 までは、次のポーリングまで約 55µs 待っていた）。think の数は、並行して動かすテストの数によって変わる。

### 4.7 コードの配置とビルド

- 新しいコードは `src/transport/` に置き、名前空間は `rcss` とする。ファイル名はクラス名に合わせ、1ファイル1クラスとする（`connection.h`、`acceptor.h`、`udpconnection.{h,cpp}`、`udpacceptor.{h,cpp}`、`datagrambuf.{h,cpp}`、`tcpconnection.{h,cpp}`、`tcpacceptor.{h,cpp}`、`wsconnection.{h,cpp}`、`wsacceptor.{h,cpp}`、`wsserver.{h,cpp}`、`messagebuf.{h,cpp}`、`pendingconnections.{h,cpp}`、`tcplisten.{h,cpp}`、`closeonexec.{h,cpp}`）。
  - サーバ内部の部品なので、独立したライブラリの置き場である `rcss/` の下には置かない。rcssclient で TCP / WS が必要になったら、インタフェースが固まった時点で別ライブラリとして切り出す。
  - rcssclient は、送信に `DatagramBuf` を共有している（`setDest()` で専用ポートに宛先を切り替える）。
- rcssserver と rcssclient は rcss::net を使わなくなり（bf327da）、その後 rcss/net を削除した（8章 #15）。
- asio は、ソケットに close-on-exec を設定しない。そのままでは、サーバが起動するプログラム（`server::team_l_start` など）がソケットを引き継ぎ、サーバの終了後もポートを使用中のままにする。rcss::net は設定していたので、Phase 5 で `set_close_on_exec()` を作り、UDP / TCP / WS のすべてのソケット（待受と受理した接続）に設定した。
- Boost の最低バージョンを 1.44 から 1.70 に上げた。`io_context` は 1.66 から、Beast の `tcp_stream` は 1.70 から使える。1.70 なら Ubuntu 20.04 の 1.71 でもビルドできる。asio と Beast はヘッダオンリー。
- asio はスレッド関連の関数を使うので、`Threads::Threads`（autotools では pthread）をリンクする。
- autotools と CMake の両方に同じ変更を入れた。各コミットで、両方のビルドと `make distcheck` が通ることを確かめた。

## 5. 移行計画

各 Phase の最後に、6章の回帰確認を行った。

| Phase | 内容 | 完了条件 | 結果 |
|---|---|---|---|
| 1 | Connection を導入する（asio なし）。既存の `UDPSocket` と `SocketStreamBuf` を包んだ `UDPConnection` を作り、`RemoteClient` が Connection 経由で送受信するようにする | 挙動が変わらない | 完了（d75f6f8） |
| 2 | Acceptor を導入する。待受ソケット、アドレスによる振り分け、`sendTo*()` を `UDPAcceptor` に移し、Stadium を 4.3 の流れにする。範囲外書き込みも解消する | 挙動が変わらない | 完了（110bc2a） |
| 3 | asio と TCP を導入する。`io_context` を作って `doRecvFromClients()` から `poll()` し、`TCPAcceptor` / `TCPConnection` を追加する | TCP で接続したモニタが1試合分のデータを受信できる。UDP の回帰確認が通る | 完了（c790844）。TCP のモニタが、sync モードの1試合（6000 サイクル）の show を取りこぼしなく受信 |
| 4 | WebSocket を導入する。Beast で `WSServer` / `WSAcceptor` / `WSConnection` を追加する | ブラウザのテストページから JSON モニタとして接続し、1試合分のデータを受信できる | 完了（5ae4acc）。ヘッドレスのブラウザで、全サイクルの show を受信 |
| 5 | UDP を asio 化する。`UDPAcceptor` / `UDPConnection` の中身を asio に置き換える（同期の非ブロッキング受信。AsioUDPSocket は使わない。7章） | 挙動が変わらない | 完了（572d6b3）。送信の待ちの不具合を 8537a9a で修正 |
| 6 | 待ちをイベント駆動にする（4.6） | 通常モードの周期が実時間と一致する。sync モードで1試合を完走する | 完了（79cb8f3） |

Phase 1〜2 はトランスポートを増やさない純粋なリファクタなので、問題が起きても原因を切り分けやすい。UDP の asio 化を最後に回したのは、UDP では asio の利点が小さく、実績のある経路を最後まで残せるからである。

## 6. 回帰確認

確認する項目:

- プレイヤ: init（v3 と v8 以上の代表的なバージョン、goalie 付き）、reconnect、bye、エラー応答（`no_more_team`、`illegal_teamname` など）
- 専用ポート: v8 以上のクライアントが 6000 番へ init 以外を送るとエラーになる。v7 以下なら処理される
- オンラインコーチの init。オフラインコーチの init と、init なしのコマンド。coach mode が無効なときのエラー
- モニタ: dispinit（v1、v3 以上、JSON）、`max_monitors` を超えたときのエラー、モニタからのコマンド（`dispstart` など）
- 圧縮 `(compression N)` に、プレイヤ、コーチ、オンラインコーチ、モニタのどれにも `(warning compression_unsupported)` を返し、その後も圧縮せずに送受信する（#9 の後）
- 通常モードで1試合: 1サイクルが実時間の 100ms からずれない
- sync モードで1試合: 「Someone missed a cycle」が出ずに完走する
- クライアントのプロセスを kill すると、切断として扱われる
- Ctrl-C で終了し、ログ（rcg / rcl）が正しく閉じられる
- サーバが起動するプログラム（`team_l_start` / `team_r_start`）がソケットを引き継がない
- TCP / WS: 区切り、最大長、未受理の接続のタイムアウト、読まないクライアントの切断、WS のハンドシェイク（404 / 400 / 403）

確認の方法:

- UDP / TCP / WS の各項目は、Python のテスト用クライアントでサーバとやり取りし、結果を JSON に記録して、変更前のビルドと比べた。実行ごとにばらつく項目を除いて、UDP 48 項目、TCP 27 項目、WS 38 項目を比べる。最終的な差分は、Phase 6 で終了時の待ちがなくなったことによる1項目（`sync.missed_cycles`）だけ。
- ほかに、ブラウザ（ヘッドレスの Chrome）での1試合、周期と CPU 使用率の計測（4.6）、引き継がれるファイル記述子の確認、rcssclient の新旧比較を行った。
- ASan / UBSan 付きのビルドでも流した。報告は、移行前からある `object.h:594`（`M_collided`）だけ。
- これらのテストスクリプトは、リポジトリに入れていない（作業ツリーの `test/` に置いていた）。

試合ログの完全一致による比較は難しい。理由は2つある。

- シミュレータの乱数シードをパラメータで指定できない（`serverparam.cpp` で `random_seed` の登録がコメントアウトされている）。
- クライアント順のシャッフルが共有の乱数を消費し、その回数が実時間に依存する（sync モードの待ちループも同じ）。

## 7. 既存ドラフトの扱い

どちらの下書きも使わず、削除した（どちらも git の管理外だった）。

**AsioUDPSocket**（`rcss/net/asiosocket.{h,cpp}`）: Phase 5 では使わなかった。非同期受信でキューに積む作りにすると、読む時点がずれ（処理中に届いたデータグラムは次の tick に回る）、受信データがユーザ空間のキューに溜まる。「挙動が変わらない」を優先して、同期の非ブロッキング受信にした。下書きには次の問題もあった。

- `async_send_to` に渡したバッファは、完了まで生きている必要がある。Linux（epoll）ではその場で送信されることが多いので普段は問題が出ず、送信バッファが詰まったときだけデータが壊れる。
- `handleReceive()` はエラー時に受信を再登録しないので、1回のエラーで待受が止まる。
- 受信のたびに、アドレスの文字列化と `gethostbyname` を通っている。
- 受信データが `'\0'` で終端されていない。
- ラムダが `this` をキャプチャしている。

**AsioTimer**（`src/asiotimer.{h,cpp}`）: Phase 6 では使わなかった。メインループを `run()` にする方式をやめ、既存のタイマーの待ちだけを置き換えたため（4.6）。下書きには次の問題もあった。

- 待機の前に `M_last_update_time` を更新しているため、待機時間が「10ms − 処理時間」と「0」を交互に繰り返す。同じロジックを切り出して計測すると毎秒 150〜190 tick（本来は 100）で、シミュレーションが実時間の 1.5〜1.9 倍の速さで進む。
- 保留中の受信があると `io_context.run()` が戻らず、Ctrl-C で終了しない。
- `processServerTasks()` が `static` 変数で状態を持っている。

## 8. 未決事項

| # | 論点 | 状況 |
|---|---|---|
| 1 | TCP の受信で `'\n'` も区切りとして受け付けるか | **決定**: 受け付ける。末尾の `'\r'` も取り除く。サーバからの送信は常に `'\0'` 区切り |
| 2 | TCP / WS での圧縮 | **決定**: 不可とし、zlib なしでビルドしたサーバと同じ `(warning compression_unsupported)` を返す。その後、UDP でもサポートをやめた（#9） |
| 3 | TCP のポート番号と設定パラメータ名 | **決定**: `server::tcp_port` / `tcp_coach_port` / `tcp_olcoach_port`。既定は 0（無効）。-1 なら UDP と同じ番号（#14）。バージョン 999 で登録し、クライアントへ送る `(server_param ...)` には含めない |
| 4 | WS のポート番号、Origin チェック、bind アドレス | **決定**: `server::ws_port`（既定 0 で無効）。Origin は `server::ws_allowed_origins` の許可リストで、空なら全許可。待受はすべてのインタフェース（TCP と同じデュアルスタック） |
| 5 | TCP / WS の送信キューの上限 | **決定**: 1 MiB を超えたら切断し、ログに残す（TCP / WS とも） |
| 6 | TCP / WS の未受理接続の扱い | **決定**: 10秒以内に受理されなければ閉じる。未受理の接続は同時に 64 本まで（WS はハンドシェイク中を含む） |
| 7 | IPv6 | **決定**: TCP / WS は IPv6 のデュアルスタックで待ち受け、使えなければ IPv4 のみ。UDP は当面 IPv4 のまま |
| 8 | `rcssclient` の移行 | **完了**: 当初は範囲外としたが、rcss::net を取り除けるようにするため移した。UDP のまま、ソケットを asio に、送信をサーバの `DatagramBuf` に置き換え（bf327da）、`select()` のループも asio のイベント処理にした（bad983d）。標準入力は `posix::stream_descriptor` で読む。asio は読む記述子に `O_NONBLOCK` を付けて戻さないので、端末は `ttyname()` で開き直す（端末は標準出力やシェルと「開いたファイル」を共有している）。標準入力が EOF になっても受信を続ける（これまでは CPU を使い切っていた）。8192 バイトのデータグラムの最後の1バイトを落とさなくなった。名前解決に失敗したときは、これまで 0.0.0.0（ローカル）に送っていたが、エラーを表示して終わる |
| 9 | 圧縮 `(compression N)` を有効にしたクライアントへの送信 | **決定**: 19.0.0 でも、圧縮を有効にしたプレイヤへのキックオフ時の送信が失敗し（stream が失敗状態になる）、そのクライアントが切断される。原因を調べる代わりに、次のメジャーバージョンで圧縮のサポートをやめる。<br>理由: 実際に使われた形跡がない（この不具合や、Phase 1 で直した圧縮時のクラッシュの報告がない。librcsc は圧縮に対応しているが既定で無効。rcssmonitor-web は非対応として扱う）。今の LAN では帯域を節約する意味がほとんどない。<br>互換性: `(compression N)` には、プレイヤ、コーチ、オンラインコーチ、モニタのどれにも、zlib なしでビルドしたサーバと同じ `(warning compression_unsupported)` を返す。この応答はマニュアルに書かれた既存のもので、librcsc は `(ok compression N)` を受け取ったときだけ圧縮に切り替えるので、圧縮を設定したチームもそのまま動く。<br>サーバの `RemoteClient` から圧縮の送受信を削り、`compress.h` も削除した。rcssclient も圧縮を扱わなくなり、zlib と librcssgz をリンクしない。zlib と `rcss/gzip` はログファイルの圧縮（`game_log_compression` / `text_log_compression`）のために残す。<br>リリースのときに NEWS に書く。マニュアル（別リポジトリ）の更新は #16 |
| 10 | sync モードのハーフタイム | **解決とみられる**: 19.0.0 でも、モニタが接続されていないと、前半終了時（3000 サイクル）に「Someone missed a cycle」が1回出ていた。Phase 6 の計測では、前半終了後も (done) を送り続けると出なかった。出ていた1回は、テストが (done) を送るのをやめて SIGINT を送ったあと、サーバが (done) を 1250ms 待つ間に出ていた（「Server Killed」の後に出る）。Phase 6 では待ちをすぐ抜けるので出ない |
| 11 | SIGINT での終了時のクラッシュ | **対処済み**: Phase 4 の検証中に1回だけ、WS クライアントの接続中に SIGINT で SIGSEGV になった（再現せず、ASan でも検出なし）。最も疑わしかったのは、シグナルハンドラの中で `Stadium::finalize()` を呼ぶ設計。Phase 6 で `signal_set` に移り、`finalize()` はイベントループの中で呼ばれるようになった |
| 12 | asio のソケットの close-on-exec | **解決**（Phase 5）: すべてのソケットに `set_close_on_exec()` で設定する |
| 13 | UDP の送信バッファが詰まったときの待ち | **解決**（8537a9a）: Phase 5 の `DatagramBuf` は `basic_socket::wait()` で待とうとしたが、非ブロッキングのソケットでは待たずに失敗し、クライアントを切断していた。`poll()` で待つように直した。ループバックの UDP では送信バッファが詰まらないので、受信側が読まない AF_UNIX のデータグラムソケットで確かめた |
| 14 | TCP のポート番号の既定値 | **決定**: 既定は 0（無効）のままにし、-1 を指定すると対応する UDP のポートと同じ番号で待ち受けるようにする（`tcp_port` → `server::port`、`tcp_coach_port` → `coach_port`、`tcp_olcoach_port` → `olcoach_port`）。<br>-1 を指定すれば、1台で複数のサーバを動かすときも、UDP のポートをずらすだけで TCP も一緒にずれる。<br>既定を -1 や 6000〜6002 にして TCP を既定で有効にする案は、見送った。TCP のポートを開けないとサーバは起動に失敗するので、X11 が TCP で待ち受けている環境（6000〜6063 番）などで、今まで起動できたサーバが起動しなくなる。トレーナ用のポートや IPv6 側でも、選ばないまま待ち受けることになる。古い `server.conf` には TCP のパラメータがないので、更新しただけで TCP が有効になる。TCP のクライアントが広まった段階で、既定値を改めて検討する |
| 15 | rcss::net を取り除くか | **決定・完了**: リポジトリの中に使うものはなく、外部の利用者も知られていないので、取り除いた。`rcss/net` を削除し、`rcss/Makefile.am` の `SUBDIRS` から `net` を、`rcss/CMakeLists.txt` から `add_subdirectory(net)` を、`configure.ac` から `rcss/net/Makefile` を消した。CMake と autotools（`make distcheck` も）のビルドが通ることを確かめた。<br>コードを別の場所に退避することはしなかった。削除したコードは rcssserver-19.0.0 のタグの `rcss/net` と同じなので、そこから取り出せる（例: `git show rcssserver-19.0.0:rcss/net/udpsocket.hpp`、`git archive rcssserver-19.0.0 rcss/net`）。単独のリポジトリにするなら、`git subtree split --prefix=rcss/net rcssserver-19.0.0` で履歴ごと切り出せる。<br>librcssnet とそのヘッダ（`$(includedir)/rcss/net`）はインストールされなくなるので、リリースのときに NEWS に書く |
| 16 | マニュアル（別リポジトリ）の更新 | **未対応**: マニュアルは別リポジトリ（rcsoccersim/manual の `source/*.rst`）にある。リリースまでに次の点を直す（行番号は manual の 19e75d6 時点）。<br>(1) `soccermonitor.rst` の compression の節（426〜440行）: 圧縮はサポートせず、どのクライアントにも `(warning compression_unsupported)` を返す（#9）。<br>(2) UDP だけを前提にした記述（`overview.rst` 24〜26行、`introduction.rst` 159〜160行、`soccermonitor.rst` 61行）: TCP と WebSocket でも接続できることを書く（4.5）。<br>(3) 新しいサーバパラメータ（`tcp_port` / `tcp_coach_port` / `tcp_olcoach_port` / `ws_port` / `ws_allowed_origins`）の説明と、`synch_micro_sleep` の既定値の変更（1 → 0、0 以下なら待たない）。マニュアルにはサーバパラメータの一覧がない（`synch_micro_sleep` は `gettingstarted.rst` の出力例に出てくるだけ）ので、書く場所から決める |
