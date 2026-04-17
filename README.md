# QtChat – Application Architecture

## Screens (satisfies the ≥ 3 distinct screens requirement)

| Screen | File | Purpose |
|--------|------|---------|
| **LoginScreen** | `LoginScreen.h/.cpp` | Username + host + port entry, inline validation errors, connecting progress bar |
| **ChatRoomScreen** | `ChatRoomScreen.h/.cpp` | Message display, input bar, online user list, network status indicator |
| **SettingsScreen** | `SettingsScreen.h/.cpp` | Edit connection parameters and preferences, save/cancel |

## Business Logic & State Management (The Controller)

| Logic Class | File | Purpose |
|-------------|------|---------|
| **AppLogic** | `AppLogic.h/.cpp` | Input validation, application state memory, screen navigation routing, and middleman between GUI and Network. |

The `AppLogic` class acts as the centralized "Brain" of the application. It strictly enforces the separation of concerns: it contains zero UI elements and zero networking protocols.
- **Input Validation:** Intercepts GUI actions and validates data (e.g., blocking empty usernames/messages) *before* forwarding to the network.
- **State Management:** Holds the application's memory during runtime (`m_isLoggedIn`, `m_myUsername`, `m_onlineUsers`).
- **Navigation Routing:** Controls the flow of the application. Screens do not change themselves; `AppLogic` waits for network confirmation before emitting signals to `MainWindow` to change the active screen.

## Architecture – Strict Layer Separation

All three screen classes are **pure view classes**:
- They only own Qt widgets and layout code.
- They expose a clean **signal/slot API** for the outside world:
  - **Signals** tell the controller what the user *wants* (e.g. `loginRequested`, `messageSendRequested`).
  - **Public slots/methods** let the controller push feedback *into* the view.
- `AppLogic` connects to the GUI's signals and tells the GUI what to display using the feedback API.
- `NetworkManager` (Boost.Asio/JSON) connects to `AppLogic` for validated requests, processes the network/JSON protocols, and sends parsed responses back to `AppLogic`.
- `main.cpp` wires all three independent layers (GUI, Logic, Network) together.

## Network/Logic API (For NetworkManager Integration)

When implementing the `NetworkManager`, it should connect to these `AppLogic` signals/slots in `main.cpp`:

```cpp
// ── AppLogic emits these (NetworkManager should catch them) ──
void requestNetworkConnect(const QString &username, const QString &host, int port);
void requestNetworkSendChat(const QString &text); // Must be packaged into JSON
void requestNetworkDisconnect();

// ── AppLogic listens for these (NetworkManager should emit them) ──
void onNetworkConnected();
void onNetworkConnectionFailed(const QString &errorMsg);
void onNetworkDisconnected();
void onNetworkIncomingMessage(const QString &sender, const QString &text); // Parsed from JSON
void onNetworkUserJoined(const QString &username);
void onNetworkUserLeft(const QString &username);
void onNetworkUserListReceived(const QStringList &users);


