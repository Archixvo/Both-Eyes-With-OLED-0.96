#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin joystick
const int VRx = A0;
const int VRy = A1;
const int SW = 2; // Pin untuk tombol joystick (tekan)

// Posisi mata (diatur agar berada di tengah layar)
int eyeRadius = 15;       // Lebih besar untuk mata yang lebih jelas
int irisRadius = 7;       // Ukuran iris dalam bola mata
int pupilRadius = 3;      // Ukuran pupil di dalam iris
int eyeSpacing = 25;      // Jarak antara kedua mata lebih kecil agar mata lebih ke tengah
int eyeCenterX = SCREEN_WIDTH / 2 - eyeSpacing / 2;
int eyeCenterY = SCREEN_HEIGHT / 2;

// Variabel untuk posisi mata
int leftEyeX = eyeCenterX - eyeSpacing;
int rightEyeX = eyeCenterX + eyeSpacing;
int eyeY = eyeCenterY;

// Variabel untuk berkedip
bool isBlinking = false;
unsigned long previousBlinkTime = 0;
unsigned long blinkDuration = 150;  // Durasi mata tertutup saat berkedip

// Fungsi untuk menggerakkan bola mata dengan halus
int smoothMove(int currentPos, int targetPos, int speed) {
  if (currentPos < targetPos) {
    currentPos += speed;
  } else if (currentPos > targetPos) {
    currentPos -= speed;
  }
  return currentPos;
}

// Fungsi menggambar mata
void drawEye(int centerX, int centerY, int eyeOffsetX, int eyeOffsetY, bool isBlinking) {
  if (isBlinking) {
    // Menggambar mata tertutup dengan kelopak mata
    for (int i = 0; i <= eyeRadius; i += 2) {
      display.drawLine(centerX - eyeRadius, centerY - i, centerX + eyeRadius, centerY - i, SSD1306_WHITE);
    }
  } else {
    // Batas gerak iris agar tidak keluar dari lingkaran mata
    int maxOffset = eyeRadius - irisRadius;
    eyeOffsetX = constrain(eyeOffsetX, -maxOffset, maxOffset);
    eyeOffsetY = constrain(eyeOffsetY, -maxOffset, maxOffset);

    // Menggambar bola mata (lingkaran luar)
    display.drawCircle(centerX, centerY, eyeRadius, SSD1306_WHITE);
    
    // Menggambar iris (lingkaran tengah)
    int irisX = centerX + eyeOffsetX;
    int irisY = centerY + eyeOffsetY;
    display.fillCircle(irisX, irisY, irisRadius, SSD1306_WHITE);
    
    // Menggambar pupil (lingkaran dalam)
    display.fillCircle(irisX, irisY, pupilRadius, SSD1306_BLACK);

    // Efek kilau pada pupil (simulasi pencahayaan)
    display.fillCircle(irisX - 1, irisY - 2, 1, SSD1306_WHITE);
  }
}

void setup() {
  // Inisialisasi joystick
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);  // Tombol joystick dengan pullup

  // Inisialisasi layar OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
}

void loop() {
  // Membaca posisi joystick
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);
  int swState = digitalRead(SW);

  // Memetakan nilai joystick ke posisi bola mata
  int eyeOffsetX = map(xValue, 0, 1023, -10, 10);  // Offset diperbesar agar gerakan mata lebih terlihat
  int eyeOffsetY = map(yValue, 0, 1023, -10, 10);

  // Cek jika tombol joystick ditekan untuk berkedip
  if (swState == LOW) {
    isBlinking = true;
    previousBlinkTime = millis();
  }

  // Mengecek apakah sudah waktunya membuka mata setelah berkedip
  if (isBlinking && millis() - previousBlinkTime >= blinkDuration) {
    isBlinking = false;
  }

  // Membersihkan layar
  display.clearDisplay();

  // Menggambar mata kiri dan kanan
  drawEye(eyeCenterX - eyeSpacing, eyeCenterY, eyeOffsetX, eyeOffsetY, isBlinking);
  drawEye(eyeCenterX + eyeSpacing, eyeCenterY, eyeOffsetX, eyeOffsetY, isBlinking);

  // Tampilkan di layar
  display.display();

  delay(30);  // Menambah delay agar pergerakan lebih smooth
}
