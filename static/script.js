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
      createGrid();
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

window.onresize = createGrid;

async function checkIcon(iconName) {
  try {
    const response = await fetch(`/icon/${iconName}`, { method: "HEAD" });

    if (response.ok) {
      return `/icon/${iconName}`;
    }
  } catch (_) {}

  return null;
}

async function createGrid() {
  if (!("size" in config)) {
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

  const container = document.getElementById("grid-container");

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

    const icon = await checkIcon(btn.macro);

    if (icon !== null) {
      const img = document.createElement("img");
      img.src = icon;
      img.style.width = "100%";
      img.style.height = "100%";
      button.appendChild(img);

      if ("radius" in btn) {
        img.style.borderRadius = btn.radius;
      } else {
        img.style.borderRadius = "25%";
      }
    } else {
      if ("text" in btn) {
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

    if ("fg" in btn) {
      fg = btn.fg;
    }

    if ("bg" in btn) {
      bg = btn.bg;
    }

    if ("radius" in btn) {
      radius = btn.radius;
    }

    if ("active" in btn) {
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
  }, 250);
}

function displayError(message) {
  const text = document.createElement("p");
  text.classList.add("error");
  text.innerHTML = message;

  const container = document.getElementById("grid-container");
  container.innerHTML = "";

  container.appendChild(text);
}
