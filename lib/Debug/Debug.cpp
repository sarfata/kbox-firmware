
#include <Arduino.h>
#include <stdarg.h>

void debug(const char *fname, int lineno, const char *fmt, ... ) {
  Serial.print(fname);
  Serial.print(":");
  Serial.print(lineno);
  Serial.print(" ");
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  Serial.println(tmp);
}
