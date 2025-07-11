#!/usr/bin/env python3

import json
import random
from pathlib import Path

import wx

NOUNS = [
    "people",
    "history",
    "way",
    "art",
    "world",
    "information",
    "map",
    "two",
    "family",
    "government",
    "health",
    "system",
    "computer",
    "meat",
    "year",
    "thanks",
    "music",
    "person",
    "reading",
    "method",
    "data",
    "food",
    "understanding",
    "theory",
    "law",
    "bird",
    "literature",
    "problem",
    "software",
    "control",
    "knowledge",
    "power",
    "ability",
    "economics",
    "love",
    "internet",
    "television",
    "science",
    "library",
    "nature",
    "fact",
    "product",
    "idea",
    "temperature",
    "investment",
    "area",
    "society",
    "activity",
    "story",
    "industry",
    "media",
    "thing",
    "oven",
    "community",
    "definition",
    "safety",
    "quality",
    "development",
    "language",
    "management",
    "player",
    "variety",
    "video",
    "week",
    "security",
    "country",
    "exam",
    "movie",
    "organization",
    "equipment",
    "physics",
    "analysis",
    "policy",
    "series",
    "thought",
    "basis",
    "boyfriend",
    "direction",
    "strategy",
    "technology",
    "army",
    "camera",
    "freedom",
    "paper",
    "environment",
    "child",
    "instance",
    "month",
    "truth",
    "marketing",
    "university",
    "writing",
    "article",
    "department",
    "difference",
    "goal",
    "news",
    "audience",
    "fishing",
    "growth",
    "income",
    "marriage",
    "user",
    "combination",
    "failure",
    "meaning",
    "medicine",
    "philosophy",
    "teacher",
    "communication",
    "night",
    "chemistry",
    "disease",
    "disk",
    "energy",
    "nation",
    "road",
    "role",
    "soup",
    "advertising",
    "location",
    "success",
    "addition",
    "apartment",
    "education",
    "math",
    "moment",
    "painting",
    "politics",
    "attention",
    "decision",
    "event",
    "property",
    "shopping",
    "student",
    "wood",
    "competition",
    "distribution",
    "entertainment",
    "office",
    "population",
    "president",
    "unit",
    "category",
    "cigarette",
    "context",
    "introduction",
    "opportunity",
    "performance",
    "driver",
    "flight",
    "length",
    "magazine",
    "newspaper",
    "relationship",
    "teaching",
    "cell",
    "dealer",
    "finding",
    "lake",
    "member",
    "message",
    "phone",
    "scene",
    "appearance",
    "association",
    "concept",
    "customer",
    "death",
    "discussion",
    "housing",
    "inflation",
    "insurance",
    "mood",
    "woman",
    "advice",
    "blood",
    "effort",
    "expression",
    "importance",
    "opinion",
    "payment",
    "reality",
    "responsibility",
    "situation",
    "skill",
    "statement",
    "wealth",
    "application",
    "city",
    "county",
    "depth",
    "estate",
    "foundation",
    "grandmother",
    "heart",
    "perspective",
    "photo",
    "recipe",
    "studio",
]

ADJECTIVES = [
    "happy",
    "sad",
    "angry",
    "excited",
    "tired",
    "hungry",
    "thirsty",
    "bored",
    "confused",
    "scared",
    "nervous",
    "brave",
    "calm",
    "careful",
    "lazy",
    "busy",
    "friendly",
    "kind",
    "polite",
    "rude",
    "smart",
    "dumb",
    "rich",
    "poor",
    "tall",
    "short",
    "fat",
    "thin",
    "young",
    "old",
    "strong",
    "weak",
    "fast",
    "slow",
    "quiet",
    "loud",
    "clean",
    "dirty",
    "beautiful",
    "ugly",
    "hot",
    "cold",
    "warm",
    "cool",
    "dry",
    "wet",
    "heavy",
    "light",
    "hard",
    "soft",
    "easy",
    "difficult",
    "simple",
    "complicated",
    "open",
    "closed",
    "full",
    "empty",
    "new",
    "old",
    "big",
    "small",
    "large",
    "tiny",
    "wide",
    "narrow",
    "deep",
    "shallow",
    "high",
    "low",
    "near",
    "far",
    "bright",
    "dark",
    "early",
    "late",
    "safe",
    "dangerous",
    "healthy",
    "sick",
    "delicious",
    "disgusting",
    "sweet",
    "sour",
    "bitter",
    "salty",
    "spicy",
    "fresh",
    "stale",
    "interesting",
    "boring",
    "funny",
    "serious",
    "annoying",
    "pleasant",
    "unpleasant",
    "beautiful",
    "ugly",
    "cute",
    "pretty",
    "handsome",
    "elegant",
    "messy",
    "neat",
    "busy",
    "free",
    "expensive",
    "cheap",
    "valuable",
    "worthless",
    "popular",
    "famous",
    "unknown",
    "common",
    "rare",
    "unique",
    "different",
    "similar",
    "same",
    "opposite",
    "important",
    "unimportant",
    "necessary",
    "unnecessary",
    "possible",
    "impossible",
    "real",
    "fake",
    "true",
    "false",
    "correct",
    "incorrect",
    "fair",
    "unfair",
    "honest",
    "dishonest",
    "legal",
    "illegal",
    "logical",
    "illogical",
    "loyal",
    "disloyal",
    "normal",
    "abnormal",
    "regular",
    "irregular",
    "strong",
    "weak",
    "powerful",
    "powerless",
    "effective",
    "ineffective",
    "efficient",
    "inefficient",
    "busy",
    "lazy",
    "active",
    "inactive",
    "creative",
    "uncreative",
    "happy",
    "unhappy",
    "optimistic",
    "pessimistic",
    "hopeful",
    "hopeless",
    "confident",
    "shy",
    "independent",
    "dependent",
    "sensitive",
    "insensitive",
    "patient",
    "impatient",
    "gentle",
    "rough",
    "quiet",
    "noisy",
    "relaxed",
    "tense",
    "flexible",
    "inflexible",
    "generous",
    "stingy",
    "grateful",
    "ungrateful",
    "brave",
    "cowardly",
    "wise",
    "foolish",
    "reliable",
    "unreliable",
    "organized",
    "disorganized",
    "careful",
    "careless",
    "curious",
    "indifferent",
    "charming",
    "boring",
]


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
        sizer.Fit(self)
        self.Layout()

    def on_choice(self, choice):
        self.choice = choice
        self.EndModal(wx.ID_OK)


