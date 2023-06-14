#include <AzureIotHub.h>
#include <Esp32MQTTClient.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

// SSiD y password de la red 
const char* ssid = "INFINITUM23A2_2.4";
const char* password = "7Y9hmQbF4s";

// api key de firebase
//#define API_KEY "AIzaSyAw0q0LU3jLQ0ANjl8cjUqUcrdd55vZvSE"
#define API_KEY "AIzaSyAw0q0LU3jLQ0ANjl8cjUqUcrdd55vZvSE"


// url de firebase
//#define DATABASE_URL "https://prueba1-f48ee-default-rtdb.firebaseio.com"
#define DATABASE_URL "https://retopotenciadoresesp32-default-rtdb.firebaseio.com"


// Aquí se define el proyecto fbdo y se le da autorización 
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
// Se guarda el valor de cada potenciometro y posteriormente se definen los pines de la ESP32 donde hay conexiones
int potentiometerValues[4] = {0}; 

void setup() {
  Serial.begin(115200);
  delay(10);
  setup_wifi();


  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A18, INPUT);
  pinMode(A19, INPUT);
}


void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up en el firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Signup successful");
    Firebase.reconnectWiFi(true); 
  } else {
    Serial.printf("Signup failed. Reason: %s\n", config.signer.signupError.message.c_str());
  }

  // generacion de token y callback status de las librerías add ons token helper 
  config.token_status_callback = tokenStatusCallback; 
  // Inicio y autorizacion 
  Firebase.begin(&config, &auth);
}
// Para leer los números que arrojan los potenciometros e imprimirlos por medio de analogRead y Serial print. Se usa println para escribir las leyendas en el serial monitor.
void loop() {
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();


    int pot1Value = analogRead(A4);
    int pot2Value = analogRead(A5);
    int pot3Value = analogRead(A18);
    int pot4Value = analogRead(A19);

    potentiometerValues[0] = pot1Value;
    potentiometerValues[1] = pot2Value;
    potentiometerValues[2] = pot3Value;
    potentiometerValues[3] = pot4Value;


    Serial.print("Potentiometer 1: ");
    Serial.println(pot1Value);
    Serial.print("Potentiometer 2: ");
    Serial.println(pot2Value);
    Serial.print("Potentiometer 3: ");
    Serial.println(pot3Value);
    Serial.print("Potentiometer 4: ");
    Serial.println(pot4Value);

    // Primero un if que sirve para autenticar el firebase
    if (Firebase.ready()) {
     // Después se iran subiendo los datos por este ciclo for que igualmente usa print y println 
      for (int i = 0; i < 4; i++) {
        String path = "potentiometer/pot" + String(i + 1);
        if (Firebase.RTDB.setInt(&fbdo, path.c_str(), potentiometerValues[i])) {
          Serial.println("potentiometers " + String(i + 1) + " value sent successfully");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        } else {
          Serial.println("Failed to send potentiometer " + String(i + 1) + " value");
          Serial.println("REASON: " + fbdo.errorReason());
        }
      }
    }
  }
}
