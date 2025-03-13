# MacroDeck Configuration

## Configuration File
The configuration file for MacroDeck must be located at:
```
~/.config/macrodeck/config.json
```

## File Format
The `config.json` file must follow this format:
```json
{
  "size": "<rows>x<cols>",
  "rotation": "horizontal/vertical",
  "buttons": [
    {
      "macro": "<macro name>",
      "text": "<button text>",
      "bg": "<button background hex>",
      "fg": "<button foreground hex>",
      "radius": "<button corner radius in % or px>",
      "active": "<button active background hex>"
    }
  ]
}
```

### Fields Description

**Root Fields***
- `size` (required): Defines the grid layout for the MacroDeck in the format `rows x cols`.
- `rotation` (optional): Specifies whether the grid should be optimized for `horizontal` or `vertical` layout. If not specified, scaling issues may occur.
- `buttons` (required): A list of button configurations. Only buttons that fit within the grid will be displayed.

**Button Object Fields**
- `macro` (required): The name of the macro assigned to this button.
- `text` (optional): The text displayed on the button.
- `bg` (optional): The background color of the button in hex format (e.g., `#ff0000`).
- `fg` (optional): The foreground (text) color of the button in hex format.
- `radius` (optional): The corner radius of the button. Can be specified in pixels (`px`) or percentage (`%`).
- `active` (optional): The background color of the button when active (pressed or selected).


## Grid Behavior
- The grid size determines how many buttons can be displayed at once.
- If more buttons are defined than can fit in the grid, only the ones that fit will be shown.
- It is recommended to set the `rotation` field to match the intended screen orientation to avoid scaling issues.

## Example Configuration
```json
{
  "size": "3x4",
  "rotation": "horizontal",
  "buttons": [
    {
      "macro": "open_browser",
      "text": "Browser",
      "bg": "#3498db",
      "fg": "#ffffff",
      "radius": "10px",
      "active": "#2980b9"
    },
    {
      "macro": "open_terminal",
      "text": "Terminal",
      "bg": "#2ecc71",
      "fg": "#ffffff",
      "radius": "5%",
      "active": "#27ae60"
    }
  ]
}
```

This configuration defines a 3-row by 4-column MacroDeck layout, optimized for horizontal orientation, with two buttons configured.
