#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

// Nom du reseau et mot de passe
const char *ssid = "reseau";
const char *password = "azerty12";

const int led = 2;
const int capteurvitesse = 34;

// Déclaration des variables globales pour les moteurs
int leftMotor = 0;
int rightMotor = 0;

AsyncWebServer server(80); // écoute sur le port 80 en asynchrone

/* Fonction pour se connecter au wifi */
void connectToWiFi() {
  Serial.println("Initialisation du WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");
  delay(500);
  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 20000; // 20 secondes de timeout

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnexion établie!");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nImpossible de se connecter au WiFi.");
    // Optionnel : redémarrer l'ESP après échec de connexion
    ESP.restart();
  }
}

/* Fonction qui calcule les paramètres des roues en fonction de la position du joystick */
void calculateMotorValues(float angle, float distance) {
  //Serial.print("Calculating motor values for angle: ");
  Serial.print(angle);
  //Serial.print(" and distance: ");
  //Serial.println(distance);

  if (angle >= 0 && angle <= 180) { // Avant
    leftMotor = distance;
    rightMotor = distance * (1 - (angle / 90));
    if (angle > 90) rightMotor = -rightMotor;
  } else { // Arrière
    leftMotor = -distance;
    rightMotor = -distance * (1 - ((angle - 180) / 90));
    if (angle > 270) rightMotor = -rightMotor;
  }

  // Modifie les variables globales des moteurs
  leftMotor = round(min(255, max(-255, leftMotor)));
  rightMotor = round(min(255, max(-255, rightMotor)));

  Serial.print("Left Motor: ");
  Serial.print(leftMotor);
  Serial.print(" | Right Motor: ");
  Serial.println(rightMotor);
}

void setup() {
  // Initialisation Serial
  Serial.begin(115200);
  Serial.println("\n");

  // Initialisation GPIO
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  pinMode(capteurvitesse, INPUT);

  // Initialisation SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file) {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  // Connexion WiFi
  connectToWiFi();

  // Initialisation du serveur

  /* Charge le contenu de l'application */
  /* A l'adresse / de l'esp32, va chercher le fichier html dans la mémoire
  HTTP_GET est pour spécifier le seul type de requete accepté sur cette route
  serve.on est une méthode
  asyncWeb est une fonction lambda*/
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {// Début de l'appel de `server.on`
    request->send(SPIFFS, "/index.html", "text/html");
    Serial.println("index.html servi");
  }// Fin du corps de la fonction lambda
  ); // Fin de l'appel de `server.on`
  
  server.on("/INSA.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/INSA.png", "image/png");
    Serial.println("INSA.png servi");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
    Serial.println("style.css servi");
  });

  server.on("/nipplejs.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/nipplejs.min.js", "application/javascript");
    Serial.println("nipplejs.min.js servi");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "application/javascript");
    Serial.println("script.js servi");
  });

  /* Communication entre l'application et l'esp32        */

  /* lit la vitesse et l'envoie au serveur, ça se passe sur la route /lireVitesse*/
  server.on("/lireVitesse", HTTP_GET, [](AsyncWebServerRequest *request) {
    int val = analogRead(capteurvitesse); // lit la valeur du pin
    String vitesse = String(val); // convertit la valeur en string
    request->send(200, "text/plain", vitesse); // envoie une réponse http au client, de type texte brut
    Serial.println("Valeur de la Vitesse: " + vitesse);
  });

/*allumer la led*/
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(led, HIGH);
    request->send(200); //le 200 c'est pour "succès"
    Serial.println("LED allumée");
  });
/*eteindre la led*/
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(led, LOW);
    request->send(200);
    Serial.println("LED éteinte");
  });

  /* Recevoir les données du Joystick*/
  server.on("/joystick", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("angle") && request->hasParam("distance")) { //verifie qu'il y a bien les deux paramètres qu'on doit recevoir
      String angleStr = request->getParam("angle")->value(); // récupère la valeur du paramètre angle
      String distanceStr = request->getParam("distance")->value(); // récupère la valeur du paramètre distance

      if (angleStr == nullptr || distanceStr == nullptr) { // si les paramètres sont vide, envoie un message d'erreur
        request->send(400, "text/plain", "Invalid parameters");
        Serial.println("Invalid parameters for joystick");
        return;
      }

      float angle = angleStr.toFloat();
      float distance = distanceStr.toFloat();
      
      calculateMotorValues(angle, distance); //appel la fonction de calcul pour transposé la distance et l'angle en valeur pour les moteurs

      Serial.println("Left Motor: " + String(leftMotor) + " | Right Motor: " + String(rightMotor));
      request->send(200);
    } else {
      request->send(400, "text/plain", "Invalid parameters");
      Serial.println("Missing parameters for joystick");
    }
  });

  /* Recevoir les données des boutons*/
  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("action")) { // verifie qu'il y a un paramètre
      String action = request->getParam("action")->value(); // Récupère le paramètre action

      Serial.print("Boutton Commande: ");
      Serial.println(action);

      /* Valeur des moteurs suivant l'action */
      if (action == "avancer") {
        leftMotor = 255;
        rightMotor = 255;
      } else if (action == "gauche") {
        leftMotor = 100;
        rightMotor = -20;
      } else if (action == "droite") {
        leftMotor = -20;
        rightMotor = 100;
      } else if (action == "arriere") {
        leftMotor = -255;
        rightMotor = -255;
      } else {
        request->send(400, "text/plain", "Invalid action");
        Serial.println("Invalid action command");
        return;
      }

      Serial.println("Action: " + action + " | Left Motor: " + String(leftMotor) + " | Right Motor: " + String(rightMotor));
      request->send(200);
    } else {
      request->send(400, "text/plain", "Invalid parameters");
      Serial.println("Missing parameters for command");
    }
  });

  server.begin();
  Serial.println("Serveur actif!");
}

void loop() {

}
