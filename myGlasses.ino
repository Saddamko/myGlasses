//Based on DIY Arduino Glass Project
//https://github.com/awall9999/Arduino-Glass
//http://hackaday.com/2017/06/04/__trashed-15
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <SoftwareSerial.h>
#define BT_SERIAL_TX 6
#define BT_SERIAL_RX 7
SoftwareSerial BluetoothSerial(BT_SERIAL_TX, BT_SERIAL_RX);

#define OLED_MOSI 16
#define OLED_CLK 15
#define OLED_DC 8
#define OLED_CS 10
#define OLED_RESET 9

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


int f[8][2];                                             // Draw box
int c[8][3];   
float r1, x1, z1, y2;      
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
unsigned int Sicaklik = ' ';

String bluedata;
String command;
byte value;
byte valueall[15];
char valuechar[15];

int SCREEN_WIDTH = display.width();
int SCREEN_HEIGHT = display.height();

float d = 3;
float px[] = { 
  -d,  d,  d, -d, -d,  d,  d, -d };
float py[] = { 
  -d, -d,  d,  d, -d, -d,  d,  d };
float pz[] = { 
  -d, -d, -d, -d,  d,  d,  d,  d };

float p2x[] = {
  0,0,0,0,0,0,0,0};
float p2y[] = {
  0,0,0,0,0,0,0,0};

float r[] = { 0,0,0};

#define SHAPE_SIZE 600
// Define how fast the cube rotates. Smaller numbers are faster.
// This is the number of ms between draws.
#define ROTATION_SPEED 0


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
  delay(1);
}

void testdrawcircle(void) {
  for (int16_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
    delay(1);
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    delay(1);
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
    delay(1);
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
    delay(1);
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
    delay(1);
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
    delay(1);
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
    delay(1);
  }
}

void testdrawline() {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
    delay(1);
  }
  delay(250);
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
  delay(1);
 
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);    
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}

