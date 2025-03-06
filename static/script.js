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
    }
  } else if (message === "conf-err-1") {
    console.error("Could not open config.json file, make sure it exists");
  } else if (message === "conf-err-2") {
    console.error(
      "Could not parse config.json file, make sure there are not errors in it",
    );
  }
};

socket.onerror = (e) => {
  console.error("WebSocket error:", e);
};

socket.onclose = () => {
  console.log("WebSocket connection closed");
};

window.onresize = createGrid;

function createGrid() {
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

  if ("rotation" in config) {
    if (config.rotation === "horizontal" && screenHeight > screenWidth) {
      [cols, rows] = config.size.split("x").map(Number);
    } else if (config.rotation == "vertical" && screenWidth > screenHeight) {
      [cols, rows] = config.size.split("x").map(Number);
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
    const cell = document.createElement("div");
    cell.classList.add("grid-cell");

    cell.style.width = `${cellWidth}px`;
    cell.style.height = `${cellHeight}px`;

    const button = document.createElement("button");
    button.classList.add("grid-button");
    button.setAttribute("data-id", `button-${i}`);
    button.setAttribute("data-macro", config.buttons[i].macro);

    if ("text" in config.buttons[i]) {
      button.textContent = config.buttons[i].text;
    } else {
      button.textContent = config.buttons[i].macro;
    }

    button.style.width = `${squareSize}px`;
    button.style.height = `${squareSize}px`;

    let fg = "#ffffff";
    let bg = "#007bff";
    let radius = "25%";
    let active = "#0047a6";

    if ("fg" in config.buttons[i]) {
      fg = config.buttons[i].fg;
    }

    if ("bg" in config.buttons[i]) {
      bg = config.buttons[i].bg;
    }

    if ("radius" in config.buttons[i]) {
      radius = config.buttons[i].radius;
    }

    if ("active" in config.buttons[i]) {
      active = config.buttons[i].active;
    }

    button.style.color = fg;
    button.style.backgroundColor = config.buttons[i].bg;
    button.style.borderRadius = config.buttons[i].radius;
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
  const button = event.target;
  const buttonId = button.getAttribute("data-id");
  const macro = button.getAttribute("data-macro");

  if (cooldowns[buttonId]) {
    return;
  }

  button.disabled = true;
  cooldowns[buttonId] = true;

  socket.send(`run-macro:${macro}`);

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
