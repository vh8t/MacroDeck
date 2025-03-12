# Macro Documentation

This document provides an overview of the macro system used in the application. Macros are stored as JSON files in the `~/.config/macrodeck/macros` directory. Each macro is represented by a JSON file, and the name of the macro is derived from the filename (without the `.json` extension).

## Macro File Structure

Each macro file must be a JSON object with a single field named `macro`. The `macro` field is a list of actions that the macro will execute. The structure of a macro file is as follows:

```json
{
  "macro": [
    ["action_name_1", "arg1", "arg2", ...],
    ["action_name_2", "arg1", "arg2", ...],
    ...
  ]
}
```

### Key Points:

- **Macro Name**: The name of the macro is the filename without the `.json` extension.
- **Actions**: The `macro` field is a list of actions. Each action is represented as a list where the first element is the action name, and the subsequent elements are the arguments for that action.
- **Arguments**: Arguments can only be strings or integers

## Example macro

Below is an example of a macro named `example_macro.json`

```json
{
    "macro": [
        ["app_open", "firefox"],
        ["wait", 2000],
        ["key_type", "Hello, World!"],
        ["key_press", "Return"]
    ]
}
```

- `app_open`: Opens the firefox browser.
- `wait`: Waits for 2000 milliseconds (2 seconds).
- `key_type`: Types the text "Hello, World!".
- `key_press`: Simulates pressing the "Return" key.

## Supported Actions

1. `app_open`
    - **Usage**: `["app_open", "<application name>", "arg1", "arg2", ...]`
    - **Args**: args that the app is launched with.
    - Opens specified application with specified flags.

2. `app_close`
    - **Usage**: `["app_close", "<application name>"]`
    - Closes all matching applications.

3. `app_switch`
    - **Usage**: `["app_switch", "<application name>"]`
    - Focuses on the matching applicatioon.

4. `key_press`
    - **Usage**: `["key_press", "<key combination>"]`
    - Presses the key combination, keys are separated by `+` (e.g., `Ctrl+c`)

5. `key_release`
    - **Usage**: `["key_release", "<key combination>"]`
    - Releases the key combination, keys are separated by `+` (e.g., `Ctrl+c`)

6. `key_click`
    - **Usage**: `["key_click", "<key combination>"]`
    - Shortly clicks the key combination, keys are separated by `+` (e.g., `Ctrl+c`)

7. `key_type`
    - **Usage**: `["key_type", "<text>"]`
    - Types the text

8. `media_play`
    - Not implemented

9. `media_pause`
    - Not implemented

10. `media_toggle`
    - Not implemented

11. `media_next`
    - Not implemented

12. `media_prev`
    - Not implemented

13. `volume_inc`
    - **Usage**: `["volume_inc", <percentage>]`
    - Increases the volume by set percentage.

14. `volume_dec`
    - **Usage**: `["volume_dec", <percentage>]`
    - Decreases the volume by set percentage.

15. `volume_set`
    - **Usage**: `["volume_set", <percentage>]`
    - Sets the volume to percentage.
    
16. `volume_mute`
    - **Usage**: `["volume_mute"]`
    - Mutes the sound output.

17. `volume_unmute`
    - **Usage**: `["volume_unmute"]`
    - Unmutes the sound output.

18. `volume_toggle`
    - **Usage**: `["volume_toggle"]`
    - Toggles the sound output.

19. `wait`
    - **Usage**: `["wait", <milliseconds>]`
    - Wait for set milliseconds
