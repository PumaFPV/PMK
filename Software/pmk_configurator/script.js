// Map HID codes to characters
const hidToCharMap = {
    // Letters
    "0x04": "A",
    "0x05": "B",
    "0x06": "C",
    "0x07": "D",
    "0x08": "E",
    "0x09": "F",
    "0x0A": "G",
    "0x0B": "H",
    "0x0C": "I",
    "0x0D": "J",
    "0x0E": "K",
    "0x0F": "L",
    "0x10": "M",
    "0x11": "N",
    "0x12": "O",
    "0x13": "P",
    "0x14": "Q",
    "0x15": "R",
    "0x16": "S",
    "0x17": "T",
    "0x18": "U",
    "0x19": "V",
    "0x1A": "W",
    "0x1B": "X",
    "0x1C": "Y",
    "0x1D": "Z",

    // Numbers
    "0x1E": "1",
    "0x1F": "2",
    "0x20": "3",
    "0x21": "4",
    "0x22": "5",
    "0x23": "6",
    "0x24": "7",
    "0x25": "8",
    "0x26": "9",
    "0x27": "0",

    // Symbols
    "0x28": "Enter",
    "0x29": "Escape",
    "0x2A": "Backspace",
    "0x2B": "Tab",
    "0x2C": "Space",
    "0x2D": "-",
    "0x2E": "=",
    "0x2F": "[",
    "0x30": "]",
    "0x31": "\\",
    "0x33": ";",
    "0x34": "'",
    "0x35": "`",
    "0x36": ",",
    "0x37": ".",
    "0x38": "/",

    // Function Keys
    "0x39": "Caps Lock",
    "0x3A": "F1",
    "0x3B": "F2",
    "0x3C": "F3",
    "0x3D": "F4",
    "0x3E": "F5",
    "0x3F": "F6",
    "0x40": "F7",
    "0x41": "F8",
    "0x42": "F9",
    "0x43": "F10",
    "0x44": "F11",
    "0x45": "F12",

    // Modifiers
    "0xE0": "Left Ctrl",
    "0xE1": "Left Shift",
    "0xE2": "Left Alt",
    "0xE3": "Left GUI",
    "0xE4": "Right Ctrl",
    "0xE5": "Right Shift",
    "0xE6": "Right Alt",
    "0xE7": "Right GUI",

    // Navigation Keys
    "0x4A": "Print Screen",
    "0x46": "Scroll Lock",
    "0x47": "Pause",
    "0x48": "Insert",
    "0x49": "Home",
    "0x4A": "Page Up",
    "0x4B": "Delete",
    "0x4C": "End",
    "0x4D": "Page Down",
    "0x4E": "Right Arrow",
    "0x4F": "Left Arrow",
    "0x50": "Down Arrow",
    "0x51": "Up Arrow",

    // Numpad Keys
    "0x54": "Num Lock",
    "0x55": "Numpad /",
    "0x56": "Numpad *",
    "0x57": "Numpad -",
    "0x58": "Numpad +",
    "0x59": "Numpad Enter",
    "0x5A": "Numpad 1",
    "0x5B": "Numpad 2",
    "0x5C": "Numpad 3",
    "0x5D": "Numpad 4",
    "0x5E": "Numpad 5",
    "0x5F": "Numpad 6",
    "0x60": "Numpad 7",
    "0x61": "Numpad 8",
    "0x62": "Numpad 9",
    "0x63": "Numpad 0",
    "0x64": "Numpad .",

    // Special Keys
    "0x65": "Non-US \\",
    "0x66": "Application",
    "0x67": "Power",
    "0x68": "Numpad =",
    "0x69": "F13",
    "0x6A": "F14",
    "0x6B": "F15",
    "0x6C": "F16",
    "0x6D": "F17",
    "0x6E": "F18",
    "0x6F": "F19",
    "0x70": "F20",
    "0x71": "F21",
    "0x72": "F22",
    "0x73": "F23",
    "0x74": "F24",
    "0x75": "Execute",
    "0x76": "Help",
    "0x77": "Menu",
    "0x78": "Select",
    "0x79": "Stop",
    "0x7A": "Again",
    "0x7B": "Undo",
    "0x7C": "Cut",
    "0x7D": "Copy",
    "0x7E": "Paste",
    "0x7F": "Find",
    "0x80": "Mute",
    "0x81": "Volume Up",
    "0x82": "Volume Down",

    // PMK Special Keys
    "0x10CD": "Play/Pause",
    "0x10B5": "Next",
    "0x10B6": "Prev",
    "0x10B7": "Stop",
    "0x10E9": "Vol+",
    "0x10EA": "Vol-",
    
    // Application Launcher,
    "0x1183": "Config",
    "0x118A": "Email",
    "0x1192": "Calc",
    "0x1194": "Browser",
    
    // Browser/Explorer Specific,
    "0x1221": "Search",
    "0x1223": "Home",
    "0x1224": "Back",
    "0x1225": "Forward",
    "0x1226": "Stop",
    "0x1227": "Refresh",
    "0x122A": "Bookmarks",

    //From 0x2000 to 0x3000
    "0x2001": "MOUSE_LEFT",
    "0x2002": "MOUSE_RIGHT",
    "0x2004": "MOUSE_MIDDLE",
    "0x2008": "MOUSE_BACKWARD",
    "0x2010": "MOUSE_FORWARD",

    //PMK
    "0xF0":"Layer0",
    "0xF1":"Layer1",
    "0xF2":"Layer2",
    "0xF3":"Layer3",
    "0xF4":"Layer4",
    "0xF5":"Layer5",
    "0xF6":"Layer6",
    "0xF7":"Layer7",

    "0x0000": "Unassigned"
};


