const onOff_img = document.getElementById('onoffimage');
const colorpicker = document.getElementById('color-picker');
let powerState = 0;
let functionNumber = 0;

// web socket connection
var connection = new WebSocket('ws://192.168.178.106:81/', ['arduino']);
connection.onopen = function() {
  connection.send('Connect ' + new Date());
};
connection.onerror = function(error) {
  console.log('WebSocket Error ', error);
};
connection.onmessage = function(e) {
  console.log('Server: ', e.data);
};
connection.onclose = function() {
  console.log('WebSocket connection closed');
};
connection.onmessage = function(event) {
  processMessage(event);
};
function processMessage(event) {
  changePowerState(event.data[0]);
};

// send functions
function sendVelocity() {
  var velocity = document.getElementById('rainbow-velocity').value;
  var velocityString = 'E' + velocity.toString();
  connection.send(velocityString);
}

function sendSensingMode() {
  connection.send('#0');
}

function sendSteadyMode() {
  connection.send('#1');
}

function sendRainbowMode() {
  connection.send('#3');
}

function sendRunningRainbowMode() {
  connection.send('#4');
}

function sendTwinkleMode() {
  connection.send('#6');
}

function sendNightMode() {
  connection.send('#2');
}

function sendHSV() {
  var hex_color = document.getElementById('color-picker').value;
  var rgb_color = hex2rgb(hex_color);
  var hsv_color = rgb2hsv(rgb_color.r, rgb_color.g, rgb_color.b);
  console.log(hsv_color);
  connection.send('H' + hsv_color.h.toString());
  connection.send('S' + hsv_color.s.toString());
  connection.send('V' + hsv_color.v.toString());
}

// on/off button
function changePowerState(powerStateFromEsp) {
  if (powerState == 0) {
    powerState = 1;
    connection.send('ON');
    changeBorderColorToGreen();
  } else {
    powerState = 0;
    connection.send('OFF');
    changeBorderColorToRed();
  }
}

function changeBorderColorToRed() {
  onOff_img.classList.remove('green-glow');
  onOff_img.classList.add('red-glow');
}

function changeBorderColorToGreen() {
  onOff_img.classList.remove('red-glow');
  onOff_img.classList.add('green-glow');
}

function main() {
  onOff_img.addEventListener('click', () => changePowerState());
  colorpicker.addEventListener('input', () => sendHSV());
}

function hex2rgb(hex) {
  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } :
                  null;
}

function rgb2hsv(r, g, b) {
  let rabs, gabs, babs, rr, gg, bb, h, s, v, diff, diffc, percentRoundFn;
  rabs = r / 255;
  gabs = g / 255;
  babs = b / 255;
  v = Math.max(rabs, gabs, babs), diff = v - Math.min(rabs, gabs, babs);
  diffc = c => (v - c) / 6 / diff + 1 / 2;
  percentRoundFn = num => Math.round(num * 100) / 100;
  if (diff == 0) {
    h = s = 0;
  } else {
    s = diff / v;
    rr = diffc(rabs);
    gg = diffc(gabs);
    bb = diffc(babs);

    if (rabs === v) {
      h = bb - gg;
    } else if (gabs === v) {
      h = (1 / 3) + rr - bb;
    } else if (babs === v) {
      h = (2 / 3) + gg - rr;
    }
    if (h < 0) {
      h += 1;
    } else if (h > 1) {
      h -= 1;
    }
  }
  return {
    h: Math.round(h * 360),
    s: percentRoundFn(s * 100),
    v: percentRoundFn(v * 100)
  };
}

main();