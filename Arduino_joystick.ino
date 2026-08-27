const int PIN_X = A0;
const int PIN_Y = A1;
const int PIN_BTN = 2;

// Array global com o tamanho exato do pacote IBus
uint8_t ibusMessage[32];

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BTN, INPUT_PULLUP);
}

void loop() {
  // 1. Leitura e Mapeamento
  uint16_t ch1 = map(analogRead(PIN_X), 0, 1023, 1000, 2000);
  uint16_t ch2 = map(analogRead(PIN_Y), 0, 1023, 1000, 2000);
  uint16_t ch3 = (digitalRead(PIN_BTN) == LOW) ? 2000 : 1000;

  // 2. Cabeçalho IBus
  ibusMessage[0] = 0x20; 
  ibusMessage[1] = 0x40; 

  // 3. Empacotamento Little Endian (Canais 1 a 3)
  ibusMessage[2] = ch1 & 0xFF;
  ibusMessage[3] = (ch1 >> 8) & 0xFF;
  ibusMessage[4] = ch2 & 0xFF;
  ibusMessage[5] = (ch2 >> 8) & 0xFF;
  ibusMessage[6] = ch3 & 0xFF;
  ibusMessage[7] = (ch3 >> 8) & 0xFF;

  // Preenche os outros 11 canais com o valor central (1500 = 0x05DC)
  for (int i = 3; i < 14; i++) {
    ibusMessage[2 + i*2] = 0xDC; 
    ibusMessage[3 + i*2] = 0x05;
  }

  // 4. Checksum Matemático
  uint16_t chksum = 0xFFFF;
  for (int i = 0; i < 30; i++) {
    chksum -= ibusMessage[i];
  }
  ibusMessage[30] = chksum & 0xFF;
  ibusMessage[31] = (chksum >> 8) & 0xFF;

  // 5. Injeção direta na UART e limpeza de buffer
  Serial.write(ibusMessage, 32);
  Serial.flush(); // Segura o processador até o último bit sair

  // 14ms é o framerate cravado do protocolo IBus original
  delay(14); 
}