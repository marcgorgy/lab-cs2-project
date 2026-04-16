# QtChat – GUI Implementation

## Screens (satisfies the ≥ 3 distinct screens requirement)

| Screen | File | Purpose |
|--------|------|---------|
| **LoginScreen** | `LoginScreen.h/.cpp` | Username + host + port entry, inline validation errors, connecting progress bar |
| **ChatRoomScreen** | `ChatRoomScreen.h/.cpp` | Message display, input bar, online user list, network status indicator |
| **SettingsScreen** | `SettingsScreen.h/.cpp` | Edit connection parameters and preferences, save/cancel |

## Architecture – No Business Logic in GUI Classes

All three screen classes are **pure view classes**:
- They only own Qt widgets and layout code.
- They expose a clean **signal/slot API** for the outside world:
  - **Signals** tell the controller what the user *wants* (e.g. `loginRequested`, `messageSendRequested`, `settingsSaved`).
  - **Public slots/methods** let the controller push feedback *into* the view (e.g. `setConnecting()`, `setErrorMessage()`, `appendMessage()`, `setNetworkStatus()`).
- `MainWindow` wires the screens together and routes navigation.
- Your `NetworkManager` (Boost.Asio) should connect to the screen signals and call the feedback API.

## Network feedback API quick reference

### LoginScreen
```cpp
screen->setConnecting(true);         // shows spinner, disables button
screen->setConnecting(false);        // re-enables button
screen->setErrorMessage("Refused");  // shows red error box
screen->reset();                     // clears everything
```

### ChatRoomScreen
```cpp
screen->setConnected(true/false);             // enables/disables input, updates dot
screen->setNetworkStatus("Connected", false); // custom status text
screen->appendMessage("alice", "hi", false);  // other user's message
screen->appendMessage("me", "hey", true);     // own message (red name)
screen->appendSystemMessage("alice joined");  // grey italic
screen->addUser("alice");
screen->removeUser("alice");
```

### SettingsScreen
```cpp
screen->populate(username, host, port);  // pre-fill before showing
```

## Build

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./QtChatApp
```

Requires Qt 5.15+ or Qt 6.x with Widgets module.
