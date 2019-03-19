#include "Morse.h"

void Morse::begin(morsePointer rx, morsePointer tx) {
  _buffer = 1;
  _receiver = rx;
  _transmiter = tx;
}

void Morse::clear() {
  _buffer = 1;
}

uint8_t Morse::clear(uint8_t label) {
  _buffer = 1;
  return label;
}

void Morse::compose(String data, bool eol = false) {
  data.toUpperCase();

  for (size_t i = 0; i < data.length(); i++) {
    uint16_t code = encode(data[i]);
 
    if (code != 1) {
    uint8_t n = 0;
     
     do {
        send(2 + bitRead(code, n) - !(bitRead(code, n++) | (code >> (n + 1)))); // SEND SIGNAL, STARTING FROM VALUE 2 AND DECREASE 1 IF IS 0 AND LAST
        if (code >> (n + 1)) send(MORSE_GAP);
      } while (code >> (n + 1));

      if (i < (data.length() - eol)) send(MORSE_CHAR);
    } else if (i < data.length()) send(MORSE_SPACE);
  }
  if (eol) send(MORSE_EOL);
}

uint8_t Morse::count(uint16_t value) {
  uint8_t count = 0;
  do count++;
  while (value = value >> 1);
  return count;
}

uint8_t Morse::decode() {
  if (_buffer < 0x100) for (size_t i = 32; i <= 95; i++) if (encode(i) == _buffer) return i;
  return MORSE_INVALID_CHAR;
}

uint16_t Morse::encode(uint8_t character) {
  switch (character) {
    case 32: return 0b1;          // SPACE

    case 33: return 0b1110101;    // !
    case 34: return 0b1010010;    // "
    case 36: return 0b11001000;   // $

    case 38: return 0b100010;     // &
    case 39: return 0b1011110;    // '
    case 40: return 0b101101;     // (
    case 41: return 0b1101101;    // )

    case 43: return 0b101010;     // +
    case 44: return 0b1110011;    // ,
    case 45: return 0b1100001;    // -
    case 46: return 0b1101010;    // .
    case 47: return 0b101001;     // /

    case 48: return 0b111111;     // 0
    case 49: return 0b111110;     // 1
    case 50: return 0b111100;     // 2
    case 51: return 0b111000;     // 3
    case 52: return 0b110000;     // 4
    case 53: return 0b100000;     // 5
    case 54: return 0b100001;     // 6
    case 55: return 0b100011;     // 7
    case 56: return 0b100111;     // 8
    case 57: return 0b101111;     // 9

    case 58: return 0b1000111;    // :
    case 59: return 0b1010101;    // ;

    case 61: return 0b110001;     // =

    case 63: return 0b1001100;    // ?
    case 64: return 0b1010110;    // @

    case 65: return 0b110;        // A
    case 66: return 0b10001;      // B
    case 67: return 0b10101;      // C
    case 68: return 0b1001;       // D
    case 69: return 0b10;         // E
    case 70: return 0b10100;      // F
    case 71: return 0b1011;       // G
    case 72: return 0b10000;      // H
    case 73: return 0b100;        // I
    case 74: return 0b11110;      // J
    case 75: return 0b1101;       // K
    case 76: return 0b10010;      // L
    case 77: return 0b111;        // M
    case 78: return 0b101;        // N
    case 79: return 0b1111;       // O
    case 80: return 0b10110;      // P
    case 81: return 0b11011;      // Q
    case 82: return 0b1010;       // R
    case 83: return 0b1000;       // S
    case 84: return 0b11;         // T
    case 85: return 0b1100;       // U
    case 86: return 0b11000;      // V
    case 87: return 0b1110;       // W
    case 88: return 0b11001;      // X
    case 89: return 0b11101;      // Y
    case 90: return 0b10011;      // Z

    case 95: return 0b1101100;    // _

    default: return 0b100000000;  // INVALID OR ERROR
  }
}

uint8_t Morse::label(uint8_t tag) {
  switch (tag) {
    case MORSE_DI: case MORSE_DIT:
      bitSet(_buffer, count(_buffer)); // ADD A BIT TO THE LEFT - INDEX 0
      bitClear(_buffer, count(_buffer) - 2); // CLEAR THE SECOND FROM LEFT BIT - INDEX 1
      break;
    case MORSE_DAH:
      bitSet(_buffer, count(_buffer)); // ADD A BIT TO THE LEFT - INDEX 0
      break;
    case MORSE_GAP: return MORSE_NULL;
    case MORSE_CHAR: return clear(decode());
    case MORSE_SPACE: return clear(32);
    case MORSE_EOL: return clear(decode());
  }

  if (_buffer > 0x100) return clear(MORSE_INVALID_CHAR);

  return MORSE_NULL;
}

void Morse::listen(uint8_t tag = MORSE_NULL) {
  if (_receiver && (tag = label(tag))) _receiver(char(tag));
}

void Morse::print(String data) {
  compose(data);
}

void Morse::println(String data) {
  compose(data, true);
}

String Morse::read(String data) {
  String string = "";
  uint8_t buffer = _buffer, tag;
  for (size_t i = 0; i < data.length(); i++) if (tag = label(String(data[i]).toInt())) string += char(tag);
  _buffer = buffer;
  return string;
}

void Morse::receiver(morsePointer pointer) {
  _buffer = 1;
  _receiver = pointer;
}

void Morse::send(uint8_t tag) {
  if (_transmiter) _transmiter(tag);
}

void Morse::transmiter(morsePointer pointer) {
  _buffer = 1;
  _transmiter = pointer;
}