// Store the selected HID key mappings
const keyMappings = {};


// Handle key click events
document.querySelectorAll('.key').forEach(key => {
    key.addEventListener('click', () => {
        // const keycode = document.getElementById('keycode-select').value;
        keyMappings[key.dataset.key] = keycode;
        console.log('Selected HID code:', keycode);
        // Get the character associated with the HID code
        const char = hidToCharMap[keycode] || "Unassigned";
        key.textContent = char;  // Update the key text to show the character
    });
});


// Handle JSON generation
document.getElementById('generate-btn').addEventListener('click', () => {
    // Get the layer number from the input box
    const layerNumber = document.getElementById('layer-input').value || "0";  // Default to 0 if empty
    // Generate the layer array based on the selected keys
    const layerArray = [];
    const totalKeys = 32; // Assuming there are 32 keys in total

    for (let i = 0; i < totalKeys; i++) {
        if (keyMappings[i]) {
            layerArray.push(keyMappings[i]);
        } else {
            layerArray.push("0x0000"); // Placeholder for keys not yet assigned
        }
    }

    // Create the JSON structure
    const jsonConfig = {
        "Layer": layerArray
    };

    // Convert to JSON string and trigger download
    const jsonString = JSON.stringify(jsonConfig, null, 4); // Pretty-print with indentation
    const filename = `kb-l${layerNumber}.json`;
    downloadJson(jsonString, filename);
});


// Function to download the JSON file
function downloadJson(json, filename) {
    const blob = new Blob([json], { type: 'application/json' });
    const link = document.createElement('a');
    link.href = URL.createObjectURL(blob);
    link.download = filename;
    link.click();
}



function resizeContainer() {
    const keyboardContainer = document.getElementById('keyboard-container');

    if (!keyboardContainer) {
        console.error('keyboard-container not found');
        return;
    }

    // Get the current viewport width
    const viewportWidth = window.innerWidth;

    // Set the container width to 90% of the viewport width, but you can adjust this value
    const containerWidth = viewportWidth * 0.9;

    // Maintain a 16:9 aspect ratio (height = width * 9 / 16)
    const containerHeight = containerWidth * 9 / 16;

    // Apply the calculated width and height to the container
    keyboardContainer.style.width = `${containerWidth}px`;
    keyboardContainer.style.height = `${containerHeight}px`;

    // console.log(`Resized container to width: ${containerWidth}px, height: ${containerHeight}px`);
    const divElement = document.getElementById('keyboard-container');  // Get the div element
    const rect = divElement.getBoundingClientRect();  // Get the dimensions and position

    const divWidth = rect.width;  // Width of the div
    const divHeight = rect.height;  // Height of the div

    // console.log('Div width:', divWidth);
    // console.log('Div height:', divHeight);

    const divElementKeyboardContainer = document.getElementById('keyboard-container');

    // Set the width dynamically (for example, 400px)
    divElementKeyboardContainer.style.width = divHeight * 12/9 + 'px';
}


