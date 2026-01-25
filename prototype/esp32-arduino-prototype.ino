
#define MCU_ESP

#ifdef MCU_ESP
// esp devkit 38 pins
#define SERIAL_BAUDS 115200
#define BUTTON_PIN 34
#define HEAT_PIN 14
#define HX711_DOUT_PIN 13
#define HX711_SCK_PIN 25
#else
// arduino
#define SERIAL_BAUDS 9600
#define BUTTON_PIN 5
#define HEAT_PIN 4
#define HX711_DOUT_PIN 3
#define HX711_SCK_PIN 2
#endif

// esp devkit 38 pins

#define HX711_BITS 24
#define HX711_T1_MICROS 1
#define HX711_T2_MICROS 1
#define HX711_T3_MICROS 1
#define HX711_T4_MICROS 1
#define HX711_TSLEEP_MICROS 60

#define HX711_CHANNEL_BITS_A_GAIN_128 1
#define HX711_CHANNEL_BITS_B_GAIN_32 2
#define HX711_CHANNEL_BITS_A_GAIN_64 3

#define HX711_CHANNEL_BITS HX711_CHANNEL_BITS_A_GAIN_128

uint8_t hx711_read_bit() {
  digitalWrite(HX711_SCK_PIN, HIGH);
  delayMicroseconds(max(HX711_T2_MICROS, HX711_T3_MICROS));
  uint8_t value = digitalRead(HX711_DOUT_PIN);
  digitalWrite(HX711_SCK_PIN, LOW);
  delayMicroseconds(HX711_T4_MICROS);
  return value;
}

void setup() {
  Serial.begin(SERIAL_BAUDS);
  Serial.println("https://github.com/nipil/smart-plate-heater");

  pinMode(BUTTON_PIN, INPUT);
  pinMode(HX711_DOUT_PIN, INPUT);

  pinMode(HEAT_PIN, OUTPUT);
  digitalWrite(HEAT_PIN, LOW);

  pinMode(HX711_SCK_PIN, OUTPUT);

  // disable HX711
  digitalWrite(HX711_SCK_PIN, HIGH);
  delay(1);
}

// const int32_t RAW_SELF_SUSPENDED_WEIGHT = 357882L;
const int32_t RAW_SELF_GRAVITY_WEIGHT = 646718L;
// const int32_t RAW_EDGE_312_GRAMS = 793342L;
// const int32_t RAW_EDGE_1293_GRAMS = 1216109L;
// const int32_t RAW_EDGE_2254_GRAMS = 1615611L;
// const int32_t RAW_EDGE_2566_GRAMS = 1800440L;
// const int32_t RAW_EDGE_3547_GRAMS = 2248014L;
// const int32_t RAW_EDGE_3859_GRAMS = 2383308L;
// const int32_t RAW_EDGE_4619_GRAMS = 2609744L;
const int32_t UNITS_PER_GRAM = 438L;

const int32_t RAW_THRESHOLD_ALLOW_CENTER = 850000L;

// chauffe assiette centré + cordon secteur + 2ème interrupteur = 420 grammes
// chauffe assiette centré + cordon secteur + 2ème interrupteur + cable réseau = 450 grammes
// chauffe assiette centré + cordon secteur + cable réseau = 390 grammes
// 1130600 = bouteille 1293 g
// 5157735 = acier 4619 g

void loop() {

  // enable HX711
  digitalWrite(HX711_SCK_PIN, LOW);
  while (digitalRead(HX711_DOUT_PIN) == HIGH) {
    delayMicroseconds(1);
  }

  int32_t raw_value = 0;
  for (uint8_t i = 0; i < HX711_BITS; i++) {
    uint8_t bit_value = hx711_read_bit();
    raw_value = (raw_value << 1) | bit_value;
  }

  for (uint8_t i = 0; i < HX711_CHANNEL_BITS; i++) {
    hx711_read_bit();
  }

  // disable HX711
  digitalWrite(HX711_SCK_PIN, HIGH);
  delay(1);

  if (raw_value > 0x007FFFFF) {
    raw_value |= 0xFF800000;
  }

  if (raw_value <= 0) {
    // handle disconnections between modules
    digitalWrite(HEAT_PIN, LOW);
    delay(100);
    return;
  }

  // if under threshold, stop heating
  if (raw_value < RAW_THRESHOLD_ALLOW_CENTER) {
    digitalWrite(HEAT_PIN, LOW);
  } else {
    // if over threshold, check button
    if (digitalRead(BUTTON_PIN) == LOW) {
      digitalWrite(HEAT_PIN, HIGH);
      Serial.println("button");
    }
  }

  Serial.print(raw_value);
  Serial.print(" ");
  raw_value -= RAW_SELF_GRAVITY_WEIGHT;
  Serial.print(raw_value);
  Serial.print(" ");
  raw_value /= UNITS_PER_GRAM;
  Serial.println(raw_value);
}
