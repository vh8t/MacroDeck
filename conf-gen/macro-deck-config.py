#!/usr/bin/env python3

import json
from pathlib import Path

import wx


class ConfigSaveDialog(wx.Dialog):
    def __init__(self, parent):
        super().__init__(parent, title="Save Configuration", size=(165, 185))
        self.choice = None

        panel = wx.Panel(self)
        sizer = wx.BoxSizer(wx.VERTICAL)

        save_as_btn = wx.Button(panel, label="Save As New")
        save_as_btn.Bind(wx.EVT_BUTTON, lambda _: self.on_choice("save_as_new"))
        sizer.Add(save_as_btn, 0, wx.ALL | wx.CENTER, 5)

        overwrite_btn = wx.Button(panel, label="Overwrite")
        overwrite_btn.Bind(wx.EVT_BUTTON, lambda _: self.on_choice("overwrite"))
        sizer.Add(overwrite_btn, 0, wx.ALL | wx.CENTER, 5)

        append_btn = wx.Button(panel, label="Append")
        append_btn.Bind(wx.EVT_BUTTON, lambda _: self.on_choice("append"))
        sizer.Add(append_btn, 0, wx.ALL | wx.CENTER, 5)

        delete_btn = wx.Button(panel, label="Delete")
        delete_btn.Bind(wx.EVT_BUTTON, lambda _: self.on_choice("delete"))
        sizer.Add(delete_btn, 0, wx.ALL | wx.CENTER, 5)

        panel.SetSizer(sizer)

    def on_choice(self, choice):
        self.choice = choice
        self.EndModal(wx.ID_OK)


class MacroDialog(wx.Dialog):
    def __init__(self, parent):
        super().__init__(parent, title="Macro Config", size=(400, 730))
        self.panel = wx.Panel(self)
        self.settings = {}
        self.create_controls()
        self.Centre()

    def create_controls(self):
        sizer = wx.BoxSizer(wx.VERTICAL)

        macro_dir = Path.home() / ".config" / "macrodeck" / "macros"
        macros = [file.name.removesuffix(".json") for file in macro_dir.glob("*.json")]

        self.macro_input = wx.ComboBox(self.panel, choices=macros, style=wx.CB_DROPDOWN)
        self.macro_input.SetHint("Macro")
        sizer.Add(self.macro_input, 0, wx.EXPAND | wx.ALL, 10)

        self.button_text_input = wx.TextCtrl(self.panel, style=wx.TE_PROCESS_ENTER)
        self.button_text_input.SetHint("Button text")
        sizer.Add(self.button_text_input, 0, wx.EXPAND | wx.ALL, 10)

        sizer.Add(
            wx.StaticText(self.panel, label="Text color:"),
            0,
            wx.LEFT | wx.RIGHT | wx.TOP,
            10,
        )
        self.text_color_picker = wx.ColourPickerCtrl(
            self.panel, colour=wx.Colour(255, 255, 255)
        )
        sizer.Add(self.text_color_picker, 0, wx.EXPAND | wx.ALL, 10)

        sizer.Add(
            wx.StaticText(self.panel, label="Background color:"),
            0,
            wx.LEFT | wx.RIGHT | wx.TOP,
            10,
        )
        self.bg_color_picker = wx.ColourPickerCtrl(
            self.panel, colour=wx.Colour(0, 123, 255)
        )
        sizer.Add(self.bg_color_picker, 0, wx.EXPAND | wx.ALL, 10)

        sizer.Add(
            wx.StaticText(self.panel, label="Active background color:"),
            0,
            wx.LEFT | wx.RIGHT | wx.TOP,
            10,
        )
        self.active_color_picker = wx.ColourPickerCtrl(
            self.panel, colour=wx.Colour(0, 71, 166)
        )
        sizer.Add(self.active_color_picker, 0, wx.EXPAND | wx.ALL, 10)

        sizer.Add(
            wx.StaticText(self.panel, label="Button scale size:"),
            0,
            wx.LEFT | wx.RIGHT | wx.TOP,
            10,
        )
        self.scale_input = wx.SpinCtrlDouble(
            self.panel, min=0, max=2, inc=0.1, initial=1
        )
        self.scale_input.SetDigits(2)
        sizer.Add(self.scale_input, 0, wx.EXPAND | wx.ALL, 10)

        self.height_input = wx.TextCtrl(self.panel, style=wx.TE_PROCESS_ENTER)
        self.height_input.SetHint("Image height (%, px, auto)")
        sizer.Add(self.height_input, 0, wx.EXPAND | wx.ALL, 10)

        self.width_input = wx.TextCtrl(self.panel, style=wx.TE_PROCESS_ENTER)
        self.width_input.SetHint("Image width (%, px, auto)")
        sizer.Add(self.width_input, 0, wx.EXPAND | wx.ALL, 10)

        self.img_radius_input = wx.TextCtrl(self.panel, style=wx.TE_PROCESS_ENTER)
        self.img_radius_input.SetHint("Image radius, (%, px)")
        sizer.Add(self.img_radius_input, 0, wx.EXPAND | wx.ALL, 10)

        self.radius_input = wx.TextCtrl(self.panel, style=wx.TE_PROCESS_ENTER)
        self.radius_input.SetHint("Border radius (%, px)")
        sizer.Add(self.radius_input, 0, wx.EXPAND | wx.ALL, 10)

        submit_btn = wx.Button(self.panel, label="Save Macro")
        submit_btn.Bind(wx.EVT_BUTTON, self.on_submit)
        sizer.Add(submit_btn, 0, wx.ALL | wx.CENTER, 15)

        self.panel.SetSizer(sizer)

    def on_submit(self, event):
        macro = self.macro_input.GetValue()
        if macro.strip() == "":
            event.Skip()
            wx.MessageBox("macro field required", "Error", wx.OK | wx.ICON_ERROR)
            return

        self.settings = {
            "macro": macro.strip(),
            "text": self.button_text_input.GetValue().strip(),
            "bg": self.bg_color_picker.GetColour().GetAsString(wx.C2S_HTML_SYNTAX),
            "fg": self.text_color_picker.GetColour().GetAsString(wx.C2S_HTML_SYNTAX),
            "scale": self.scale_input.GetValue(),
            "img-height": self.height_input.GetValue().strip(),
            "img-width": self.width_input.GetValue().strip(),
            "img-radius": self.img_radius_input.GetValue().strip(),
            "radius": self.radius_input.GetValue().strip(),
            "active": self.active_color_picker.GetColour().GetAsString(
                wx.C2S_HTML_SYNTAX
            ),
        }

        self.EndModal(wx.ID_OK)

    def get_settings(self):
        return self.settings


