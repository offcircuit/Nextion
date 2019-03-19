#ifndef MORSE_H
#define MORSE_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MORSE_NULL 0    // NULL
#define MORSE_DIT 1     // SHORT MARK
#define MORSE_DI 2      // SHORT ENDING MARK
#define MORSE_DAH 3     // LONG MARK
#define MORSE_GAP 4     // BETWEEN CHARACTERS
#define MORSE_CHAR 5    // BETWEEN CHARS
#define MORSE_SPACE 6   // BETWEEN WORDS (SPACE)
#define MORSE_EOL 7     // END OF LINE

#define MORSE_INVALID_CHAR 0x7C

class Morse {
  private:
    typedef void (*morsePointer) (uint8_t *);

    uint16_t _buffer = 1;

    morsePointer _receiver, _transmiter;

    uint8_t clear(uint8_t label);
    uint8_t count(uint16_t value);
    uint8_t decode();
    uint16_t encode(uint8_t character);

    void compose(String data, bool eol = false);
    uint8_t label(uint8_t tag);
    void send(uint8_t tag);

  public:
    explicit Morse() {};

    void begin(morsePointer rx, morsePointer tx);
    void clear();
    void listen(uint8_t tag = MORSE_NULL);
    void print(String data);
    void println(String data);
    String read(String data);
    void receiver(morsePointer pointer);
    void transmiter(morsePointer pointer);
};

#endif
