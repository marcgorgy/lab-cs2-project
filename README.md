# QtChat – Application Architecture

## Screens (satisfies the ≥ 3 distinct screens requirement)

| Screen | File | Purpose |
|--------|------|---------|
| **LoginScreen** | `LoginScreen.h/.cpp` | Username + host + port entry, inline validation errors, connecting progress bar |
| **ChatRoomScreen** | `ChatRoomScreen.h/.cpp` | Message display, input bar, online user list, network-status indicator, group badge |
| **SettingsScreen** | `SettingsScreen.h/.cpp` | Edit connection parameters and preferences, save/cancel |

## Business Logic & State Management (The Controller)

| Class | File | Purpose |
|-------|------|---------|
| **AppLogic** | `applogic.h/.cpp` | Input validation, application state, screen-navigation routing, group-command parsing; zero UI and zero networking code |

`AppLogic` is the centralized "Brain":

- **Input Validation** – intercepts GUI actions and validates data (empty username, bad port, empty messages) _before_ forwarding to the network layer.
- **State Management** – holds runtime state: `m_isLoggedIn`, `m_myUsername`, `m_onlineUsers`, `m_currentGroup`.
- **Navigation Routing** – controls screen transitions; screens never change themselves. `AppLogic` waits for a network confirmation before telling `MainWindow` to switch screens.
- **Group Command Parsing** – parses `/join <group>` and `/leave` commands typed in the message box; no extra screen needed.

## Architecture – Strict Layer Separation

All three screen classes are **pure view classes**:
- They own only Qt widgets and layout code.
- Signals tell the controller what the user *wants* (`loginRequested`, `messageSendRequested`).
- Public methods let the controller push feedback *into* the view (`appendMessage`, `setCurrentGroup`).

`NetworkManager` handles all TCP/JSON work and communicates with `AppLogic` only through Qt signals and slots. `main.cpp` wires all three independent layers together.

## Bonus Feature: Groups

Users can create or join named sub-groups without any extra screen, using chat commands:

| Command | Effect |
|---------|--------|
| `/join <name>` | Join (or create) a group named `<name>`. Messages you send go only to group members. |
| `/leave` | Return to the global channel. |

**How it works end-to-end:**

1. The user types `/join devs` in the message box.
2. `ChatRoomScreen` emits `messageSendRequested("/join devs")`.
3. `AppLogic::onGuiMessageSendRequested` detects the `/join` prefix, sets `m_currentGroup = "devs"`, emits `requestNetworkGroupJoin("devs")` and `chatScreenSetGroup("devs")`.
4. `NetworkManager::sendGroupJoin` sends `{"type":"group_join","group":"devs","username":"…"}` to the server.
5. The server's `handle_client` processes `group_join`, adds the session to `groups["devs"]`, and broadcasts `{"type":"group_joined","group":"devs","username":"…"}` to all group members.
6. All group members' `NetworkManager` receive `group_joined` and emit `groupJoined`, which `AppLogic` picks up in `onNetworkGroupJoined` and displays as a system message.
7. Subsequent messages from that user are sent as `group_message` and delivered only to `groups["devs"]` members. The top bar in `ChatRoomScreen` shows a gold `# devs` badge.

## Network/Logic API Reference

```cpp
// AppLogic emits → NetworkManager catches
void requestNetworkConnect(const QString &username, const QString &host, int port);
void requestNetworkSendChat(const QString &text);          // global channel
void requestNetworkDisconnect();
void requestNetworkGroupJoin(const QString &group);        // join/create group
void requestNetworkGroupMessage(const QString &group, const QString &text);

// NetworkManager emits → AppLogic catches
void onNetworkConnected();
void onNetworkConnectionFailed(const QString &errorMsg);
void onNetworkDisconnected();
void onNetworkIncomingMessage(const QString &sender, const QString &text);
void onNetworkUserJoined(const QString &username);
void onNetworkUserLeft(const QString &username);
void onNetworkUserListReceived(const QStringList &users);
void onNetworkGroupMessage(const QString &group, const QString &sender, const QString &text);
void onNetworkGroupJoined(const QString &group, const QString &username);
```

## Build Instructions

```bash
# 1. Install dependencies (Ubuntu/Debian example)
sudo apt install qt6-base-dev libboost-system-dev nlohmann-json3-dev

# 2. Configure + build
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)

# 3. Run
./CombinedChatApp

# 4. Tests
ctest --output-on-failure
```

> **Note:** The `Makefile` and `cmake_install.cmake` in the root are CMake-generated artifacts. They are listed in `.gitignore` and should never be committed. Always run CMake from a dedicated `build/` subdirectory.