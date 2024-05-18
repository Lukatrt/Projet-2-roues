function onButton() {
    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "on", true);
    xhttp.send();
}

function offButton() {
    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "off", true);
    xhttp.send();
}

setInterval(function getData() {
    const xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if(this.readyState === 4 && this.status === 200) {
            document.getElementById("valeurVitesse").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "lireVitesse", true);
    xhttp.send();
}, 2000);

document.addEventListener('DOMContentLoaded', () => {
    // Fonction pour calculer la taille du joystick en fonction de la taille de l'écran
    function calculateJoystickSize() {
        const size = Math.min(window.innerWidth, window.innerHeight) * 0.5; // 20% de la plus petite dimension de l'écran
        return Math.max(100, size); // Assurez-vous que la taille est d'au moins 100 pixels
    }

    // Initialiser le joystick avec la taille calculée
    const joystickSize = calculateJoystickSize();

    const joystick = nipplejs.create({
        zone: document.getElementById('joystick'),
        mode: 'static',
        position: {left: '50%', top: '50%'},
        color: 'blue',
        size:joystickSize
    });

    joystick.on('move', function(evt, data) {
        if (data.direction && data.distance) {
            const xhttp = new XMLHttpRequest();
            let angle = data.angle.degree;
            let distance = data.distance;

            console.log(`Sending joystick data: angle=${angle}, distance=${distance}`);
            xhttp.open("GET", `/joystick?angle=${angle}&distance=${distance}`, true);
            xhttp.send();
        }
    });

    function sendButtonCommand(action) {
        const xhttp = new XMLHttpRequest();
        console.log(`Sending button command: action=${action}`);
        xhttp.open("GET", `/command?action=${action}`, true);
        xhttp.send();
    }

    document.getElementById('avancer').addEventListener('click', function() {
        sendButtonCommand('avancer');
    });

    document.getElementById('gauche').addEventListener('click', function() {
        sendButtonCommand('gauche');
    });

    document.getElementById('droite').addEventListener('click', function() {
        sendButtonCommand('droite');
    });

    document.getElementById('arriere').addEventListener('click', function() {
        sendButtonCommand('arriere');
    });
});
