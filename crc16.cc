#include "crc16.hh"

uint16_t crc16(uint16_t crc, std::string_view buf, size_t len) {
  unsigned i = 0;
  while (len--)
    crc = crc16_byte(crc, buf[i++]);
  return crc;
}
