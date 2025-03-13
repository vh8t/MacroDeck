## Application Management
`app_open`
- **Usage**: `["app_open", "<application name>", "arg1", "arg2", ...]`
- **Arguments**: Additional arguments to launch the application with.
- **Description**: Opens the specified application with optional flags.

`app_close`
- **Usage**: `["app_close", "<application name>"]`
- **Description**: Closes all matching applications.

`app_switch`
- **Usage**: `["app_switch", "<application name>"]`
- **Description**: Focuses on the specified application.

## Keyboard Actions
`key_press`
- **Usage**: `["key_press", "<key combination>"]`
- **Description**: Presses the specified combination.
- **Note**: Keys are separated by `+` (e.g., `Ctrl+c`)

`key_release`
- **Usage**: `["key_release", "<key combination>"]`
- **Description**: Releases the specified combination.
- **Note**: Keys are separated by `+` (e.g., `Ctrl+c`)

`key_press`
- **Usage**: `["key_click", "<key combination>"]`
- **Description**: Simulates a short press of the specific key combination.
- **Note**: Keys are separated by `+` (e.g., `Ctrl+c`)

`key_type`
- **Usage**: `["key_type", "<text>"]`
- **Description**: Types the provided text as keyboard input.

## Media Controls *(Not Implemented Yet)*
- `media_play`
- `media_pause`
- `media_toggle`
- `media_next`
- `media_prev`

## Volume Controls
`volume_inc`
- **Usage**: `["volume_inc", <percentage>]`
- **Description**: Increase volume by the specific percentage.

`volume_dec`
- **Usage**: `["volume_dec", <percentage>]`
- **Description**: Decrease volume by the specific percentage.

`volume_set`
- **Usage**: `["volume_set", <percentage>]`
- **Description**: Sets volume to the specified percentage.

`volume_mute`
- **Usage**: `["volume_mute"]`
- **Description**: Mutes sound output.

`volume_unmute`
- **Usage**: `["volume_unmute"]`
- **Description**: Unmutes sound output.

`volume_toggle`
- **Usage**: `["volume_toggle"]`
- **Description**: Toggles sound output between mute and unmute.

## Miscellaneous
`wait`
- **Usage**: `["wait", <milliseconds>]`
- **Description**: Pause macro execution for the specified duration in milliseconds
