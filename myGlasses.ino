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
//  display.clear(ALL); // Clear the display's internal memory
  display.display();  // Display what's in the buffer (splashscreen)
       // Delay 1000 ms
//  display.clear(PAGE);
  display.display();
  
//  display.setFontType(1); 
  display.setCursor(20, 0);
 display.print("ArduinoGlasses V1.0");
 display.display();
 delay(2000); 
// display.clear(ALL);
// display.clear(PAGE);
 display.display();
 display.setCursor(20, 10);
 display.print("Glasses");
 display.setCursor(10, 30);
 display.print("Ready");
 display.display();
// display.clear(ALL);
//display.clear(PAGE);
// if (BluetoothSerial.available()>0){
//Sicaklik = BluetoothSerial.read();
//lcd.setCursor(0, 1);
//lcd.print(Sicaklik);
//lcd.print(" derece");
//delay(100);
// BluetoothSerial.write(Sicaklik);
    for ( int q =  5; q < 10; q = q + 2 ){ cube(q);} // 
  for ( int q = 10; q >  5; q = q - 2 ){ cube(q);}       //
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

