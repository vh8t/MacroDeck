# Macro Configuration

## Macro Files Location
All macros are stored in:
```
~/.config/macrodeck/macros
```
Each macro is a JSON file with the following naming convention:
```
<macro name>.json
```

## File Format
Each macro file must follow this format:
```json
{
  "macro": [
    ["<action name>", "<arg1>", "<arg2>", ...]
  ],
  "author": "<author name>",
  "version": "<macro version>",
  "description": "<macro description>"
}
```

### Field Description

**Root Fields**
- `macro` (required): A list of actions that define the macro.
- `author` (optional): The name of the macro's creator.
- `version` (optional): The version of the macro.
- `description` (optional): A brief description of what the macro does.

**Action Format**
Each action in the `macro` list follows this format:
```json
["<action name>", "<arg1>", "<arg2>", ...]
```

- The first element must always be the **action name**.
- Actions may have **zero or more arguments**.
- Arguments can only be **string** or **integers**

## Example Macro File
```json
{
  "macro": [
    ["app_open", "firefox"],
    ["wait", 2000],
    ["key_click", "Ctrl+t"],
    ["key_type", "https://example.com"],
    ["key_click", "Return"]
  ],
  "author": "John Doe",
  "version": "1.0",
  "description": "Opens Firefox and navigates to example.com",
}
```

## Notes
- A macro can contain **any number of actions**.
- If an action does not require arguments, it must still be listed as a list with only the action name.
- Including metadata like `author`, `version` and `description` is a good practice to improve maintainabiliy.

This structure ensures flexibility while keeping macros organized and easy to manage.
