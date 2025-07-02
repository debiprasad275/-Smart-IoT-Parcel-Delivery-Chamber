#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Keypad.h>
#include <Stepper.h>
#include <ESP32Servo.h>

// --- WiFi & Telegram Setup ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* botToken = "YOUR_BOT_TOKEN"; // From BotFather
const String chatID = "YOUR_CHAT_ID";    // From @userinfobot
WiFiClientSecure secured_client;
UniversalTelegramBot bot(botToken, secured_client);

// --- Stepper Motor Setup ---
const int stepsPerRevolution = 2048;
Stepper stepper(stepsPerRevolution, 32, 33, 25, 26); // IN1-IN4

// --- Servo Motor Setup ---
#define SERVO_PIN 13
Servo lockServo;

// --- Ultrasonic Sensor ---
#define TRIG_PIN 12
#define ECHO_PIN 14
long duration;
int distance;

// --- Keypad Setup ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 2, 15};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- System Variables ---
String enteredPass = "";
String parcelOTP = "0000";
bool parcelPresent = false;
bool parcelCheckPending = false;
bool waitingForPickup = false;
unsigned long pickupTimer = 0;
unsigned long waitDuration = 6000;

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  secured_client.setInsecure();
}

void sendTelegramOTP(String otp) {
  String msg = "\xF0\x9F\x93\xA6 New parcel detected!\nYour OTP is: *" + otp + "*\nUse this to unlock your smart delivery chamber.";
  bool sent = bot.sendMessage(chatID, msg, "Markdown");
  if (sent) Serial.println("\u2705 OTP sent via Telegram.");
  else Serial.println("\u274C Failed to send Telegram message.");
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  lockServo.attach(SERVO_PIN);
  lockServo.write(90); // Locked
  stepper.setSpeed(10);
  randomSeed(analogRead(34));

  connectWiFi();
  Serial.println("\xF0\x9F\x93\xA6 Smart Locker Ready.");
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      Serial.print("\nEntered: ");
      Serial.println(enteredPass);

      if (enteredPass == "0000") {
        Serial.println("\u2705 Common password. Opening...");
        lockServo.write(0); delay(2000);
        stepper.step(-1024); delay(3000);
        parcelCheckPending = true;
      } else if (enteredPass == parcelOTP) {
        Serial.println("\u2705 OTP accepted. Releasing parcel...");
        lockServo.write(0); delay(2000);
        stepper.step(-1024); delay(3000);
        waitingForPickup = true;
        pickupTimer = millis();
      } else {
        Serial.println("\u274C Incorrect code.");
      }
      enteredPass = "";
    } else if (key == '*') {
      enteredPass = "";
      Serial.println("\uD83D\uDD01 Cleared input.");
    } else if (key >= '0' && key <= '9') {
      enteredPass += key;
      Serial.print("*");
    }
  }

  if (parcelCheckPending) {
    int dist = readDistance();
    Serial.print("\xF0\x9F\x93\x8F Parcel Check Distance: ");
    Serial.println(dist);

    if (dist > 0 && dist < 15) {
      parcelCheckPending = false;
      parcelPresent = true;
      Serial.println("\xF0\x9F\x93\xA6 Parcel detected. Securing...");

      delay(2000);
      stepper.step(1024);
      delay(1000);
      lockServo.write(90);

      int otpVal = random(1000, 9999);
      parcelOTP = String(otpVal);
      Serial.print("\xF0\x9F\x94\x90 OTP generated: ");
      Serial.println(parcelOTP);
      sendTelegramOTP(parcelOTP);
    }
  }

  if (waitingForPickup && millis() - pickupTimer > waitDuration) {
    int dist = readDistance();
    Serial.print("\xF0\x9F\x93\x8F Checking for parcel removal: ");
    Serial.println(dist);

    if (dist > 20 || dist == 0) {
      Serial.println("\xF0\x9F\x93\xA4 Parcel removed. Closing...");
      stepper.step(1024);
      delay(1000);
      lockServo.write(90);
      parcelOTP = "0000";
      parcelPresent = false;
      waitingForPickup = false;
      Serial.println("\uD83D\uDD04 System reset. Ready for next delivery.");
    } else {
      Serial.println("\u23F3 Parcel still inside. Waiting...");
      pickupTimer = millis();
    }
  }
}

int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return duration * 0.034 / 2;
}
