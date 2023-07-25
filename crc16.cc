#include "crc16.hh"

uint16_t crc16(uint16_t crc, std::string buffer, size_t len) {
  const char *buf = buffer.c_str();
  while (len--)
    crc = crc16_byte(crc, *buf++);
  return crc;
}
