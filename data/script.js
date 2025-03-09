const socket = new WebSocket('ws://' + window.location.hostname + ":8080");
let keyboardLayout = {};
fetch('./keycodes.json')
    .then(response => response.json())
    .then(data => {
        keyboardLayout = data;
        createKeyboard('keyboard', keyboardLayoutLabels);
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

touchpad.addEventListener('touchstart', on_touchpad_start);
touchpad.addEventListener('touchend', on_touchpad_end);
touchpad.addEventListener('touchmove', on_touchpad_move);
touchpad.addEventListener('mousedown', on_touchpad_start);
touchpad.addEventListener('mouseup', on_touchpad_end);
touchpad.addEventListener('mousemove', on_touchpad_move);

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
        tbody += "code:\tBTN_LEFT\n";
        button = 1;
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
    get_position(e);
    var rect = tablet.getBoundingClientRect();
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

function get_position(e){
    if (e.touches) {
        pos.x = e.touches[0].clientX;
        pos.y = e.touches[0].clientY;
    } else {
        pos.x = e.clientX;
        pos.y = e.clientY;
    }
}

var beginPos = { x: 0, y: 0 };
var lastPos = { x: 0, y: 0 };
var pressed = false;
var moved = false;
function on_touchpad_start(e) {
    e.preventDefault();
    if(pressed){
        return;
    }
    pressed = true;
    moved = false;
    get_position(e);
    // Capture initial touch position
    beginPos.x = pos.x;
    beginPos.y = pos.y;
}

function on_touchpad_end(e) {
    if(!pressed){
        return;
    }
    if(!moved){
        var tbody = "type:\tEV_KEY\n";
        tbody += "code:\tBTN_LEFT\n";
        tbody += "value:\t1\n\0";
        socket.send(tbody);

        tbody = "type:\tEV_KEY\n";
        tbody += "code:\tBTN_LEFT\n";
        tbody += "value:\t0\n\0";
        socket.send(tbody);
    }
    pressed = false;
}

function on_touchpad_move(e) {
    e.preventDefault();
    if(!pressed){
        return;
    }
    moved = true;
    var rect = tablet.getBoundingClientRect();
    get_position(e);
    var currentPos = {
        x: pos.x,
        y: pos.y
    };

    // Calculate the relative movement
    var deltaX = currentPos.x - beginPos.x;
    var deltaY = currentPos.y - beginPos.y;
    
    beginPos = currentPos;

    lastPos.x += deltaX * 5;
    lastPos.y += deltaY * 5;
    
    if (lastPos.x < -1) {lastPos.x = -1};
    if (lastPos.y < -1) {lastPos.y = -1};
    if (lastPos.x > 3941) {lastPos.x = 3941};
    if (lastPos.y > 2161) {lastPos.y = 2161};
    var tbody = "type:\tEV_ABS\n";
    tbody += "code:\tABS_X\n";
    tbody += "value:\t" + lastPos.x + "\n\0";
    socket.send(tbody);
    tbody = "type:\tEV_ABS\n";
    tbody += "code:\tABS_Y\n";
    tbody += "value:\t" + lastPos.y + "\n\0";
    socket.send(tbody);

}

function hideAll(){
    document.getElementById('keyboard').style.display = 'none';
    document.getElementById('tablet').style.display = 'none';
    document.getElementById('touchpad').style.display = 'none';
}
function showPage(page){
    hideAll();
    document.getElementById(page).style.display = 'block';
    content = document.getElementById(page);
}

function showKeyboard(){
    showPage('keyboard');
}

function showGfxTablet() {
    showPage('tablet');
}

function showTouchpad() {
    showPage('touchpad');
}

showTouchpad();

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
    ['Esc', '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 'Backspace', 'Del'],
    ['Tab', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\', '\'', 'Home'],
    ['CapsLock', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', 'Enter', 'PgUp'],
    ['Shift', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 'End', 'Up', 'PgDn'],
    ['Ctrl', 'Super',  'Alt', 'Space', 'Altgr', 'Left', 'Down', 'Right']
];

// Object to keep track of the toggle states of modifier keys
const modifierStates = {
    'Ctrl': false,
    'Alt': false,
    'Altgr': false,
    'Shift': false,
    'Super': false
};

// Function to create the keyboard
function createKeyboard(page, labels) {
    const container = document.getElementById(page);

    labels.forEach(row => {
        const rowDiv = document.createElement('div');
        rowDiv.className = 'row';

        row.forEach(key => {
            const keyButton = document.createElement('button');
            keyButton.className = 'key';
            keyButton.textContent = key;
            keyButton.classList.add(key);
            if (key.length == 1){
                keyButton.classList.add('letter');
            }

            // Add mouse event listeners
            keyButton.onmousedown = (e) => {
                if (modifierStates.hasOwnProperty(key)) {
                    // Toggle the state of the modifier key
                    if (!modifierStates[key]) {
                        // If the key is not active, send press event
                        on_key_press(getValueFromLabel(key));
                        keyButton.classList.add('active');
                    } else {
                        // If the key is already active, send release event
                        on_key_release(getValueFromLabel(key));
                        keyButton.classList.remove('active');
                    }
                    // Toggle the state
                    modifierStates[key] = !modifierStates[key];
                } else {
                    on_key_press(getValueFromLabel(key));
                }
            };

            keyButton.onmouseup = (e) => {
                if (!modifierStates.hasOwnProperty(key)) {
                    on_key_release(getValueFromLabel(key));
                }
            };

            rowDiv.appendChild(keyButton);
        });

        container.appendChild(rowDiv);
    });
}
