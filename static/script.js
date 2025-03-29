const host = window.location.host;
const socket = new WebSocket(`ws://${host}/ws`);

var authenticated = false;

var original_config = null;
var current_confg = null;

const cooldowns = {};

socket.addEventListener("message", (event) => {
  const message = event.data;

  if (message === "auth-required") {
    authenticate();
  } else if (message === "auth-not-required" || message === "auth-success") {
    authenticate = true;
    safeSend("get-config");
  } else if (message.startsWith("config:")) {
    try {
      original_config = JSON.parse(message.slice(7));

      if (Array.isArray(original_config)) {
        pickConfig();
      } else {
        current_confg = original_config;
        createGrid();
      }
    } catch (error) {
      console.error("Failed to parse JSON:", error);
    }
  }
});

socket.addEventListener("error", (event) => {
  console.error("WebSocket error:", event);
});

socket.addEventListener("close", () => {
  console.log("WebSocket connection closed");

  authenticated = false;
  original_config = null;
  current_confg = null;
});

function safeSend(message) {
  if (typeof socket !== "undefined" && socket.readyState === WebSocket.OPEN) {
    socket.send(message);
  } else {
    console.error("Could not send message to socket");
  }
}

function removeElements() {
  let element = null;

  if ((element = document.getElementById("grid-container"))) {
    element.remove();
  }

  if ((element = document.getElementById("config-picker"))) {
    element.remove();
  }

  if ((element = document.getElementById("auth-container"))) {
    element.remove();
  }
}

function authenticate() {
  removeElements();

  const authContainer = document.createElement("div");
  authContainer.id = "auth-container";

  const input = document.createElement("input");
  input.placeholder = "Enter password";
  input.type = "password";
  input.id = "auth-input";

  const button = document.createElement("button");
  button.innerText = "Authenticate";

  button.addEventListener("click", () => {
    const password = input.value.trim();
    safeSend(`auth:${password}`);
  });

  authContainer.appendChild(input);
  authContainer.appendChild(button);
  document.body.appendChild(authContainer);
}

function pickConfig() {
  removeElements();

  const picker = document.createElement("div");
  picker.id = "config-picker";

  original_config.forEach((obj) => {
    if ("name" in obj) {
      const button = document.createElement("button");
      button.innerText = obj.name;

      button.addEventListener("click", () => {
        current_confg = obj;
        createGrid();
      });

      picker.appendChild(button);
    }
  });

  document.body.appendChild(picker);
}

function createGrid() {
  removeElements();

  if (current_confg === null) {
    return;
  }

  if (
    !("size" in current_confg) ||
    typeof current_confg.size !== "string" ||
    current_confg.size.trim() === ""
  ) {
    displayError(
      `Missing <span style="font-family: monospace;">size</span> property in <span style="font-family: monospace;">config.json</span>`,
    );
    return;
  }

  if (!/^\d+x\d+$/.test(current_confg.size)) {
    displayError(
      `Invalid <span style="font-family: monospace;">size</span> property in <span style="font-family: monospace;">config.json</span>`,
    );
    return;
  }

  if (!("buttons" in current_confg) || !Array.isArray(current_confg.buttons)) {
    displayError(
      `Missing <span style="font-family: monospace;">buttons</span> property in <span style="font-family: monospace;">config.json</span>`,
    );
    return;
  }

  if (
    "bg" in current_confg &&
    typeof current_confg.bg === "string" &&
    current_confg.bg.trim() !== ""
  ) {
    document.body.style.backgroundColor = current_confg.bg;
  }

  const container = document.createElement("div");
  container.id = "grid-container";

  const screenWidth = window.innerWidth;
  const screenHeight = window.innerHeight;
  const padding =
    screenWidth > screenHeight
      ? Math.floor(screenHeight * 0.1)
      : Math.floor(screenWidth * 0.1);

  let [rows, cols] = current_confg.size.split("x").map(Number);
  let flipped = false;

  if (
    "rotation" in current_confg &&
    typeof current_confg.rotation === "string"
  ) {
    if (current_confg.rotation === "horizontal" && screenHeight > screenWidth) {
      [cols, rows] = current_confg.size.split("x").map(Number);
      flipped = true;
    } else if (
      current_confg.rotation === "vertical" &&
      screenHeight < screenWidth
    ) {
      [cols, rows] = current_confg.size.split("x").map(Number);
      flipped = true;
    }
  }

  const squareSize =
    Math.floor(Math.min(screenWidth / cols, screenHeight / rows)) - padding * 2;

  const cellWidth = Math.floor(screenWidth / cols);
  const cellHeight = Math.floor(screenHeight / rows);

  container.style.gridTemplateColumns = `repeat(${cols}, 1fr)`;
  container.style.gridTemplateRows = `repeat(${rows}, 1fr)`;

  const loopLen = Math.min(current_confg.buttons.length, rows * cols);
  for (let i = 0; i < loopLen; i++) {
    const btn = current_confg.buttons[i];

    if (
      !("macro" in btn) ||
      typeof btn.macro !== "string" ||
      btn.macro.trim() === ""
    ) {
      displayError(
        `Missing button <span style="font-family: monospace;">macro</span> property in <span style="font-family: monospace;">config.json</span>`,
      );
      return;
    }

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
        img.style.height = "100%";
      }

      if (
        "img-radius" in btn &&
        typeof btn["img-radius"] === "string" &&
        btn["img-radius"].trim() !== ""
      ) {
        img.style.borderRadius = btn["img-radius"];
      } else {
        img.style.borderRadius = "25%";
      }

      button.appendChild(img);
    } else {
      if (
        "text" in btn &&
        typeof btn.text === "string" &&
        btn.text.trim() !== ""
      ) {
        button.innerText = btn.text;
      } else {
        button.innerText = btn.macro;
      }
    }

    let fg = "#ffffff";
    let bg = "#007bff";
    let radius = "25%";
    let active = "#0047a6";
    let size = `${squareSize}px`;

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

    if ("scale" in btn && typeof btn.scale === "number") {
      size = `${Math.round(squareSize * btn.scale)}px`;
    }

    button.style.color = fg;
    button.style.backgroundColor = bg;
    button.style.borderRadius = radius;
    button.style.width = size;
    button.style.height = size;

    button.addEventListener("mousedown", () => {
      button.style.backgroundColor = active;
    });

    button.addEventListener("mouseup", () => {
      button.style.backgroundColor = bg;
    });

    button.addEventListener("mouseout", () => {
      button.style.backgroundColor = bg;
    });

    cell.appendChild(button);
    container.appendChild(cell);

    button.addEventListener("click", handleButtonClick);
    cooldowns[`button-${i}`] = false;
  }

  document.body.appendChild(container);
}

function handleButtonClick(event) {
  event.preventDefault();

  const button = event.currentTarget;
  const buttonId = button.getAttribute("data-id");
  const macro = button.getAttribute("data-macro");

  if (cooldowns[buttonId]) {
    return;
  }

  button.disabled = true;
  cooldowns[buttonId] = true;

  safeSend(`run-macro:${macro}`);

  setTimeout(() => {
    button.disabled = false;
    cooldowns[buttonId] = false;
  }, 100);
}

function displayError(message) {
  removeElements();

  const text = document.createElement("p");
  text.classList.add("error");
  text.innerHTML = message;

  container.appendChild(text);
}
