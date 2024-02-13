volatile bool state = false;
volatile bool PIRstate = false;

int timer1_compare_match;

ISR(TIMER1_COMPA_vect) {
}

const int pirPin = 6;
const int ledPin2 = 2;
const int ledPin3 = 3;
const int ledPin4 = 4;
const int buzzerPin = 8;
const int buttonPin = 7;
const int newLedPin = 11;
const int motionThreshold = 1;
const int motionCheckDelay = 500;

int celsius = 0;
int baselineTemp = 40;
unsigned long previousMillis = 0;
unsigned long lastMotionCheck = 0;
unsigned long lastBlinkMillis = 0;
bool blinkLed = false;
const long interval = 2000;

void setup() {
  pinMode(A0, INPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);

  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(newLedPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  timer1_compare_match = 31249;
  TCNT1 = timer1_compare_match;
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

void loop() {
  unsigned long currentMillis = millis();

  celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" C");
  controlLEDs();

  if (currentMillis - lastMotionCheck >= motionCheckDelay) {
    checkMotion(currentMillis);
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    toggleLED();
  }

  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    blinkLed = true;
    lastBlinkMillis = millis();
  }

  if (blinkLed) {
    if (currentMillis - lastBlinkMillis >= 2000) {
      lastBlinkMillis = currentMillis;
      digitalWrite(newLedPin, !digitalRead(newLedPin));
    }
  }
}

void controlLEDs() {
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  digitalWrite(ledPin4, LOW);

  if (celsius >= baselineTemp && celsius < baselineTemp + 10) {
    digitalWrite(ledPin2, HIGH);
  } else if (celsius >= baselineTemp + 10 && celsius < baselineTemp + 20) {
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
  } else if (celsius >= baselineTemp + 20) {
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin4, HIGH);
  }
}

void checkMotion(unsigned long currentMillis) {
  lastMotionCheck = currentMillis;

  int currentPirState = digitalRead(pirPin);
  if (currentPirState == HIGH) {
    Serial.println("Motion detected!");
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
  }
}

void toggleLED() {
  digitalWrite(ledPin4, !digitalRead(ledPin4));
}
