#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <DHTesp.h>

// Reemplaza con tu SSID/Password
const char* ssid = "INFINITUM22F1";
const char* password = "Fb9Tr9Lg5p";

#define API_KEY "AIzaSyB54vJMoLzQZYhI-CWkv_M5ko6P3wzYXjk"
#define DATABASE_URL "https://pruebaenviodatos-8e4eb-default-rtdb.firebaseio.com"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

DHTesp dht;

// Definimos los pines de los 3 potenciometros
#define POT_1_PIN 32
#define POT_2_PIN 33
#define POT_3_PIN 34

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Configura el pin del sensor DHT
  dht.setup(26, DHTesp::DHT11);
  
  setup_wifi();
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

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

     // Leemos las señales analogicas de los potenciometros
    int potValue1 = analogRead(POT_1_PIN);
    int potValue2 = analogRead(POT_2_PIN);
    int potValue3 = analogRead(POT_3_PIN);

    // Envía la temperatura a Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "DatosSensores/temperature", temperature)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Envía la humedad a Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "DatosSensores/humidity", humidity)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Mandamos el valor del potenciometro 1
    if (Firebase.RTDB.setInt(&fbdo, "DatosSensores/potValue1", potValue1)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Mandamos el valor del potenciometro 2
    if (Firebase.RTDB.setInt(&fbdo, "DatosSensores/potValue2", potValue2)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Mandamos el valor del potenciometro 3
    if (Firebase.RTDB.setInt(&fbdo, "DatosSensores/potValue3", potValue3)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  
}
