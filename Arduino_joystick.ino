const int PIN_X = A0;        // EIXO X
const int PIN_Y = A1;        // EIXO Y
const int PIN_THROTTLE = A2; // ACELERADOR 
const int PIN_BTN = 2;       // BOTÃO DO JOYSTICK
const int PIN_GTLH = 3;      // SWITCH MECÂNICO GATILHO

uint8_t ibusMessage[32];

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_GTLH, INPUT_PULLUP);
}

void loop() {
  // Leitura e Mapeamento
  uint16_t ch1 = map(analogRead(PIN_X), 0, 1023, 1000, 2000);
  uint16_t ch2 = map(analogRead(PIN_Y), 0, 1023, 1000, 2000);
  uint16_t ch5 = map(analogRead(PIN_THROTTLE), 0, 1023, 1000, 2000);
  
  // Pressionado (LOW) = 2000, Solto = 1000
  uint16_t ch3 = (digitalRead(PIN_BTN) == LOW) ? 2000 : 1000;
  uint16_t ch4 = (digitalRead(PIN_GTLH) == LOW) ? 2000 : 1000;

  // Cabeçalho IBus
  ibusMessage[0] = 0x20; 
  ibusMessage[1] = 0x40; 

  // Empacotamento Little Endian (Canais 1 a 5)
  ibusMessage[2] = ch1 & 0xFF;
  ibusMessage[3] = (ch1 >> 8) & 0xFF;
  ibusMessage[4] = ch2 & 0xFF;
  ibusMessage[5] = (ch2 >> 8) & 0xFF;
  ibusMessage[6] = ch3 & 0xFF;
  ibusMessage[7] = (ch3 >> 8) & 0xFF;

  ibusMessage[8] = ch4 & 0xFF;
  ibusMessage[9] = (ch4 >> 8) & 0xFF;

  ibusMessage[10] = ch5 & 0xFF;
  ibusMessage[11] = (ch5 >> 8) & 0xFF;

  // Preenche os outros 9 canais com o valor central (1500 = 0x05DC)
  for (int i = 5; i < 14; i++) {
    ibusMessage[2 + i*2] = 0xDC; 
    ibusMessage[3 + i*2] = 0x05;
  }

  // Checksum 
  uint16_t chksum = 0xFFFF;
  for (int i = 0; i < 30; i++) {
    chksum -= ibusMessage[i];
  }
  ibusMessage[30] = chksum & 0xFF;
  ibusMessage[31] = (chksum >> 8) & 0xFF;


  Serial.write(ibusMessage, 32);
  Serial.flush();

  
  delay(14); 
}
