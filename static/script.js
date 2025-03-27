const host = window.location.host;
const socket = new WebSocket(`ws://${host}/ws`);

var config;
const cooldowns = {};

socket.onopen = () => {
  setTimeout(() => {
    socket.send("get-config");
  }, 150);
};

socket.onmessage = (e) => {
  const message = e.data;

  if (message.startsWith("config:")) {
    const raw_config = message.slice(7);

    try {
      config = JSON.parse(raw_config);
      if (Array.isArray(config)) {
        configPicker();
      } else {
        createGrid();
      }
    } catch (error) {
      console.error("Failed to parse JSON:", error);
      displayError("Failed to parse JSON:" + error.toString());
    }
  }
};

socket.onerror = (e) => {
  console.error("WebSocket error:", e);
};

socket.onclose = () => {
  console.log("WebSocket connection closed");
};

window.addEventListener("resize", () => {
  if (Array.isArray(config)) {
    configPicker();
  } else {
    createGrid();
  }
});

function configPicker() {
  let element;
  if ((element = document.getElementById("grid-container"))) {
    element.remove();
  }

  if ((element = document.getElementById("config-picker"))) {
    element.remove();
  }

  const picker = document.createElement("div");
  picker.id = "config-picker";

  config.forEach((obj) => {
    if ("name" in obj) {
      const button = document.createElement("button");
      button.textContent = obj.name;

      button.addEventListener("click", () => {
        config = obj;
        createGrid();
      });

      picker.appendChild(button);
    }
  });

  document.body.appendChild(picker);
}

