const socket = new WebSocket('ws://' + window.location.hostname + ":8080");
let keyboardLayout = {};
fetch('./keycodes.json')
    .then(response => response.json())
    .then(data => {
        keyboardLayout = data;
    });

function getValueFromLabel(label) {
    // Iterate through the keyboard layout array
    for (const keyMapping of keyboardLayout.keyboardLayout) {
        // Check if the key matches the provided label
        if (keyMapping.key === label) {
            // Return the associated value
            return keyMapping.value;
        }
    }
    // Return null if the label is not found
    return null;
}

function getValueFromCode(code) {
    // Iterate through the keyboard layout array
    for (const keyMapping of keyboardLayout.keyboardLayout) {
        // Check if the code matches the provided key code
        if (keyMapping.code === code) {
            // Return the associated value
            return keyMapping.value;
        }
    }
    // Return null if the code is not found
    return null;
}

// tablet functionality
const tablet = document.getElementById('tablet');
tablet.addEventListener('mousemove', on_move);
tablet.addEventListener('mousedown', on_press);
tablet.addEventListener('mouseup', on_release);
tablet.addEventListener('mouseenter', on_move);

// Touch event listeners
document.addEventListener('keydown', (e) => on_key_press(getValueFromCode(e.code)));
document.addEventListener('keyup', (e) => on_key_release(getValueFromCode(e.code)));
tablet.addEventListener('touchstart', on_press);
tablet.addEventListener('touchmove', on_move);
tablet.addEventListener('touchend', on_release);

// Handle key press events
function on_key_press(code) {
    console.log(code);
    var tbody = "type:\tEV_KEY\n";
    tbody += "code:\t" + code + "\n"; // Use the key code
    tbody += "value:\t1\n\0"; // Key down
    socket.send(tbody);
}

// Handle key release events
function on_key_release(code) {
    var tbody = "type:\tEV_KEY\n";
    tbody += "code:\t" + code + "\n"; // Use the key code
    tbody += "value:\t0\n\0"; // Key up
    socket.send(tbody);
}

function on_press(e) {
    console.log(e.key);
    e.preventDefault();
    var tbody = "type:\tEV_KEY\n";
    if (e.touches) {
        tbody += "code:\tBTN_TOUCH\n";
        button = 330;
    } else {
        button = e.buttons;
        if (e.buttons == 1) {
            tbody += "code:\tBTN_LEFT\n";
        } else if (e.buttons == 2) {
            tbody += "code:\tBTN_RIGHT\n";
        }
    }
    tbody += "value:\t1\n\0";
    socket.send(tbody)
}

function on_release(e) {
    e.preventDefault();
    var tbody = "type:\tEV_KEY\n";
    if (button == 1) {
        tbody += "code:\tBTN_LEFT\n";
    } else if (button == 2) {
        tbody += "code:\tBTN_RIGHT\n";
    } else if (button == 330) {
        tbody += "code:\tBTN_TOUCH\n";
    }
    button = 0;
    tbody += "value:\t0\n\0";
    socket.send(tbody)
}

var pos = {
    x: 0,
    y: 0
};
var button = 0;
// new position from mouse/touch event
function on_move(e) {
    e.preventDefault();
    var rect = tablet.getBoundingClientRect();
    if (e.touches) {
        pos.x = e.touches[0].clientX - rect.left;
        pos.y = e.touches[0].clientY - rect.top;
    } else {
        pos.x = e.clientX - rect.left;
        pos.y = e.clientY - rect.top;
    }
    pos.x = (pos.x * 3920) / rect.width;
    pos.y = (pos.y * 2160) / rect.height;
    var tbody = "type:\tEV_ABS\n";
    tbody += "code:\tABS_X\n";
    tbody += "value:\t" + pos.x + "\n\0";
    socket.send(tbody);
    tbody = "type:\tEV_ABS\n";
    tbody += "code:\tABS_Y\n";
    tbody += "value:\t" + pos.y + "\n\0";
    socket.send(tbody);
}

document.getElementById('keyboard').style.display = 'none';
let content = document.getElementById('tablet');

function showKeyboard() {
    document.getElementById('keyboard').style.display = 'block';
    document.getElementById('tablet').style.display = 'none';
    content = document.getElementById('keyboard');
}

function showGfxTablet() {
    document.getElementById('keyboard').style.display = 'none';
    document.getElementById('tablet').style.display = 'block';
    content = document.getElementById('tablet');
}

function showFullScreen() {

    if (content.requestFullscreen) {
        content.requestFullscreen();
    } else if (content.mozRequestFullScreen) { // Firefox
        content.mozRequestFullScreen();
    } else if (content.webkitRequestFullscreen) { // Chrome, Safari, and Opera
        content.webkitRequestFullscreen();
    } else if (content.msRequestFullscreen) { // IE/Edge
        content.msRequestFullscreen();
    }
}


// Define the keyboard layout
const keyboardLayoutLabels = [
    ['Esc', '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 'Backspace'],
    ['Tab', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\', '\''],
    ['Caps Lock', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', 'Enter'],
    ['Shift', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 'Up'],
    ['Ctrl', 'Alt', 'Space', 'Altgr', 'Ctrl', 'Left', 'Down', 'Right']
];


// Function to create the keyboard
function createKeyboard() {
    const keyboardContainer = document.getElementById('keyboard');

    keyboardLayoutLabels.forEach(row => {
        const rowDiv = document.createElement('div');
        rowDiv.className = 'row';

        row.forEach(key => {
            const keyButton = document.createElement('button');
            keyButton.className = 'key';
            keyButton.textContent = key;

            // Add mouse event listeners
            keyButton.onmousedown = (e) => on_key_press(getValueFromLabel(key));
            keyButton.onmouseup = (e) => on_key_release(getValueFromLabel(key));

            rowDiv.appendChild(keyButton);
        });

        keyboardContainer.appendChild(rowDiv);
    });
}

// Call the function to create the keyboard
createKeyboard();
