// VMC-ATtiny85 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// VMC-ATtiny85 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with VMC-ATtiny85.  If not, see <https://www.gnu.org/licenses/>.

#include <dht.h>

#define RELAY_PIN 1
#define LED_PIN 3
#define DHT11_PIN 4
#define REGULAR_HUMIDITY 55.0

#define RELAY_ON() digitalWrite(RELAY_PIN, LOW)
#define RELAY_OFF() digitalWrite(RELAY_PIN, HIGH)
#define LED_ON() digitalWrite(LED_PIN, HIGH)
#define LED_OFF() digitalWrite(LED_PIN, LOW)

dht DHT;
int latest_measure;

void flash_led(int measure)
{
  LED_OFF();
  delay(100);
  for (int i = 0; i < measure / 10; i++) {
    LED_ON();
    delay(300);
    LED_OFF();
    delay(300);
  }
}

int check_measure(int check)
{
  static int err_count = 0;

  if (check != DHTLIB_OK) {
    err_count++;
    if (err_count > 3) {
      return 1;
    }
    return 0;
  } 
  err_count = 0;
  return 0;
}

int impossible_variation(int measure)
{
  if (abs(measure - latest_measure) > 30)
    return 1;
  return 0;
}

void led_error_display(void)
{
  LED_ON();
  delay(50);
  LED_OFF();
  delay(50);
  LED_ON();
  delay(50);
  LED_OFF();
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  LED_OFF();
  RELAY_ON();
  LED_ON();
  delay(5000);
  RELAY_OFF();
  LED_OFF();
  int chk = DHT.read11(DHT11_PIN);
  if (check_measure(chk)) {
    led_error_display();
    RELAY_OFF();
    return;
  }
  latest_measure = DHT.humidity;
}

void loop()
{
  int chk;
  int h = 0;
  
  delay(2500);
  chk = DHT.read11(DHT11_PIN);
  if (check_measure(chk)) {
    led_error_display();
    RELAY_OFF();
    return;
  }
  h = DHT.humidity;
  if (impossible_variation(h)) {
    return;
  }
  latest_measure = h;
  flash_led(h - 5);
  if (h < REGULAR_HUMIDITY) {
    LED_OFF();
    RELAY_OFF();
  } else  if (h > REGULAR_HUMIDITY + 5.0) {
    LED_ON();
    RELAY_ON();
  }
}