// Call the function when the page loads and on window resize
window.addEventListener('load', resizeContainer);
window.addEventListener('resize', resizeContainer);

// Store key mappings
let selectedHidCode = null;  // Track the selected keycode

// Render the grid of keycodes
const keycodeGrid = document.getElementById('keycode-grid');
for (const [hidCode, char] of Object.entries(hidToCharMap)) {
    const keycodeCell = document.createElement('div');
    keycodeCell.className = 'keycode-cell';
    keycodeCell.textContent = char;
    keycodeCell.dataset.keycode = hidCode;

    // Click event to select keycode
    keycodeCell.addEventListener('click', function() {
        selectedHidCode = hidCode;  // Store the selected HID code
        // alert(`Selected keycode: ${char} (HID code: ${hidCode})`);
    });

    // Append the cell to the grid
    keycodeGrid.appendChild(keycodeCell);
}


// Record a keypress from the user
const recordButton = document.getElementById('record-keypress');
const keypressDisplay = document.getElementById('keypress-display');


const keyEventToHidMap = {
    'KeyA': '0x04',
    'KeyB': '0x05',
    'KeyC': '0x06',
    'KeyD': '0x07',
    'KeyE': '0x08',
    'KeyF': '0x09',
    'KeyG': '0x0A',
    'KeyH': '0x0B',
    'KeyI': '0x0C',
    'KeyJ': '0x0D',
    'KeyK': '0x0E',
    'KeyL': '0x0F',
    'KeyM': '0x10',
    'KeyN': '0x11',
    'KeyO': '0x12',
    'KeyP': '0x13',
    'KeyQ': '0x14',
    'KeyR': '0x15',
    'KeyS': '0x16',
    'KeyT': '0x17',
    'KeyU': '0x18',
    'KeyV': '0x19',
    'KeyW': '0x1A',
    'KeyX': '0x1B',
    'KeyY': '0x1C',
    'KeyZ': '0x1D',
    'Digit1': '0x1E',
    'Digit2': '0x1F',
    'Digit3': '0x20',
    'Digit4': '0x21',
    'Digit5': '0x22',
    'Digit6': '0x23',
    'Digit7': '0x24',
    'Digit8': '0x25',
    'Digit9': '0x26',
    'Digit0': '0x27',
    'Enter': '0x28',
    'Escape': '0x29',
    'Backspace': '0x2A',
    'Tab': '0x2B',
    'Space': '0x2C',
    'Minus': '0x2D',
    'Equal': '0x2E',
    'BracketLeft': '0x2F',
    'BracketRight': '0x30',
    'Backslash': '0x31',
    'Semicolon': '0x33',
    'Quote': '0x34',
    'Backquote': '0x35',
    'Comma': '0x36',
    'Period': '0x37',
    'Slash': '0x38',
    'CapsLock': '0x39',
    'F1': '0x3A',
    'F2': '0x3B',
    'F3': '0x3C',
    'F4': '0x3D',
    'F5': '0x3E',
    'F6': '0x3F',
    'F7': '0x40',
    'F8': '0x41',
    'F9': '0x42',
    'F10': '0x43',
    'F11': '0x44',
    'F12': '0x45',
    'PrintScreen': '0x46',
    'ScrollLock': '0x47',
    'Pause': '0x48',
    'Insert': '0x49',
    'Home': '0x4A',
    'PageUp': '0x4B',
    'Delete': '0x4C',
    'End': '0x4D',
    'PageDown': '0x4E',
    'ArrowRight': '0x4F',
    'ArrowLeft': '0x50',
    'ArrowDown': '0x51',
    'ArrowUp': '0x52',
    'NumLock': '0x53',
    'NumpadDivide': '0x54',
    'NumpadMultiply': '0x55',
    'NumpadSubtract': '0x56',
    'NumpadAdd': '0x57',
    'NumpadEnter': '0x58',
    'Numpad1': '0x59',
    'Numpad2': '0x5A',
    'Numpad3': '0x5B',
    'Numpad4': '0x5C',
    'Numpad5': '0x5D',
    'Numpad6': '0x5E',
    'Numpad7': '0x5F',
    'Numpad8': '0x60',
    'Numpad9': '0x61',
    'Numpad0': '0x62',
    'NumpadDecimal': '0x63',
    'IntlBackslash': '0x64',
    'ContextMenu': '0x65',
    'Power': '0x66',
    'NumpadEqual': '0x67',
    'F13': '0x68',
    'F14': '0x69',
    'F15': '0x6A',
    'F16': '0x6B',
    'F17': '0x6C',
    'F18': '0x6D',
    'F19': '0x6E',
    'F20': '0x6F',
    'F21': '0x70',
    'F22': '0x71',
    'F23': '0x72',
    'F24': '0x73',
    'Help': '0x75',
    'Undo': '0x7A',
    'Cut': '0x7B',
    'Copy': '0x7C',
    'Paste': '0x7D',
    'Mute': '0x7F',
    'VolumeUp': '0x80',
    'VolumeDown': '0x81',
    'NumpadComma': '0x85',
    'IntlRo': '0x87',
    'KanaMode': '0x88',
    'IntlYen': '0x89',
    'MetaLeft': '0xE3',
    'MetaRight': '0xE7',
    'ControlLeft': '0xE0',
    'ControlRight': '0xE4',
    'AltLeft': '0xE2',
    'AltRight': '0xE6',
    'ShiftLeft': '0xE1',
    'ShiftRight': '0xE5',

    'Play/Pause':'0x10CD',
    'Next':'0x10B5',
    'Prev':'0x10B6',
    'Stop':'0x10B7',
    'Volume':'0x10E0',
    'Mute':'0x10E2',
    'Vol+':'0x10E9',
    'Vol-':'0x10EA',
    
    // Application Launcher
    'Config':'0x1183',
    'Email':'0x118A',
    'Calc':'0x1192',
    'Web':'0x1194',
    
    // Browser/Explorer Specific
    'Search':'0x1221',
    'Home':'0x1223',
    'Back':'0x1224',
    'Forward':'0x1225',
    'Stop':'0x1226',
    'Refresh':'0x1227',
    'Bookmarks':'0x122A',

    'MOUSE_LEFT':'0x2001',
    'MOUSE_RIGHT':'0x2002',
    'MOUSE_MIDDLE':'0x2004',
    'MOUSE_BACKWARD':'0x2008',
    'MOUSE_FORWARD':'0x2010',

    'Layer0':'0xF0',
    'Layer1':'0xF1',
    'Layer2':'0xF2',
    'Layer3':'0xF3',
    'Layer4':'0xF4',
    'Layer5':'0xF5',
    'Layer6':'0xF6',
    'Layer7':'0xF7',

};