void setup() {
 // инициализация и очистка дисплея
 display.begin(SSD1306_SWITCHCAPVCC);
 display.clearDisplay();
 display.display();
 
 BluetoothSerial.begin(9600);
 Serial.begin(9600);
 
 Serial1.begin(9600);
 
  display.begin();    // Initialize the OLED
  delay(1000);
 
  display.display();  // Display what's in the buffer (splashscreen)
       // Delay 1000 ms

  display.display();

   // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw many lines
  testdrawline();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw rectangles
  testdrawrect();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw a white circle, 10 pixel radius
  display.fillCircle(display.width()/2, display.height()/2, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  testdrawroundrect();
  delay(2000);
  display.clearDisplay();

  testfillroundrect();
  delay(2000);
  display.clearDisplay();

  testdrawtriangle();
  delay(2000);
  display.clearDisplay();
   
  testfilltriangle();
  delay(2000);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  delay(2000);
  display.clearDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display();
  delay(2000);
  display.clearDisplay();

  // miniature bitmap display
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(1);

  // invert the display
  display.invertDisplay(true);
  delay(1000); 
  display.invertDisplay(false);
  delay(1000); 
  display.clearDisplay();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
  

 display.setCursor(20, 0);
 display.print("ArduinoGlasses V1.0");
 display.display();
 delay(2000); 
 display.display();
 display.setCursor(20, 10);
 display.print("Glasses");
 display.setCursor(10, 30);
 display.print("Ready");
 display.display();
// if (BluetoothSerial.available()>0){
//Sicaklik = BluetoothSerial.read();
//lcd.setCursor(0, 1);
//lcd.print(Sicaklik);
//lcd.print(" derece");
//delay(100);
// BluetoothSerial.write(Sicaklik);
  //for ( int q =  5; q < 10; q = q + 2 ){ cube(q);} // 
  //for ( int q = 10; q >  5; q = q - 2 ){ cube(q);}       //
}

void loop() {

  
  drawCube();
  delay(ROTATION_SPEED);
}


void cube(int q){                                        //
  int c[8][3] = {                                        // Cube
    {-q,-q, q},{q,-q, q},{q,q, q},{-q,q, q},             //
    {-q,-q,-q},{q,-q,-q},{q,q,-q},{-q,q,-q} };           //
  for (int a = 0; a <= 360; a = a + 3 ) {                // 0 to 360 degree
   for (int i = 0; i < 8; i++) {                         //
    r1       = a * 0.0174531;                             // 1 degree
    x1      = c[i][2] * sin(r1) + c[i][0] * cos(r1);       // rotate X
    z1      = c[i][2] * cos(r1) - c[i][0] * sin(r1);       // rotate Z          
    y2      = c[i][1] * cos(r1) - z1      * sin(r1);       // rotate Y
    f[i][0] = x1      * cos(r1) - y2      * sin(r1) + 64;  // X
    f[i][1] = x1      * sin(r1) + y2      * cos(r1) + 20;  // Y
    f[i][2] = c[i][1] * sin(r1) + z1      * cos(r1);       // Z
   }                                                     //
  display.clearDisplay();                                       //
//  display.drawString(32,16,"3D Cube");                   //
  display.drawLine(f[0][0],f[0][1],f[1][0],f[1][1],1);     //
  display.drawLine(f[1][0],f[1][1],f[2][0],f[2][1],1);     //
  display.drawLine(f[2][0],f[2][1],f[3][0],f[3][1],1);     //
  display.drawLine(f[3][0],f[3][1],f[0][0],f[0][1],1);     //
  display.drawLine(f[4][0],f[4][1],f[5][0],f[5][1],1);     //
  display.drawLine(f[5][0],f[5][1],f[6][0],f[6][1],1);     //
  display.drawLine(f[6][0],f[6][1],f[7][0],f[7][1],1);     //
  display.drawLine(f[7][0],f[7][1],f[4][0],f[4][1],1);     //
  display.drawLine(f[0][0],f[0][1],f[4][0],f[4][1],1);     //
  display.drawLine(f[1][0],f[1][1],f[5][0],f[5][1],1);     //
  display.drawLine(f[2][0],f[2][1],f[6][0],f[6][1],1);     //
  display.drawLine(f[3][0],f[3][1],f[7][0],f[7][1],1);     //
  display.drawLine(f[1][0],f[1][1],f[3][0],f[3][1],1);     // cross
  display.drawLine(f[0][0],f[0][1],f[2][0],f[2][1],1);     // cross
  display.display();                                     //
  delay(1);                                              //
  } 
}


void drawCube()
{
  r[0]=r[0]+PI/180.0; // Add a degree
  r[1]=r[1]+PI/180.0; // Add a degree
  r[2]=r[2]+PI/180.0; // Add a degree
  if (r[0] >= 360.0*PI/180.0) r[0] = 0;
  if (r[1] >= 360.0*PI/180.0) r[1] = 0;
  if (r[2] >= 360.0*PI/180.0) r[2] = 0;

  for (int i=0;i<8;i++)
  {
    float px2 = px[i];
    float py2 = cos(r[0])*py[i] - sin(r[0])*pz[i];
    float pz2 = sin(r[0])*py[i] + cos(r[0])*pz[i];

    float px3 = cos(r[1])*px2 + sin(r[1])*pz2;
    float py3 = py2;
    float pz3 = -sin(r[1])*px2 + cos(r[1])*pz2;

    float ax = cos(r[2])*px3 - sin(r[2])*py3;
    float ay = sin(r[2])*px3 + cos(r[2])*py3;
    float az = pz3-150;

    p2x[i] = SCREEN_WIDTH/2+ax*SHAPE_SIZE/az;
    p2y[i] = SCREEN_HEIGHT/2+ay*SHAPE_SIZE/az;
  }

  display.clearDisplay();
  for (int i=0;i<3;i++) 
  {
    display.drawLine(p2x[i],p2y[i],p2x[i+1],p2y[i+1],1);
    display.drawLine(p2x[i+4],p2y[i+4],p2x[i+5],p2y[i+5],1);
    display.drawLine(p2x[i],p2y[i],p2x[i+4],p2y[i+4],1);
  }    
  display.drawLine(p2x[3],p2y[3],p2x[0],p2y[0],1);
  display.drawLine(p2x[7],p2y[7],p2x[4],p2y[4],1);
  display.drawLine(p2x[3],p2y[3],p2x[7],p2y[7],1);
  display.display();
}