class RemoveMacroDialog(wx.Dialog):
    def __init__(self, parent, macros):
        super().__init__(parent, title="Remove Macro", size=(200, 110))
        self.macros = macros
        self.choice = None
        self.panel = wx.Panel(self)
        self.create_controls()
        self.Centre()

    def create_controls(self):
        sizer = wx.BoxSizer(wx.VERTICAL)

        self.macro_choice = wx.Choice(self.panel, choices=self.macros)
        self.macro_choice.SetSelection(0)
        sizer.Add(self.macro_choice, 0, wx.EXPAND | wx.ALL, 10)

        delete_btn = wx.Button(self.panel, label="Delete")
        delete_btn.Bind(
            wx.EVT_BUTTON,
            lambda _: self.on_choice(self.macro_choice.GetStringSelection()),
        )
        sizer.Add(delete_btn, 0, wx.ALL | wx.CENTER, 5)

        self.panel.SetSizer(sizer)
        sizer.Fit(self)
        self.Layout()

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
        sizer.Fit(self)
        self.Layout()

    def on_submit(self, event):
        macro = self.macro_input.GetValue()
        if macro.strip() == "":
            wx.MessageBox("macro field required", "Error", wx.OK | wx.ICON_ERROR)
            event.Skip()
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
            parent, title=title, size=(300, 395), style=style
        )

        self.name = (
            random.choice(ADJECTIVES).capitalize() + random.choice(NOUNS).capitalize()
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
        self.config_input.SetValue(self.name)
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

        rem_macro_btn = wx.Button(self.panel, label="Remove Macro")
        rem_macro_btn.Bind(wx.EVT_BUTTON, self.on_open_dialog_rem)
        button_sizer.Add(rem_macro_btn, 1, wx.EXPAND | wx.LEFT, 5)

        main_sizer.Add(button_sizer, 0, wx.EXPAND | wx.LEFT | wx.RIGHT, 10)

        submit_btn = wx.Button(self.panel, label="Save Configuration")
        submit_btn.Bind(wx.EVT_BUTTON, self.on_submit)
        main_sizer.Add(submit_btn, 0, wx.EXPAND | wx.ALL, 10)

        self.panel.SetSizer(main_sizer)
        main_sizer.Fit(self)
        self.Layout()

    def on_open_dialog_rem(self, event):
        if len(self.config["buttons"]) == 0:
            wx.MessageBox("No macros present", "Error", wx.OK | wx.ICON_ERROR)
            event.Skip()
            return

        dialog = RemoveMacroDialog(
            self, [button["macro"] for button in self.config["buttons"]]
        )
        if dialog.ShowModal() == wx.ID_OK:
            macro = dialog.choice
            remove = []
            for i, btn in enumerate(self.config["buttons"]):
                if btn["macro"] == macro:
                    remove.append(i)
            remove.sort(reverse=True)
            for i in remove:
                self.config["buttons"].pop(i)
        dialog.Destroy()

    def on_open_dialog(self, _):
        dialog = MacroDialog(self)
        if dialog.ShowModal() == wx.ID_OK:
            macro = dialog.get_settings()
            self.config["buttons"].append(macro)
        dialog.Destroy()

    def on_submit(self, _) -> None:
        rows = self.rows_input.GetValue()
        cols = self.cols_input.GetValue()

        name = self.config_input.GetValue().strip()
        if name == "":
            name = self.name

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