function createGrid() {
  let element;
  if ((element = document.getElementById("grid-container"))) {
    element.remove();
  }

  if ((element = document.getElementById("config-picker"))) {
    element.remove();
  }

  if (
    !("size" in config) ||
    typeof config.size !== "string" ||
    config.size.trim() === ""
  ) {
    displayError(
      `Missing <span style="font-family: monospace;">size</span> property in <span style="font-family: monospace;">config.json</span>`,
    );
    return;
  }

  if (!/^\d+x\d+$/.test(config.size)) {
    displayError(
      `Invalid <span style="font-family: monospace;">size</span> property in <span style="font-family: monospace;">config.json</span>`,
    );
    return;
  }

  if (!("buttons" in config)) {
    displayError(
      `Missing <span style="font-family: monospace;">buttons</span> property in <span style="font-family: monospace;">config.json</span>`,
    );
    return;
  }

  config.buttons.forEach((button) => {
    if (!("macro" in button)) {
      displayError(
        `Missing button <span style="font-family: monospace;">macro</span> property in <span style="font-family: monospace;">config.json</span>`,
      );
      return;
    }
  });

  if (
    "bg" in config &&
    typeof config.bg === "string" &&
    config.bg.trim() !== ""
  ) {
    document.body.style.backgroundColor = config.bg;
  }

  const container = document.createElement("div");
  container.id = "grid-container";

  const screenWidth = window.innerWidth;
  const screenHeight = window.innerHeight;
  const padding =
    screenWidth > screenHeight
      ? Math.floor(screenHeight * 0.1)
      : Math.floor(screenWidth * 0.1);

  let [rows, cols] = config.size.split("x").map(Number);
  let flipped = false;

  if ("rotation" in config) {
    if (config.rotation === "horizontal" && screenHeight > screenWidth) {
      [cols, rows] = config.size.split("x").map(Number);
      flipped = true;
    } else if (config.rotation == "vertical" && screenWidth > screenHeight) {
      [cols, rows] = config.size.split("x").map(Number);
      flipped = true;
    }
  }

  const squareSize =
    Math.floor(Math.min(screenWidth / cols, screenHeight / rows)) - padding * 2;

  const cellWidth = Math.floor(screenWidth / cols);
  const cellHeight = Math.floor(screenHeight / rows);

  container.style.gridTemplateColumns = `repeat(${cols}, 1fr)`;
  container.style.gridTemplateRows = `repeat(${rows}, 1fr)`;

  container.innerHTML = "";

  const loopLen = Math.min(config.buttons.length, rows * cols);
  for (let i = 0; i < loopLen; i++) {
    const btn = config.buttons[i];

    const cell = document.createElement("div");
    cell.classList.add("grid-cell");

    cell.style.width = `${cellWidth}px`;
    cell.style.height = `${cellHeight}px`;

    const button = document.createElement("button");
    button.classList.add("grid-button");
    button.setAttribute("data-id", `button-${i}`);
    button.setAttribute("data-macro", btn.macro);

    button.style.padding = "0";

    const req = new XMLHttpRequest();
    req.open("HEAD", `/icon/${btn.macro}`, false);
    req.send();

    const icon =
      req.status >= 200 && req.status < 300 ? `/icon/${btn.macro}` : null;

    if (icon !== null) {
      const img = document.createElement("img");
      img.src = icon;

      if (
        "img-width" in btn &&
        typeof btn["img-width"] === "string" &&
        btn["img-width"].trim() !== ""
      ) {
        img.style.width = btn["img-width"];
      } else {
        img.style.width = "100%";
      }

      if (
        "img-height" in btn &&
        typeof btn["img-height"] === "string" &&
        btn["img-height"].trim() !== ""
      ) {
        img.style.height = btn["img-height"];
      } else {
        img.style.height = "auto";
      }

      button.appendChild(img);

      if (
        "img-radius" in btn &&
        typeof btn["img-radius"] === "string" &&
        btn["img-radius"].trim() !== ""
      ) {
        img.style.borderRadius = btn["img-radius"];
      }
    } else {
      if (
        "text" in btn &&
        typeof btn.text === "string" &&
        btn.text.trim() !== ""
      ) {
        button.textContent = btn.text;
      } else {
        button.textContent = btn.macro;
      }
    }

    if ("scale" in btn && typeof btn.scale === "number") {
      button.style.width = `${Math.round(squareSize * btn.scale)}px`;
      button.style.height = `${Math.round(squareSize * btn.scale)}px`;
    } else {
      button.style.width = `${squareSize}px`;
      button.style.height = `${squareSize}px`;
    }

    let fg = "#ffffff";
    let bg = "#007bff";
    let radius = "25%";
    let active = "#0047a6";

    if ("fg" in btn && typeof btn.fg === "string" && btn.fg.trim() !== "") {
      fg = btn.fg;
    }

    if ("bg" in btn && typeof btn.bg === "string" && btn.bg.trim() !== "") {
      bg = btn.bg;
    }

    if (
      "radius" in btn &&
      typeof btn.radius === "string" &&
      btn.radius.trim() !== ""
    ) {
      radius = btn.radius;
    }

    if (
      "active" in btn &&
      typeof btn.active === "string" &&
      btn.active.trim() !== ""
    ) {
      active = btn.active;
    }

    button.style.color = fg;
    button.style.backgroundColor = btn.bg;
    button.style.borderRadius = btn.radius;
    button.onmousedown = () => {
      button.style.backgroundColor = active;
    };
    button.onmouseup = () => {
      button.style.backgroundColor = bg;
    };
    button.onmouseout = () => {
      button.style.backgroundColor = bg;
    };

    cell.appendChild(button);
    container.appendChild(cell);

    button.addEventListener("click", handleButtonClick);
    cooldowns[`button-${i}`] = false;
  }

  document.body.appendChild(container);
}

function handleButtonClick(event) {
  const button = event.currentTarget;
  const buttonId = button.getAttribute("data-id");
  const macro = button.getAttribute("data-macro");

  if (cooldowns[buttonId]) {
    return;
  }

  button.disabled = true;
  cooldowns[buttonId] = true;

  socket.send(`run-macro:${macro}`);
  console.log(macro);

  setTimeout(() => {
    button.disabled = false;
    cooldowns[buttonId] = false;
  }, 100);
}

function displayError(message) {
  const text = document.createElement("p");
  text.classList.add("error");
  text.innerHTML = message;

  const container = document.getElementById("grid-container");
  container.innerHTML = "";

  container.appendChild(text);
}
