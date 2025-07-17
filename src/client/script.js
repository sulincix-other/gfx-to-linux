// Apple garbage blocker
function isApple() {
    return /iPhone/i.test(navigator.userAgent) 
    || /Macintosh/i.test(navigator.userAgent) 
    || /Mac OS X/i.test(navigator.userAgent) 
    || /iPad/i.test(navigator.userAgent);
}

if(isApple()){
    document.body.innerHTML = "<h1 style='color:white;'>Apple Devices Are Not Allowed!</h1>"
    document.body.innerHTML += "<p style='color:white;'>Get a real device: use Android or GNU/Linux!</p>";
    throw new Error("Apple Detected!");
}
document.body.oncontextmenu = ()=>{return false;};

const socket = new WebSocket('ws://' + window.location.hostname + ":8080");
let keyboardLayout = {};
var auth = false;
fetch('./keycodes.json')
    .then(response => response.json())
    .then(data => {
        keyboardLayout = data;
        createKeyboard('keyboard', keyboardLayoutLabels);
        createKeyboard('presentation', presentationLayoutLabels);
    });

socket.onmessage = function(event) {
    console.log('Message from server: ', event.data);
    if(event.data == "AUTH:OK"){
        showPage('keyboard');
        document.getElementById('pinForm').style.display = 'none';
        document.getElementById('tabs').style.display = 'block';
        auth=true;
    }
};

//document.getElementById('pinForm').style.display = 'none';
//document.getElementById('tabs').style.display = 'block';

function sendWebSocketMessage(type, code, value) {
    if(!auth){
        return;
    }
    const message = `type:\t${type}\ncode:\t${code}\nvalue:\t${value}\n\0`;
    socket.send(message);
}

function sendPin(){
    const pin = document.getElementById("pinInput").value;
    socket.send(`pin:\t${pin}\n\0`);
}

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

const touchpad_area = document.getElementById('touchpad_area');
touchpad_area.addEventListener('touchstart', on_touchpad_start);
touchpad_area.addEventListener('touchend', on_touchpad_end);
touchpad_area.addEventListener('touchmove', on_touchpad_move);
touchpad_area.addEventListener('mousedown', on_touchpad_start);
touchpad_area.addEventListener('mouseup', on_touchpad_end);
touchpad_area.addEventListener('mousemove', on_touchpad_move);

addEventListener("resize", (event) => {

})

// Handle key press events
function on_key_press(code) {
    console.log(code);
    sendWebSocketMessage("EV_KEY", code, "1");
}

// Handle key release events
function on_key_release(code) {
    console.log(code);
    sendWebSocketMessage("EV_KEY", code, "0");
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function on_key_send(code) {
    on_key_press(code);
    sleep(100);
    on_key_release(code);
}

function on_press(e) {
    console.log(e.key);
    e.preventDefault();
    var key = "BTN_LEFT";
    if (e.touches) {
        button = 1;
    } else {
        button = e.buttons;
        if (e.buttons == 2) {
            key = "BTN_RIGHT";
        }
    }
    sendWebSocketMessage("EV_KEY", key, "1");
}

function on_release(e) {
    e.preventDefault();
    var key = "BTN_LEFT";
    if (button == 2) {
        key = "BTN_RIGHT";
    } else if (button == 330) {
        key = "BTN_TOUCH";
    }
    button = 0;
    sendWebSocketMessage("EV_KEY", key, "0");
}

var button = 0;
// new position from mouse/touch event
function on_move(e) {
    e.preventDefault();
    get_position(e);
    var rect = tablet.getBoundingClientRect();
    pos.x = (pos.x * 3920) / rect.width;
    pos.y = (pos.y * 2160) / rect.height;
    sendWebSocketMessage("EV_ABS", "ABS_X", pos.x);
    sendWebSocketMessage("EV_ABS", "ABS_Y", pos.y);
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
var pos = { x: 0, y: 0 };
var pressed = false;
var moved = false;
var touch_count = 0;
function on_touchpad_start(e) {
    e.preventDefault();
    if(e.touches){
        touch_count =  e.touches.length;
    } else {
        touch_count = 1;
    }
    console.log(e.touches);
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
        if (touch_count == 1) {
            on_key_send("BTN_LEFT");
        } else if (touch_count == 2) {
            on_key_send("BTN_RIGHT");
        }
    }
    touch_count = 0;
    pressed = false;
}

function on_touchpad_move(e) {
    e.preventDefault();
    if(!pressed){
        return;
    }
    if (touch_count != 1) {
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
    sendWebSocketMessage("EV_ABS", "ABS_X", lastPos.x);
    sendWebSocketMessage("EV_ABS", "ABS_Y", lastPos.y);

}

function hideAll(){
    const pages = ['keyboard', 'tablet', 'touchpad', 'presentation'];
    pages.forEach(page => {
        console.log(page);
        document.getElementById(page).style.display = 'none';
        document.getElementById(page+"Key").classList.remove('menuKeyActive');
    });
}
function showPage(page){
    hideAll();
    document.getElementById(page).style.display = 'block';
    document.getElementById(page+"Key").classList.add('menuKeyActive');
    content = document.getElementById(page);
}

function showFullScreen() {

    if (content.requestFullscreen) {
        content.requestFullscreen();
    } else if (content.mozRequestFullScreen) { // Firefox
        content.mozRequestFullScreen();
    } else if (content.webkitRequestFullscreen) { // Chrome, Safari, and Opera
        content.webkitRequestFullscreen();
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

const presentationLayoutLabels = [
    ['VolDn', 'Mute', 'VolUp', 'PgUp'],
    ['Prev', 'Play', 'Next', 'PgDn'],
    ['Esc', 'Up', 'PrtSc', 'Enter'],
    ['Left', 'Down', 'Right', 'F5']
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
            rowDiv.style.height = (100 / labels.length) + "%";

        });

        container.appendChild(rowDiv);
    });
}
