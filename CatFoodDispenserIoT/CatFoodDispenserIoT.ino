#define BLYNK_TEMPLATE_ID "TMPL6wUtNklmo"
#define BLYNK_TEMPLATE_NAME "Cat Feeder Template"
#define BLYNK_AUTH_TOKEN "d5DB-IyLqCVKHocYVTnQt3PsoqPNK7Bd"
#define BLYNK_PRINT Serial
#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 25200
#define UTC_OFFSET_DST 0

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

char ssid[] = "";      // GANTI INI
char pass[] = "";  // GANTI INI

int putaran = 0;
bool flag = true;

class Schedule {
public:
  int hour;
  int minute;

  int getHour() {
    return hour;
  }

  int getMinute() {
    return minute;
  }

  void setHour(int hourParam) {
    hour = hourParam;
  }

  void setMinute(int minuteParam) {
    minute = minuteParam;
  }
} schedule_one, schedule_two, REALTIME;

String timeSCH1 = "NULL";
String timeSCH2 = "NULL";

BlynkTimer timer;
Servo myServo;

BLYNK_WRITE(V0) {
  String timeSCH = param.asString();
  if (timeSCH.equals("NULL")) {
    Serial.println("Jadwal 1: Kosong");
    timeSCH1 = "NULL";
  } else {
    Serial.println("Jadwal 1: " + timeSCH);
    timeSCH1 = timeSCH;
    int hour = timeSCH.substring(0, 2).toInt();
    int minute = timeSCH.substring(3).toInt();

    schedule_one.setHour(hour);
    schedule_one.setMinute(minute);
  }
}

BLYNK_WRITE(V1) {
  String timeSCH = param.asString();
  if (timeSCH.equals("NULL")) {
    Serial.println("Jadwal 2: Kosong");
    timeSCH2 = "NULL";
  } else {
    Serial.println("Jadwal 2: " + timeSCH);
    timeSCH2 = timeSCH;
    int hour = timeSCH.substring(0, 2).toInt();
    int minute = timeSCH.substring(3).toInt();

    schedule_two.setHour(hour);
    schedule_two.setMinute(minute);
  }
}

BLYNK_WRITE(V2) {
  putaran = param.asInt();
  String berapa = param.asString();
  Serial.println("Putaran yang dijalankan: " + berapa);
}

BLYNK_CONNECTED() {
  Serial.println("Kode dijalankan di dalam BLYNK_CONNECTED");
}

void checkTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Time Connection Error");
  } else {
    REALTIME.setHour(String(timeinfo.tm_hour).toInt());
    REALTIME.setMinute(String(timeinfo.tm_min).toInt());
  }

  if ((REALTIME.getHour() == schedule_one.getHour() && REALTIME.getMinute() == schedule_one.getMinute() && String(timeinfo.tm_sec).toInt() == 0) || (REALTIME.getHour() == schedule_two.getHour() && REALTIME.getMinute() == schedule_two.getMinute() && String(timeinfo.tm_sec).toInt() == 0)) {
    putaran = 1;
  }
}

void setup() {
  Serial.begin(115200);

  myServo.attach(D12, 500, 2500);
  myServo.write(0);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("\nConnecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (Blynk.connected()) {
    Serial.println("Koneksi Blynk SUKSES");
  }

  else {
    Serial.println("Koneksi Blynk GAGAL");
  }

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  Blynk.run();
  timer.run();

  checkTime();
  while (putaran > 0) {
    Serial.println("Putaran yang dijalankan: " + String(putaran));
    if (flag) {
      for (int i = 0; i <= 180; i++) {
        myServo.write(i);
      }
      flag = false;
    } else {
      for (int i = 180; i >= 0; i--) {
        myServo.write(i);
      }
      flag = true;
    }
    
    delay(3000);
    putaran--;
  }

  Blynk.virtualWrite(V2, "0");
}