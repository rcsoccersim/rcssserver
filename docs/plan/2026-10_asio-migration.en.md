# Moving the network layer to boost::asio, with TCP and WebSocket support

- Status: Phases 1 to 6 are done (2026-11-06). The open issues in section 8 remain.
- Branch: `feature/asio-migration` (not merged into master yet)
- Japanese version: [2026-10_asio-migration.md](2026-10_asio-migration.md). Keep both versions in step.

## 0. Progress

| Phase | Work | Commit | Status |
|---|---|---|---|
| 1 | Add Connection (without asio) | d75f6f8 | Done |
| 2 | Add Acceptor | 110bc2a | Done |
| 3 | Add asio and TCP | c790844 | Done |
| 4 | Add WebSocket | 5ae4acc | Done |
| 5 | Move UDP to asio | 572d6b3, fix 8537a9a | Done |
| 6 | Wait for events instead of sleeping and polling | 79cb8f3 | Done |

Work after the plan:

- bf327da: rcssserver and rcssclient no longer link with rcss::net. rcssclient also moved to an asio UDP socket.
- bad983d: the `select()` loop of rcssclient is replaced with asio event handling.
- 9996511: opening the acceptors moves out of `Stadium::init()` into `openAcceptors()` and other functions.
- eaa4625: compressed messages `(compression N)` are no longer supported (section 8, #9).

Status of the goals in section 1:

- Replace rcss::net: done. Nothing in the repository uses rcss::net. rcss/net itself is still built and installed (section 8, #15).
- Connect over TCP and WebSocket: done. Players, offline coaches, online coaches and monitors can all use them.
- Existing UDP clients: the regression checks of each phase showed no change in behavior (section 6).

## 1. Goals and scope

**Goals**

- Replace the hand-written socket layer (`rcss::net`) with boost::asio.
- Let clients (players, coaches and monitors) connect over TCP and WebSocket, in addition to UDP.
- Keep existing UDP clients working without changes.

**Out of scope**

- Changes to the content of the messages (S-expressions / JSON)
- Changes to the meaning of the simulation, and multithreading
- TLS (wss). A reverse proxy can terminate it if needed.
- Moving `rcssclient` ([src/client.cpp](../../src/client.cpp)). It was out of scope at first, but it was moved after the plan so that rcss::net can be removed (section 8, #8).

## 2. The design before the migration

This section describes the design of 19.0.0, for the record.

### Listening and establishing connections

- Stadium has three UDP sockets to listen on: 6000 for players (and monitors), 6001 for offline coaches and 6002 for online coaches.
- A message from an unknown address is taken as an init message (`parsePlayerInit()` / `parseMonitorInit()` / `parseCoachInit()` / `parseOnlineCoachInit()`).
- When the client is accepted, its `RemoteClient` opens a new UDP socket, connects it to the client's address and sends from that socket. The client then sends to the port that the replies come from (the dedicated port).
- An error reply to a rejected client is sent from the listening socket (`sendToPlayer()` / `sendToCoach()` / `sendToOnlineCoach()`).
- When an accepted client sends a message to the listening port, the sender is found by its address and the message goes to `RemoteClient::undedicatedRecv()`. Clients that must use the dedicated port (players and coaches of v8 or later, monitors of v2 or later and JSON monitors) get an error.
- A client is identified by its address (`RemoteClient::getDest()`). `getDest()` is used only in these four places that find the sender.

### Sending

- The senders write to `RemoteClient::getTransport()` (a `std::ostream`) and complete a message with `<< std::ends << std::flush` (56 places). `Player::send()` and others use the same ostream.
- Under the ostream is a `SocketStreamBuf` (with a `gzstreambuf` on top when compressed). One flush sends one UDP datagram. When the 8192-byte buffer is full, it is sent at that point, so a large message is split into several datagrams.

### When messages are received and processed

- `StandardTimer` calls `Stadium::doRecvFromClients()` every recv_step (10 ms by default).
- Messages are processed in the following order. In each group the order of the clients is shuffled, and each client is processed until it has no more data (`recv_from_clients()`).
  1. Players, then monitors, then the listening socket on port 6000
  2. Online coaches, then the listening socket on port 6002
  3. Offline coaches, then the listening socket on port 6001
  4. `removeDisconnectedClients()`
- A received message is parsed at once and applied to the state of its client (`RemoteClient::processMsg()` → `parseMsg()`).

### Sync mode

- `SyncTimer` loops without sleeping. After `Stadium::doSendThink()` sends `(think)`, it calls `doRecvFromClients()` every `synch_micro_sleep` and waits for `(done)` from everyone (up to 1250 ms).
- In sync mode, all messages are received in this waiting loop.

### Existing problems (fixed during the migration)

- In three places such as `udp_recv_message()`, a datagram of exactly MaxMesg bytes makes `message[len] = '\0'` write one byte past the array. When an accepted client sends to the listening port, `parseMsg()` also writes one byte past the same array. This was confirmed with 19.0.0 built with ASan. → Fixed in phase 2.
- `RemoteClient::close()` deletes the ostream of the transport, but the senders keep references to it. When a client is closed because of a send error and another message (e.g. from the referee) is sent in the same step, the server writes to the deleted ostream and crashes. This happened with 19.0.0 at kick off, with a player that used compression. → Fixed in phase 1 (the ostream stays, and `close()` only detaches its streambuf).
- `Addr` is just a `sockaddr_in`, so it is IPv4 only. → TCP and WebSocket support IPv6. UDP stays IPv4 only (section 8, #7).

## 3. Compatibility to keep (invariants)

All phases kept these.

- **I1 UDP protocol**: do not change the following.
  - The port numbers and the dedicated ports
  - The handling of messages sent to the listening port (processed for clients older than v8, an error for v8 or later)
  - Where error replies are sent from
  - The message format terminated by `'\0'`
- **I2 When messages are processed**: messages from clients are processed only in `doRecvFromClients()`, never in the I/O handlers.
- **I3 The order of processing**: keep the order of section 2 (the order of the groups, and shuffling in each group).
- **I4 Sync mode**: keep the steps of think and done, and the timeout. Phase 6 replaced the polling with waiting for messages, but the steps and the timeout did not change.
- **I5 Message boundaries**: one flush of `transport()` is one message. The senders do not change.
- **I6 Threads**: the simulation and the I/O run in the same thread, with no locks.

## 4. Design

### 4.1 Abstractions

The abstractions are not sockets but the following two.

- **Connection**: the path of the messages of one client. Owned by `RemoteClient`.
- **Acceptor**: one for each pair of a service (players / offline coaches / online coaches) and a transport. It gives Stadium the messages that are not tied to a client yet, and creates a Connection when the client is accepted. Owned by Stadium.

The name Acceptor avoids confusion with the existing `rcss::Listener` (a receiver of audio, [src/audio.h](../../src/audio.h)).

A client is identified by its Connection object.

```cpp
// the path of the messages of one client (implemented for UDP, TCP and WebSocket)
class Connection {
public:
    virtual ~Connection() = default;

    // Sending: write here. One pubsync() (std::flush) completes one message.
    // RemoteClient puts an ostream on top of it.
    virtual std::streambuf & outbuf() = 0;

    // Receiving: take one received message. false if there is none (never blocks).
    // The length never exceeds MaxMesg.
    virtual bool popMessage( std::string & msg ) = 0;

    virtual bool isOpen() const = 0;
    virtual void close() = 0;

    // true if a message may contain any byte. false for TCP, which uses '\0'
    // as the delimiter. If false, the binary monitors (v1 / v2) cannot be
    // used.
    virtual bool isBinarySafe() const = 0;
};

// a message received on a listening port and not tied to a client yet
struct Inbound {
    std::string message;
    std::shared_ptr< Connection > reply;  // for replies. UDP: sent from the listening socket / TCP and WS: the connection itself
    const Connection * known = nullptr;   // UDP only: the Connection of an accepted client that sent to the listening port
};

class Acceptor {
public:
    virtual ~Acceptor() = default;

    // Take one pending Inbound. false if there is none (never blocks).
    virtual bool popInbound( Inbound & in ) = 0;

    // Accept the sender of in and return its own Connection (nullptr on failure).
    // UDP: open a new socket and connect it to the client / TCP and WS: return the connection itself
    virtual std::shared_ptr< Connection > accept( const Inbound & in ) = 0;
};
```

Notes:

- `Connection::peerName()` and `Acceptor::close()` of the first draft were not implemented, because nothing needed them.
- `isBinarySafe()` was added in phase 3.
- `known` reproduces how UDP finds the sender (an accepted client is found by its address and the message goes to `undedicatedRecv()`). Stadium finds the client that has the `known` Connection and calls its `undedicatedRecv()`. It is always `nullptr` for TCP and WebSocket.
- For TCP and WebSocket, the messages from a connection that is not accepted yet come one by one as Inbound.
  - Once the client is accepted, its later messages go to the receive queue of its Connection. A command sent right after init is therefore not taken as init.
  - If the client is not accepted, its next message comes as the next Inbound. This is the same as a message from an unknown address over UDP.
- Connections are shared with `std::shared_ptr`, because the handlers of asio's asynchronous operations must keep them (`shared_from_this()`).
- `RemoteClient` no longer has `open()` / `connect()` / `getDest()`. It takes a Connection with `attach( std::shared_ptr< Connection > )`. Reconnect goes the same way.
- `parse*Init()` / `init*()` take `const Inbound &` instead of `const Addr &`. `sendToPlayer( msg, addr )` and the others became sending to `in.reply`.

### 4.2 Threads and the event loop

- There is only one `io_context`, run in the main thread (I6). Stadium owns it.
- **Phases 3 to 5**: the existing timers were used as they were, and `doRecvFromClients()` called `io_context.poll()` at its beginning.
  - `poll()` stops the io_context when there is no pending asynchronous operation. So the io_context is restarted before `poll()` if it is stopped (no work guard).
- **Phase 6**: the loops of the timers stay, and the waits (`sleep_for` and the polling every `synch_micro_sleep`) are replaced with waits that run the io_context (4.6). The I/O handlers run while the server waits.
- At every stage, the I/O handlers only queue the received data (I2).

### 4.3 How messages are received

```
Stadium::doRecvFromClients()
  ├─ delayed effects (doLongKick)
  ├─ io_context.poll()
  ├─ recvFromPlayerPort()
  │    ├─ players, then monitors (each shuffled): RemoteClient::recv() → Connection::popMessage()
  │    └─ the acceptors for players (UDP → TCP → WS /player): popInbound()
  │         ├─ known → undedicatedRecv() of that client
  │         └─ not known → parseMonitorInit() / parsePlayerInit() → accept(), or an error to in.reply
  ├─ recvFromMonitorPort(): the acceptor only for monitors (WS /monitor)
  ├─ recvFromOnlineCoachPort(): online coaches (shuffled) → the acceptors for online coaches
  ├─ recvFromOfflineCoachPort(): offline coaches (shuffled) → the acceptors for offline coaches
  └─ removeDisconnectedClients()
```

The acceptors of each service are in the order UDP, TCP and WS (`Stadium::openAcceptors()`), and they are processed in this order.

### 4.4 Sending and buffers

- **UDP**: sending is synchronous (`send` / `send_to`).
  - asio's `async_send_to` is not used. Its buffer must stay alive until the operation completes, and it has no benefit for UDP.
  - `DatagramBuf` replaces `SocketStreamBuf` (phase 5). A full buffer of 8192 bytes is sent at that point, as before (I1).
  - While the send buffer of the socket is full (EWOULDBLOCK), the server waits with `poll()` until the socket can send. rcss::net retried until the datagram was sent (a busy loop). Phase 5 replaced this with `basic_socket::wait()`, but asio's `wait()` fails at once on a non-blocking socket, so the client was disconnected (fixed in 8537a9a).
  - If sending fails, the stream gets the error state and errno tells the reason (`RemoteClient::send()` uses it to ignore ECONNREFUSED). The data that could not be sent stays in the buffer. This is also the same as rcss::net.
- **TCP / WS**: each Connection has a send queue, and at most one asynchronous write is in progress.
  - On TCP, when no asynchronous write is in progress, the message is first sent at once without blocking, and only the rest is queued.
  - WS (Beast) cannot send synchronously without blocking, so it always writes asynchronously. Up to phase 5, the second and later messages in a tick waited for the next `poll()` (up to one recv_step). Since phase 6, the handlers run during the waits, so there is no such delay.
  - `TCP_NODELAY` is set to avoid the delay of the Nagle algorithm (on both TCP and WS). Without it, a cycle in sync mode took about 40 ms (measured on WS).
  - The queue holds a copy of each message. If it holds more than 1 MiB, the connection is closed and logged.
- The streambuf for TCP and WS (`MessageBuf`) grows its buffer until `sync()` is called, and never sends in between. Only `sync()` decides the message boundaries (I5).

### 4.5 The transports

| | UDP | TCP | WebSocket |
|---|---|---|---|
| Listening | 6000 / 6001 / 6002 (`server::port` / `coach_port` / `olcoach_port`). IPv4 only | `server::tcp_port` / `tcp_coach_port` / `tcp_olcoach_port`. 0 (disabled) by default. Dual stack IPv6, or IPv4 only if IPv6 is not available | One port, `server::ws_port`. 0 (disabled) by default. The URL path (`/player` `/monitor` `/coach` `/olcoach`) selects the service. 404 for an unknown path, 400 for a request that is not an upgrade. Listens on the same addresses as TCP |
| Connection | Dedicated ports | The accepted connection is the client's own | The upgraded HTTP connection is the client's own |
| Server → client | One datagram is one message (ends with `'\0'`) | Messages joined with `'\0'` as the delimiter | One text frame is one message, without the trailing `'\0'` |
| Client → server | One datagram is one message | Delimited by `'\0'` or `'\n'`. A trailing `'\r'` is removed | One frame is one message. Trailing `'\0'`, `'\n'` and `'\r'` are removed |
| Longest message received | MaxMesg | MaxMesg. Disconnected if longer | MaxMesg (Beast's `read_message_max`). Disconnected if longer |
| Compression `(compression N)` | Not supported (section 8, #9). Replies `(warning compression_unsupported)` | Not supported. Replies `(warning compression_unsupported)` | Not supported. Replies `(warning compression_unsupported)` |
| Monitor versions | All | v3 or later, and JSON | v3 or later, and JSON |
| Dedicated port enforced | As before | Not needed | Not needed |
| Connections not accepted yet | - | Closed if not accepted within 10 seconds. At most 64 at a time | The same as TCP, including the ones in the opening handshake |

- Monitors v1 / v2 are not accepted on TCP or WS (`(error illegal_client_version)`). They send the display data as binary structures (`dispinfo_t` / `dispinfo_t2`), which can be carried neither with `'\0'` as the delimiter nor in WS text frames (`Connection::isBinarySafe()`). For the same reason, compression was not supported on TCP and WS at first. Later, it was dropped for all transports (section 8, #9).
- As over UDP, a monitor sends `dispinit` to the service for players (the TCP port for players, or `/player` on WS). The main use is expected to be a JSON monitor (dispinit with a negative version) on WS.
- On WS, the path `/monitor` is only for monitors, which is easier for the authors of web monitors. `/monitor` accepts only `dispinit`, and other messages (e.g. init and reconnect of players) get `(error unknown_command)`. The connection is kept, so the client can send `dispinit` next. `/player` also accepts monitors, as before.
  - Stadium has a list of acceptors only for monitors, `M_monitor_acceptors`, and `recvFromMonitorPort()`. Messages from an accepted monitor are received in `recvFromPlayerPort()`, as before.
- `server::ws_allowed_origins` (comma separated) limits the Origin of WS requests. If it is empty, all origins are accepted. A request without the Origin header (from a client that is not a browser) is always accepted. A request from another origin gets 403. On the command line, the value must be quoted and must not contain spaces (a limit of the rcssserver parser).
- WS is made of `WSServer`, which has the port, and `WSAcceptor`, the entrance for each service. `WSServer` keeps the connections in the opening handshake and the ones not accepted yet, for each path, and the four `WSAcceptor`s share it.
- TCP and WS share `MessageBuf`, which keeps an outgoing message until `sync()`, `PendingConnections`, which manages the connections not accepted yet, and `listen_tcp()`, which listens on the dual stack.
- Text messages sent over WS must be valid UTF-8. The server output is mostly ASCII, but if a string from a client (e.g. a coach's freeform message) contains invalid UTF-8, a browser closes the connection (1007).
- The TCP and WS ports are disabled by default, and the server listens on them only when they are enabled (section 8, #14). Each service has its own port because the service cannot be told from the first message: the init messages of players and online coaches have the same form, and an offline coach may send any command without init.

### 4.6 Phase 6: waiting for events

**Approach**: the first plan was to run the main loop with `io_context.run()` and to turn think and done of sync mode into a state machine (with AsioTimer). Instead, the loops of StandardTimer and SyncTimer and the steps of `doSendThink()` stay, and only the waits are replaced.

- Why: the change is smaller and the order of processing does not change, so differences in behavior are easier to check. The simulation runs outside the I/O handlers, so it never blocks in a handler.
- Waiting is `Timeable::waitUntil()`, which Stadium implements (`Stadium::runIO()`).
  - It waits for the time with a `steady_timer` while it runs the I/O handlers with `run_one()`. timerfd gives the precision of the time (`run_until()` is not used, because the timeout of epoll is rounded up to milliseconds).
  - It returns before the time if the stadium is disabled, e.g. by a signal.

**Normal mode (StandardTimer)**

- Ticks come every TIMEDELTA (10 ms) on a fixed schedule of `steady_clock`. The timer used to sleep for 10 ms minus the time of the tasks, so the delay in waking up made a cycle longer (100.6 ms).
- lcmt grows by TIMEDELTA for each tick. Ticks that have passed while the tasks took longer than TIMEDELTA are not caught up with: they are skipped and counted in lcmt.

**Sync mode**

- SyncTimer does not change.
- To wait for (done), `doSendThink()` repeats: receive, check, and if (done) is still missing, wait until a message arrives or the time of 1250 ms comes (`runIO( ..., true )`). The messages are not processed one by one as they arrive: after waking up, it calls `doRecvFromClients()` (the processing in the shuffled order) (I2, I3).
- The wait of `synch_micro_sleep` stays, because people use it to slow down a game in sync mode on purpose, so that they can watch it.
  - The first receive is synch_micro_sleep after (think). Each later one is at synch_micro_sleep after the previous receive or when a message arrives, whichever is later. As long as the messages arrive before the wait ends, the times are the same as before.
  - With zero or less, there is no wait. The default changed from 1 to 0: a wait of 1 µs made sense as the interval of the polling, but now it only adds a timer before each receive, which cost about a fifth of the cycles in sync mode over TCP.
  - `ServerParam::synchMicroSleep()` used to convert the value to `unsigned int`, so a negative value made the server wait about 71 minutes for each receive. It now returns `int`.
- The `sleep_for` of 50 ms before the kick off and of 20 ms with monitors are also replaced with `runIO()`, which runs the I/O while it waits.

**Arrival of UDP datagrams**

- The socket's `async_wait( wait_read )` tells when a datagram arrives. The handler only clears the waiting flag, and the socket is still read in `doRecvFromClients()`.
- The wait is started only when the socket has been read empty (would_block), and when the socket is opened. A wait started while a datagram is unread completes at once, and the event loop would wake up again and again until the datagram is read.
- When asio's epoll reactor (checked with Boost 1.83) starts a wait on an empty queue, it registers the descriptor again with `EPOLL_CTL_MOD`, so the wait completes at once if data is unread. Linux epoll also checks the state again when it takes the events, so data that has already been read is not reported. A small program confirmed these four points:
  1. The wait completes when a datagram arrives during the wait.
  2. It does not complete when the datagram is read before the reactor runs.
  3. A wait started with unread data completes at once.
  4. A wait started again in the handler with unread data completes at once again.
- The wait of UDPConnection refers to the connection with a weak_ptr and does not keep it alive. UDPAcceptor refers to `this`, as TCPAcceptor does, and its destructor closes the socket to cancel the wait.

**Signals and exit**

- SIGINT, SIGTERM and SIGHUP are received with `boost::asio::signal_set`, whose handler (which runs during the waits, like the I/O handlers) calls `finalize()`. The old `sigHandle()` called `finalize()` in the signal handler, which is not async-signal-safe.
- main creates the signal_set after `Std.init()`, because the io_context is created in init. A signal during init terminates the server by the default action.
- When `alive()` becomes false, the loop of the timer ends. Neither `io_context.stop()` nor closing the acceptors explicitly was needed.

**Measurements** (Release build, compared with phase 5, 572d6b3)

| Item | Phase 5 | Phase 6 |
|---|---|---|
| Cycle period in normal mode (20 s) | 100.607 ms (120 ms late after 20 s) | 100.000 ms |
| Standard deviation of the tick interval in normal mode | 0.125 ms | 0.047 ms |
| Cycle period in sync mode when one player never sends (done) | 1250 ms | 1250 ms |
| CPU usage of the server in that case | 3% | 0% |
| Time from SIGINT to exit while waiting for (done) in sync mode | 1.07 s | 0.00 s |
| First half (3000 cycles) in sync mode with 22 UDP players | 1.26 s | 1.12 s, no missed cycle |
| Cycle period in sync mode with `synch_micro_sleep=100000` (answer at once / 150 ms later) | 100.1 ms / 200.2 ms | 100.1 ms / 200.1 ms |
| Thinks answered by a TCP / WS client in a fixed time | about 110,000 / 60,000 | about 350,000 / 300,000 |

Sync mode over TCP and WS became faster because the server wakes up as soon as a message arrives (up to phase 5, it waited about 55 µs until the next poll). The number of thinks depends on how many tests run at the same time.

### 4.7 Where the code is, and the build

- The new code is in `src/transport/`, in the namespace `rcss`. Each file holds one class and is named after it (`connection.h`, `acceptor.h`, `udpconnection.{h,cpp}`, `udpacceptor.{h,cpp}`, `datagrambuf.{h,cpp}`, `tcpconnection.{h,cpp}`, `tcpacceptor.{h,cpp}`, `wsconnection.{h,cpp}`, `wsacceptor.{h,cpp}`, `wsserver.{h,cpp}`, `messagebuf.{h,cpp}`, `pendingconnections.{h,cpp}`, `tcplisten.{h,cpp}`, `closeonexec.{h,cpp}`).
  - They are parts of the server, so they are not under `rcss/`, which holds independent libraries. If rcssclient needs TCP or WS, they will be moved to a separate library once the interfaces are stable.
  - rcssclient shares `DatagramBuf` for sending (`setDest()` changes the destination to the dedicated port).
- rcssserver and rcssclient do not use rcss::net (bf327da). rcss/net is still built and installed. Whether to remove it is #15 in section 8.
- asio does not set close-on-exec on sockets. Without it, the programs started by the server (`server::team_l_start` and others) inherit the sockets and keep the ports in use after the server exits. rcss::net set the flag, so phase 5 added `set_close_on_exec()` and sets it on all UDP, TCP and WS sockets (listening and accepted).
- The minimum Boost version is raised from 1.44 to 1.70. `io_context` needs 1.66, and Beast's `tcp_stream` needs 1.70. 1.70 also builds on Ubuntu 20.04, which has 1.71. asio and Beast are header only.
- asio uses thread functions, so `Threads::Threads` (pthread with autotools) is linked.
- autotools and CMake have the same changes. For each commit, both builds and `make distcheck` were checked.

## 5. Migration plan

At the end of each phase, the regression checks of section 6 were run.

| Phase | Work | Done when | Result |
|---|---|---|---|
| 1 | Add Connection (without asio). Wrap the existing `UDPSocket` and `SocketStreamBuf` in `UDPConnection`, and let `RemoteClient` send and receive through the Connection | The behavior does not change | Done (d75f6f8) |
| 2 | Add Acceptor. Move the listening sockets, finding the sender by address and `sendTo*()` to `UDPAcceptor`, and make Stadium follow 4.3. This also fixes the out-of-bounds writes | The behavior does not change | Done (110bc2a) |
| 3 | Add asio and TCP. Create an `io_context`, `poll()` it from `doRecvFromClients()`, and add `TCPAcceptor` / `TCPConnection` | A monitor connected over TCP receives the data of a whole game. The UDP regression checks pass | Done (c790844). A TCP monitor received the show of every cycle of a game in sync mode (6000 cycles) |
| 4 | Add WebSocket. Add `WSServer` / `WSAcceptor` / `WSConnection` with Beast | A simple test page in a browser connects as a JSON monitor and receives the data of a whole game | Done (5ae4acc). A headless browser received the show of every cycle |
| 5 | Move UDP to asio. Replace the inside of `UDPAcceptor` / `UDPConnection` with asio (synchronous non-blocking receive, without AsioUDPSocket; section 7) | The behavior does not change | Done (572d6b3). The wait for sending was fixed in 8537a9a |
| 6 | Wait for events (4.6) | The cycle period in normal mode matches real time. A game in sync mode runs to the end | Done (79cb8f3) |

Phases 1 and 2 are pure refactoring without new transports, so problems were easy to locate. UDP moved to asio last, because asio brings little to UDP, and the proven path could be kept until the end.

## 6. Regression checks

What to check:

- Players: init (v3 and typical versions of v8 or later, with goalie), reconnect, bye, error replies (`no_more_team`, `illegal_teamname` and others)
- Dedicated ports: a client of v8 or later that sends anything but init to port 6000 gets an error. For v7 or earlier, the message is processed
- init of online coaches. init of offline coaches, and commands without init. The error when coach mode is disabled
- Monitors: dispinit (v1, v3 or later, JSON), the error when there are more than `max_monitors`, commands from monitors (`dispstart` and others)
- `(compression N)` gets `(warning compression_unsupported)` for players, coaches, online coaches and monitors, and the messages after it are not compressed (after #9)
- A game in normal mode: a cycle stays at 100 ms of real time
- A game in sync mode: it runs to the end without "Someone missed a cycle"
- When a client process is killed, it is treated as disconnected
- Ctrl-C ends the server, and the logs (rcg / rcl) are closed properly
- The programs started by the server (`team_l_start` / `team_r_start`) do not inherit its sockets
- TCP / WS: delimiters, the longest message, the timeout of connections not accepted, disconnecting a client that does not read, and the WS opening handshake (404 / 400 / 403)

How they were checked:

- For UDP, TCP and WS, test clients in Python talked to the server, recorded the results in JSON, and the results were compared with the build before the change. Leaving out the items that change from run to run, 48 items for UDP, 27 for TCP and 38 for WS are compared. In the end, only one item differs, because phase 6 removed the wait at exit (`sync.missed_cycles`).
- Also checked: a game in a browser (headless Chrome), the cycle period and the CPU usage (4.6), the file descriptors inherited by started programs, and rcssclient before and after its changes.
- The checks were also run with a build with ASan and UBSan. The only report is in `object.h:594` (`M_collided`), which existed before the migration.
- These test scripts are not in the repository (they were kept in `test/` of the working tree).

Comparing game logs byte by byte is hard, for two reasons:

- The random seed of the simulator cannot be set with a parameter (the registration of `random_seed` is commented out in `serverparam.cpp`).
- Shuffling the clients uses the shared random numbers, and how many times depends on real time (so does the waiting loop of sync mode).

## 7. The earlier drafts

Neither draft was used, and both were deleted (neither was tracked by git).

**AsioUDPSocket** (`rcss/net/asiosocket.{h,cpp}`): not used in phase 5. If datagrams were received asynchronously into a queue, they would be read at different times (a datagram that arrives during processing would wait for the next tick), and received data would pile up in a queue in user space. Keeping the behavior the same came first, so the sockets are read synchronously without blocking. The draft also had these problems:

- A buffer passed to `async_send_to` must stay alive until the operation completes. On Linux (epoll) it is usually sent at once, so the problem shows up only when the send buffer is full, and then the data is corrupted.
- `handleReceive()` does not start receiving again after an error, so one error stops the socket from listening.
- Each receive converts the address to a string and goes through `gethostbyname`.
- Received data is not terminated by `'\0'`.
- The lambdas capture `this`.

**AsioTimer** (`src/asiotimer.{h,cpp}`): not used in phase 6, because the main loop does not run with `run()`; only the waits of the existing timers were replaced (4.6). The draft also had these problems:

- It updates `M_last_update_time` before waiting, so the wait alternates between "10 ms minus the processing time" and 0. The same logic, measured on its own, gives 150 to 190 ticks per second (100 is right), and the simulation runs 1.5 to 1.9 times faster than real time.
- With a pending receive, `io_context.run()` does not return, so Ctrl-C does not end the server.
- `processServerTasks()` keeps its state in `static` variables.

## 8. Open issues

| # | Issue | Status |
|---|---|---|
| 1 | Whether TCP also takes `'\n'` as a delimiter | **Decided**: yes. A trailing `'\r'` is also removed. The server always sends with `'\0'` as the delimiter |
| 2 | Compression on TCP / WS | **Decided**: not supported. The server replies `(warning compression_unsupported)`, as a server built without zlib does. Later, UDP also dropped it (#9) |
| 3 | TCP port numbers and parameter names | **Decided**: `server::tcp_port` / `tcp_coach_port` / `tcp_olcoach_port`. 0 (disabled) by default. Registered with version 999, so they are not in the `(server_param ...)` message sent to clients |
| 4 | WS port number, Origin check, bind address | **Decided**: `server::ws_port` (disabled with 0 by default). The Origin is checked against the list in `server::ws_allowed_origins`; an empty list accepts all. Listens on all interfaces (the same dual stack as TCP) |
| 5 | Limit of the send queue of TCP / WS | **Decided**: disconnect and log when it holds more than 1 MiB (both TCP and WS) |
| 6 | Connections of TCP / WS not accepted yet | **Decided**: closed if not accepted within 10 seconds. At most 64 at a time (on WS, including the ones in the opening handshake) |
| 7 | IPv6 | **Decided**: TCP and WS listen on the dual stack IPv6, or IPv4 only if IPv6 is not available. UDP stays IPv4 only for now |
| 8 | Moving `rcssclient` | **Done**: out of scope at first, but moved so that rcss::net can be removed. It still uses UDP. Its socket is an asio socket and it sends with `DatagramBuf` of the server (bf327da), and its `select()` loop is replaced with asio event handling (bad983d). The standard input is read with `posix::stream_descriptor`. asio sets O_NONBLOCK on the descriptor it reads and never clears it, so a terminal is opened again with `ttyname()` (a terminal shares its open file with the standard output and the shell). At the end of the standard input, the client keeps receiving (it used to use a whole CPU). A datagram of 8192 bytes no longer loses its last byte. An unknown server name used to send to 0.0.0.0 (the local host); now the client reports the error and exits |
| 9 | Sending to clients that enabled compression `(compression N)` | **Decided**: also in 19.0.0, sending to a player with compression fails at kick off (the stream gets the error state), and the client is disconnected. Instead of investigating the cause, compression is no longer supported from the next major version.<br>Why: nothing shows that it has been used (no reports of this bug or of the crash with compression fixed in phase 1; librcsc supports compression but disables it by default; rcssmonitor-web treats it as unsupported). On today's LANs, saving bandwidth means almost nothing.<br>Compatibility: `(compression N)` gets `(warning compression_unsupported)` for players, coaches, online coaches and monitors, as a server built without zlib replies. The manual already describes this reply, and librcsc switches to compression only when it receives `(ok compression N)`, so teams that set compression keep working.<br>The server's `RemoteClient` no longer compresses or decompresses messages, and `compress.h` is deleted. rcssclient no longer handles compression and does not link with zlib or librcssgz. zlib and `rcss/gzip` stay for compressing log files (`game_log_compression` / `text_log_compression`).<br>NEWS must mention it at the release. The update of the manual (another repository) is #16 |
| 10 | Half time in sync mode | **Probably resolved**: also in 19.0.0, without a monitor, "Someone missed a cycle" appeared once at the end of the first half (cycle 3000). In the measurements of phase 6, it did not appear when the players kept sending (done) after the first half. The one seen before came after the test stopped sending (done) and sent SIGINT, while the server waited 1250 ms for (done) (it comes after "Server Killed"). Since phase 6, the server stops waiting at once, so it does not appear |
| 11 | Crash at exit on SIGINT | **Addressed**: once during the checks of phase 4, SIGINT with a WS client connected ended in SIGSEGV (it did not happen again, and ASan found nothing). The most likely cause was calling `Stadium::finalize()` in the signal handler. Since phase 6, signals go through `signal_set`, and `finalize()` is called in the event loop |
| 12 | close-on-exec of asio sockets | **Resolved** (phase 5): `set_close_on_exec()` sets it on all sockets |
| 13 | Waiting while the send buffer of a UDP socket is full | **Resolved** (8537a9a): `DatagramBuf` of phase 5 tried to wait with `basic_socket::wait()`, which fails at once on a non-blocking socket, so the client was disconnected. It now waits with `poll()`. UDP over loopback never fills the send buffer, so this was checked with an AF_UNIX datagram socket whose other end did not read |
| 14 | Default TCP port numbers | **Under discussion**: 0 (disabled) now. Another option is the same numbers as UDP (6000 to 6002) by default. The server fails to start if it cannot open a TCP port. So with TCP enabled by default, running several servers on one host needs the TCP ports to be set or disabled too, and the server does not start where X11 listens on TCP (6000 to 6063). The port for offline coaches would also be open by default. A default value meaning "the same as the UDP port" (e.g. -1) would avoid the problem with several servers |
| 15 | Whether to remove rcss::net | **Open**: nothing in the repository uses it. To remove it, delete `rcss/net`, and remove `net` from `SUBDIRS` in `rcss/Makefile.am`, `add_subdirectory(net)` from `rcss/CMakeLists.txt`, and `rcss/net/Makefile` from `configure.ac`. A copy with these changes builds with both CMake and autotools (also `make distcheck`). librcssnet is installed with public headers, though, so programs outside the repository may use it |
| 16 | Updating the manual (another repository) | **To do**: the manual is in another repository (`source/*.rst` of rcsoccersim/manual). Fix the following before the release (line numbers as of 19e75d6 of the manual).<br>(1) The compression section of `soccermonitor.rst` (lines 426 to 440): compression is not supported, and every client gets `(warning compression_unsupported)` (#9).<br>(2) The text that assumes UDP only (`overview.rst` lines 24 to 26, `introduction.rst` lines 159 to 160, `soccermonitor.rst` line 61): clients can also connect over TCP and WebSocket (4.5).<br>(3) The new server parameters (`tcp_port` / `tcp_coach_port` / `tcp_olcoach_port` / `ws_port` / `ws_allowed_origins`), and the new default of `synch_micro_sleep` (1 → 0; zero or less means no wait). The manual has no list of the server parameters (`synch_micro_sleep` appears only in an output example in `gettingstarted.rst`), so first decide where to describe them |