class MainWindow(wx.Frame):
    def __init__(self, parent, title):
        style = wx.DEFAULT_FRAME_STYLE & ~(wx.RESIZE_BORDER | wx.MAXIMIZE_BOX)
        super(MainWindow, self).__init__(
            parent, title=title, size=(300, 370), style=style
        )

        self.panel = wx.Panel(self)
        self.create_controls()
        self.Center()
        self.Show()

        self.config = {
            "name": "",
            "size": "1x1",
            "rotation": "horizontal",
            "bg": "#ffffff",
            "buttons": [],
        }

    def create_controls(self) -> None:
        main_sizer = wx.BoxSizer(wx.VERTICAL)

        config_path = Path.home() / ".config" / "macrodeck" / "config.json"
        existing = []
        if config_path.exists():
            with open(config_path, "r") as f:
                existing = json.load(f)
                if not isinstance(existing, list):
                    existing = [existing]

        configs = []
        for conf in existing:
            if "name" in conf:
                configs.append(conf["name"])

        self.config_input = wx.ComboBox(
            self.panel, choices=configs, style=wx.CB_DROPDOWN
        )
        self.config_input.SetHint("Config name")
        main_sizer.Add(self.config_input, 0, wx.EXPAND | wx.ALL, 10)

        grid_label = wx.StaticText(self.panel, label="Select grid size (rows x cols):")
        main_sizer.Add(grid_label, 0, wx.LEFT | wx.RIGHT | wx.TOP, 10)

        grid_sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.rows_input = wx.SpinCtrl(self.panel, min=1, max=100, initial=1)
        self.cols_input = wx.SpinCtrl(self.panel, min=1, max=100, initial=1)

        grid_sizer.Add(self.rows_input, 1, wx.EXPAND | wx.RIGHT, 5)
        grid_sizer.Add(
            wx.StaticText(self.panel, label="x"),
            0,
            wx.ALIGN_CENTER | wx.LEFT | wx.RIGHT,
            5,
        )
        grid_sizer.Add(self.cols_input, 1, wx.EXPAND | wx.LEFT, 5)
        main_sizer.Add(grid_sizer, 0, wx.EXPAND | wx.ALL, 10)

        rotation_label = wx.StaticText(self.panel, label="Select rotation:")
        main_sizer.Add(rotation_label, 0, wx.LEFT | wx.RIGHT | wx.TOP, 10)

        self.rotation_choice = wx.Choice(self.panel, choices=["Horizontal", "Vertical"])
        self.rotation_choice.SetSelection(0)
        main_sizer.Add(self.rotation_choice, 0, wx.EXPAND | wx.ALL, 10)

        color_label = wx.StaticText(self.panel, label="Select background color:")
        main_sizer.Add(color_label, 0, wx.LEFT | wx.RIGHT | wx.TOP, 10)

        self.color_picker = wx.ColourPickerCtrl(
            self.panel, colour=wx.Colour(255, 255, 255), style=wx.CLRP_DEFAULT_STYLE
        )
        main_sizer.Add(self.color_picker, 0, wx.EXPAND | wx.ALL, 10)

        button_sizer = wx.BoxSizer(wx.HORIZONTAL)

        add_macro_btn = wx.Button(self.panel, label="Add Macro")
        add_macro_btn.Bind(wx.EVT_BUTTON, self.on_open_dialog)
        button_sizer.Add(add_macro_btn, 1, wx.EXPAND | wx.RIGHT, 5)

        submit_btn = wx.Button(self.panel, label="Save Configuration")
        submit_btn.Bind(wx.EVT_BUTTON, self.on_submit)
        button_sizer.Add(submit_btn, 1, wx.EXPAND | wx.LEFT, 5)

        main_sizer.Add(button_sizer, 0, wx.EXPAND | wx.ALL, 15)

        self.panel.SetSizer(main_sizer)

    def on_open_dialog(self, _):
        dialog = MacroDialog(self)
        if dialog.ShowModal() == wx.ID_OK:
            macro = dialog.get_settings()
            self.config["buttons"].append(macro)
        dialog.Destroy()

    def on_submit(self, _) -> None:
        rows = self.rows_input.GetValue()
        cols = self.cols_input.GetValue()

        self.config["name"] = self.config_input.GetValue()
        self.config["size"] = f"{rows}x{cols}"
        self.config["rotation"] = self.rotation_choice.GetStringSelection().lower()
        self.config["bg"] = self.color_picker.GetColour().GetAsString(
            wx.C2S_HTML_SYNTAX
        )

        save_dialog = ConfigSaveDialog(self)
        if save_dialog.ShowModal() == wx.ID_OK:
            self.handle_save_choice(save_dialog.choice)
        save_dialog.Destroy()

    def handle_save_choice(self, choice):
        config_path = Path.home() / ".config" / "macrodeck" / "config.json"
        config_path.parent.mkdir(parents=True, exist_ok=True)

        if choice == "overwrite":
            existing = []
            if config_path.exists():
                with open(config_path, "r") as f:
                    existing = json.load(f)
                    if not isinstance(existing, list):
                        existing = [existing]

            for i, conf in enumerate(existing):
                if "name" in conf and conf["name"] == self.config["name"]:
                    existing[i] = self.config

            with open(config_path, "w") as f:
                if len(existing) == 1:
                    json.dump(existing[0], f, indent=2)
                else:
                    json.dump(existing, f, indent=2)

        elif choice == "append":
            existing = []
            if config_path.exists():
                with open(config_path, "r") as f:
                    existing = json.load(f)
                    if not isinstance(existing, list):
                        existing = [existing]

            existing.append(self.config)
            with open(config_path, "w") as f:
                json.dump(existing, f, indent=2)

        elif choice == "save_as_new":
            with open(config_path, "w") as f:
                json.dump(self.config, f, indent=2)

        elif choice == "delete":
            existing = []
            if config_path.exists():
                with open(config_path, "r") as f:
                    existing = json.load(f)
                    if not isinstance(existing, list):
                        existing = [existing]

            delete = []
            for i, conf in enumerate(existing):
                if "name" in conf and conf["name"] == self.config["name"]:
                    delete.append(i)

            delete.sort(reverse=True)
            for i in delete:
                existing.pop(i)

            with open(config_path, "w") as f:
                if len(existing) == 1:
                    json.dump(existing[0], f, indent=2)
                else:
                    json.dump(existing, f, indent=2)


if __name__ == "__main__":
    app = wx.App(False)
    frame = MainWindow(None, "Config Builder")
    app.MainLoop()