recordButton.addEventListener('click', () => {
    keypressDisplay.textContent = 'Waiting for a keypress...';

    document.addEventListener('keydown', function onKeyPress(event) {
        // Map event.code to corresponding HID code
        if (keyEventToHidMap[event.code]) {
            selectedHidCode = keyEventToHidMap[event.code];
            const char = hidToCharMap[selectedHidCode];
            keypressDisplay.textContent = `Key pressed: ${event.key}, HID Code: ${selectedHidCode}`;
            //alert(`Key pressed: ${event.key}, HID Code: ${selectedHidCode}`);
        } else {
            alert('Unrecognized key!');
        }

        // Remove event listener after the keypress is captured
        document.removeEventListener('keydown', onKeyPress);
    });
});


// Handle key click events on the keyboard
document.querySelectorAll('.key').forEach(key => {
    key.addEventListener('click', () => {
        if (selectedHidCode) {
            const char = hidToCharMap[selectedHidCode] || hidToCharMap[selectedHidCode];  // Use character or the keycode
            key.textContent = char;  // Display the keycode character
            keyMappings[key.dataset.key] = selectedHidCode;  // Map the key location to the selected keycode
            console.log(`Mapped key: ${key.dataset.key} to HID code: ${selectedHidCode}`);
            selectedHidCode = null;  // Reset after mapping
        } else {
            alert('Select a keycode first!');
        }
    });
});
