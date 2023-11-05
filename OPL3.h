#include <Arduino.h>

class OPL3 {
  public:
    explicit OPL3();
    void reset(void);
    void writereg(const uint8_t addr, const uint8_t val, const uint8_t port);
  private:
    void write8(const uint8_t data);
};
