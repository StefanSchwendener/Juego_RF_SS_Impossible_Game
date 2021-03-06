//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
 //Se crea GitHub 2/05/20

//Version Modificada de Rodrigo Figueroa y Stefan Schwendener
// Link del Repositorio de cambios: https://github.com/Ricochetrj/Juego_RF_SS_Impossible_Game.git
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"
#include <SPI.h>
#include <SD.h>

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
void make_floor();
void Game_Start();
void gameover();
void animate();
void collision();
void jump();
void collision2();
void jump2();
void sd_highscore();
void plataforma();
extern uint8_t fondo[];
//extern uint8_t chonk[];
//extern uint8_t cubo2[];
extern uint8_t sans[];
extern uint8_t papyrus[];
extern uint8_t logo[];
//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
int jumps = PA_7;
uint8_t yB = 189;
uint8_t yB2 = 185;
uint8_t fallRateInt = 0;
float fallRate = 0;
uint8_t fallRateInt2 = 0;
float fallRate2 = 0;
///Botones para los jugadores 
uint8_t buttonStateOld = 0;
const int buttonPin = PA_6;  
uint8_t buttonState = 0; 

const int buttonPin2 = PE_2;  
uint8_t buttonState2; 
uint8_t buttonStateOld2 = 0;
uint8_t xspike = 200;
uint8_t xspike2; 
uint8_t xspike3; 
uint8_t spikescroll = -4;
uint8_t animc = 0;
uint8_t animsp = 0;
uint8_t animchonk = 0;
uint8_t animjerry = 0;
uint8_t animcstate = 0;
uint8_t animspstate = 0;
uint16_t animsp2= 0;
uint8_t spikeon = 0;
uint8_t Start=false;
uint16_t Points = 0;
uint8_t grounded = true;
uint8_t plane;
uint8_t Multiplayer;
uint8_t grounded2 = true;
uint8_t cube2y1 = 0;
uint8_t cube2y2 = 0;
uint8_t cube2x1 = 0;
uint8_t cube2x2 = 0;
uint8_t p1w = 0;
uint8_t p2w = 0;
uint8_t platx1 = 0;
uint8_t platx2 = 0;
uint8_t platy1 = 0;
String Puntos;

File myFile;
//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  pinMode(PA_7, OUTPUT);
  //pinMode(PA_6, INPUT);   
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(buttonPin2, INPUT_PULLUP);
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
Game_Start();
 while(!Start){
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(PE_2);
  animate();
    if(buttonState == LOW){
      Start = true;
      //LCD_Sprite(55,170,28,35,chonk,14,0,0,0);
      digitalWrite(PA_7,HIGH);
      Multiplayer = 0;
    }
    if(buttonState2 == LOW){
      Start = true;
      //LCD_Sprite(55,170,28,35,chonk,14,1,0,0);
      FillRect(55,155,28,35,0x00ff);
      Multiplayer = 1;
    }
  }
  FillRect(0, 0, 319, 219, 0x421b);
  String text1 = "Impossible Game!";
  LCD_Print(text1, 30, 20, 2, 0xffff, 0xD082);
  
 xspike = 212;
 make_floor();
  String score = "Score:";
  LCD_Print(score, 200, 50, 2, 0x0000, 0xD082);
  Puntos = String(Points);
  LCD_Print(Puntos, 200, 70, 2, 0x0000, 0xD082);
  

 
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************

void loop() {
 if(Multiplayer == 0){
  make_floor();
  Points= Points +0.001;
  Puntos = String(Points);
  LCD_Print(Puntos, 200, 70, 2, 0x0000, 0xD082);
  xspike2 = xspike + 100;
  xspike3 = xspike2 + 200;
  
  if(yB >= 189){
    grounded = true;
  }
  else if(yB < 189){
    grounded = false;
  }
  Points ++;
  Serial.println(Points);
  buttonState = digitalRead(buttonPin);
    LCD_Sprite(150,yB,16,30,sans,4,animc,0,0);
  

  

  
  FillRect(0, 187, 35, 30, 0x421b);
 //spikescroll -= .2;
  //la velocidad a la que se mueven los spikes acelera

  
////////////////////////////////////////

 ///Jump
   
    jump();    
    buttonStateOld = buttonState;
//spikes

xspike += spikescroll;
if(Points<200){
  if(xspike<292){
  LCD_Sprite(xspike,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike+27,189,27,29,0x421b);   
  }
  else if(xspike>292){
  xspike = 292;
  }
}

if(Points>=200 && Points<500){
  xspike2 = xspike + 100;
  if(xspike<292){
  LCD_Sprite(xspike,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike+27,189,27,29,0x421b);   
  }
  else if(xspike>292){
  xspike = 292;
  }

  if(xspike2<292){
  LCD_Sprite(xspike2,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike2+27,189,27,29,0x421b);   
  }
  else if(xspike2>292){
  xspike2 = xspike + 250;
  }
  
}

if(Points>=500 /*&& Points<1000*/){
  xspike2 = xspike + 250;
  xspike3 = xspike2 + 250;
  if(xspike<292){
  LCD_Sprite(xspike,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike+27,189,27,29,0x421b);   
  }
  else if(xspike>292){
  xspike = 292;
  }

  if(xspike2<292){
  LCD_Sprite(xspike2,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike2+27,189,27,29,0x421b);   
  }
  else if(xspike2>292){
  xspike2 = xspike + 250;
  }

  if(xspike3<292){
  LCD_Sprite(xspike3,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike3+27,189,27,29,0x421b);   
  }
  else if(xspike3>292){
  xspike3 = xspike2 + 250;
  }
}
///Plataformas
plane = 2;//random(0,3);
if(plane == 0){
LCD_Bitmap(xspike, 90, 32, 17, platform);
FillRect(xspike+32,90,32,17,0x421b);
}

if(plane == 1){
LCD_Bitmap(xspike, 120, 32, 17, platform);
FillRect(xspike+32,120,32,17,0x421b);
}

if(plane == 2){
LCD_Bitmap(xspike-60, 150, 32, 17, platform);
FillRect(xspike-60+32,150,32,17,0x421b);
}

FillRect(0,150,34,17,0x421b);
//animaciones
animate();

collision();


}




if(Multiplayer == 1){
   make_floor();
  Points= Points +0.001;
  Puntos = String(Points);
  LCD_Print(Puntos, 200, 70, 2, 0x0000, 0xD082);
  xspike2 = xspike + 100;
  xspike3 = xspike2 + 200;
  
  if(yB >= 176){
    grounded = true;
  }
  else if(yB < 176){
    grounded = false;
  }

  if(yB2 >= 176){
    grounded2 = true;
  }
  else if(yB2 < 176){
    grounded2 = false;
  }
  Points ++;
  Serial.println(Points);
  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);
    LCD_Sprite(150,yB,16,32,sans,4,animc,0,0); //sans = player 1 sprite
    LCD_Sprite(110,yB2,20,40,papyrus,4,animc,0,0);//papyrus = player 2 sprite
  FillRect(0, 187, 35, 30, 0x421b);





 ///Jump
   
    jump2();    
    buttonStateOld = buttonState;
    buttonStateOld2 = buttonState2;
//spikes

xspike += spikescroll;
if(Points<200){
  if(xspike<292){
  LCD_Sprite(xspike,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike+27,189,27,29,0x421b);   
  }
  else if(xspike>292){
  xspike = 292;
  }
}

if(Points>=200 && Points<500){
  xspike2 = xspike + 150;
  if(xspike<292){
  LCD_Sprite(xspike,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike+27,189,27,29,0x421b);   
  }
  else if(xspike>292){
  xspike = 292;
  }

  if(xspike2<292){
  LCD_Sprite(xspike2,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike2+27,189,27,29,0x421b);   
  }
  else if(xspike2>292){
  xspike2 = 292;
  }
  
}

if(Points>=500 /*&& Points<1000*/){
  xspike2 = xspike + 100;
  xspike3 = xspike2 + 100;
  if(xspike<292){
  LCD_Sprite(xspike,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike+27,189,27,29,0x421b);   
  }
  else if(xspike>292){
  xspike = 292;
  }

  if(xspike2<292){
  LCD_Sprite(xspike2,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike2+27,189,27,29,0x421b);   
  }
  else if(xspike2>292){
  xspike2 = xspike + 50;
  }

  if(xspike3<292){
  LCD_Sprite(xspike3,189,27,29,spikes,7,animsp,0,0);
  FillRect(xspike3+27,189,27,29,0x421b);   
  }
  else if(xspike3>292){
  xspike3 = xspike2 + 50;
  }
}
///Plataformas


FillRect(0,150,34,17,0x421b);

//animaciones
animate();
collision2();

}
}








//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
//void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
//  unsigned int i;
//  for (i = 0; i < h; i++) {
//    H_line(x  , y  , w, c);
//    H_line(x  , y+i, w, c);
//  }
//}

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
      
      //LCD_DATA(bitmap[k]);    
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Funciones Propias
//***************************************************************************************************************************************
void make_floor(){
   for(int x = 0; x <319; x++){
    LCD_Bitmap(x, 222, 17, 17, ground);
    x += 16;
 }
}

void Game_Start(){
  //FillRect(0, 0, 319, 219, 0x00ff);
  LCD_Bitmap(0, 0, 320, 240, fondo);
  LCD_Bitmap(80, 40, 154, 77, logo);
  String Start1 = "Press 1 ->"; 
  String Start2 = "Press 2 ->";
  String Player1 = "Single Player";
  String Player2 = "Multiplayer";
  LCD_Print(Start1, 10, 130, 1, 0xFF00, 0x0000);
  LCD_Print(Start2, 10, 180, 1, 0xff00, 0x0000);
  LCD_Print(Player1, 100, 130, 2, 0xffff, 0xD077);
  LCD_Print(Player2, 100, 180, 2, 0xffff, 0xC088);
  //LCD_Sprite(55,120,28,35,chonk,14,animchonk,0,0);
  xspike = 250;
  
}
////////////////////////////////////////
void jump(){
    fallRate = 0 ;
    fallRateInt= int(fallRate);
    yB+=fallRateInt; 
    if(grounded == true){
    if (buttonState == LOW && buttonStateOld == HIGH) {
      fallRate = -100;
      digitalWrite(PA_7,HIGH); 
    }
    fallRateInt= int(fallRate);
    yB+=fallRateInt; 
    FillRect(150, yB+99, 16, 32, 0x421b);
    
    }

    
    if(grounded == false){
    if( yB<189 ){
      fallRate = 4;
      digitalWrite(PA_7,LOW);
      //FillRect(150, 200, 16, 39, 0x421b);
    }
    FillRect(150, yB-4, 16, 4, 0x421b);
    
    fallRateInt= int(fallRate);
    yB+=fallRateInt; 
    }
}
///////////////////////////////////
void jump2(){
    fallRate = 0 ;
    fallRate2 = 0 ;
    fallRateInt= int(fallRate);
    fallRateInt2= int(fallRate2);
    yB+=fallRateInt;
    yB2+=fallRateInt2; 
    if(grounded == true){
    if (buttonState == LOW && buttonStateOld == HIGH) {
      fallRate = -100;
      digitalWrite(PA_7,HIGH);
    }
    fallRateInt= int(fallRate);
    yB+=fallRateInt; 
    FillRect(150, yB+100, 16, 30, 0x421b);
    }

     if(grounded2 == true){
      if (buttonState2 == LOW && buttonStateOld2 == HIGH) {
      fallRate2 = -100;
      digitalWrite(PA_7,HIGH);
    }
      fallRateInt2= int(fallRate2);
      yB2+=fallRateInt2; 
      FillRect(110, yB2+100, 20, 40, 0x421b);
     }

    
    if(grounded == false){
    if( yB<189 ){
      fallRate = 4;
      digitalWrite(PA_7,LOW);
      
    }
    fallRateInt= int(fallRate);
    yB+=fallRateInt; 
    FillRect(150, yB-4, 16, 4, 0x421b);
    }

    if(grounded2 == false){
      if( yB2<185 ){
      fallRate2 = 4;
      digitalWrite(PA_7,LOW);
    }
    fallRateInt2= int(fallRate2);
    yB2+=fallRateInt2;
    FillRect(110, yB2-4, 20, 4, 0x421b);
    }
    
     
}

//////////////////////
void collision(){
uint8_t cubey1 = yB;
uint8_t cubey2 = yB+30;
uint8_t cubex1 = 150;
uint8_t cubex2 = 166;

uint8_t spikey1 = 187;
uint8_t spikey2 = 187+29;
uint8_t spikex1 = xspike;
uint8_t spikex2 = xspike+27;

uint8_t spike2y1 = 187;
uint8_t spike2y2 = 187+29;
uint8_t spike2x1 = xspike2;
uint8_t spike2x2 = xspike2+27;

uint8_t spike3y1 = 187;
uint8_t spike3y2 = 187+29;
uint8_t spike3x1 = xspike3;
uint8_t spike3x2 = xspike3+27;
if (Points<200){
 if((cubex2>=spikex1) && (cubex1<=spikex2) &&(cubey2>=spikey1)){
  gameover();   
 }
}
if (Points>=200 && Points<500){
  if((cubex2>=spikex1) && (cubex1<=spikex2) &&(cubey2>=spikey1)){
  gameover();   
 }
  
  if((cubex2>=spike2x1) && (cubex1<=spike2x2) &&(cubey2>=spike2y1)){
  gameover();   
 }
}

if (Points>=500){
  if((cubex2>=spike3x1) && (cubex1<=spike3x2) &&(cubey2>=spike3y1)){
  gameover();   
 }
 if((cubex2>=spike2x1) && (cubex1<=spike2x2) &&(cubey2>=spike2y1)){
  gameover();   
 }
 if((cubex2>=spikex1) && (cubex1<=spikex2) &&(cubey2>=spikey1)){
  gameover();   
 }
}
}
///////////////////////////////
void collision2(){
uint8_t cubey1 = yB;
uint8_t cubey2 = yB+30;
uint8_t cubex1 = 150;
uint8_t cubex2 = 166;

uint8_t cube2y1 = yB2;
uint8_t cube2y2 = yB2+40;
uint8_t cube2x1 = 110;
uint8_t cube2x2 = 130;

uint8_t spikey1 = 187;
uint8_t spikey2 = 187+29;
uint8_t spikex1 = xspike;
uint8_t spikex2 = xspike+27;

uint8_t spike2y1 = 187;
uint8_t spike2y2 = 187+29;
uint8_t spike2x1 = xspike2;
uint8_t spike2x2 = xspike2+27;

uint8_t spike3y1 = 187;
uint8_t spike3y2 = 187+29;
uint8_t spike3x1 = xspike3;
uint8_t spike3x2 = xspike3+27;
if (Points<200){
 if((cubex2>=spikex1) && (cubex1<=spikex2) &&(cubey2>=spikey1)){
  p2w = 1;
  winner();   
 }

 if((cube2x2>=spikex1) && (cube2x1<=spikex2) &&(cube2y2>=spikey1)){
  p1w = 1;
  winner();   
 }
}
if (Points>=200 && Points<500){
  if((cube2x2>=spikex1) && (cube2x1<=spikex2) &&(cube2y2>=spikey1)){
  p1w = 1;
  winner(); 
 }
  
  if((cube2x2>=spike2x1) && (cube2x1<=spike2x2) &&(cube2y2>=spike2y1)){
  p1w = 1;
  winner();   
 }

 if((cubex2>=spikex1) && (cubex1<=spikex2) &&(cubey2>=spikey1)){
  p2w = 1;
  winner();  
 }
  
  if((cubex2>=spike2x1) && (cubex1<=spike2x2) &&(cubey2>=spike2y1)){
  p2w = 1;
  winner();  
 }
 
}

if (Points>=500){
  if((cubex2>=spike3x1) && (cubex1<=spike3x2) &&(cubey2>=spike3y1)){
  p2w = 1;
  winner();   
 }
 if((cubex2>=spike2x1) && (cubex1<=spike2x2) &&(cubey2>=spike2y1)){
  p2w = 1;
  winner();   
 }
 if((cubex2>=spikex1) && (cubex1<=spikex2) &&(cubey2>=spikey1)){
  p2w = 1;
  winner();  
 }

 if((cube2x2>=spike3x1) && (cube2x1<=spike3x2) &&(cube2y2>=spike3y1)){
  p1w = 1;
  winner();   
 }
 if((cube2x2>=spike2x1) && (cube2x1<=spike2x2) &&(cube2y2>=spike2y1)){
  p1w = 1;
  winner();
  
 }
 if((cube2x2>=spikex1) && (cube2x1<=spikex2) &&(cube2y2>=spikey1)){
  p1w = 1;
  winner();   
 }
}
}

void animate(){
animcstate += 3;
animspstate += 3;
animc = (animcstate/10)%4;
animsp = (animspstate/10)%7;
}
void gameover(){
  Start = false;
  FillRect(0, 0, 319, 219, 0x421b);
  while(!Start){
  String over = "Fuck You!";
  LCD_Print(over, 100, 100, 2, 0xffff, 0xD082);
  String again = "Press Jump to Start again";
  LCD_Print(again, 100, 150, 1, 0xf420, 0x0000);
  make_floor();
  buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
      Start = true;
    }
  }
  Game_Start();
  Start = false;
  while(!Start){
  buttonState2 = digitalRead(buttonPin2);
  buttonState = digitalRead(buttonPin);
  //sd_highscore();
    if(buttonState == LOW){
      Start = true;
      digitalWrite(PA_7,HIGH);
      Multiplayer = 0;
    }
    if(buttonState2 == LOW){
      Multiplayer = 1;
      Start = true;
      digitalWrite(PA_7,HIGH);
    }
  }
  FillRect(0, 0, 319, 219, 0x421b);
  String text1 = "Impossible Game!";
  LCD_Print(text1, 30, 20, 2, 0xffff, 0xD082);
   make_floor();
   String score = "Score:";
  LCD_Print(score, 200, 50, 2, 0x0000, 0xD082);
  Points = 0;
  xspike = 292;
  yB = 187;
  yB2 = 184;
  
  }

void winner(void){
 if(p1w == 1){
   
  Start = false;
  FillRect(0, 0, 319, 219, 0x421b);
  while(!Start){
  String text1 = "Player 1 Wins";
  LCD_Print(text1, 30, 20, 2, 0xffff, 0xD082);
  String again = "Press Jump to Start again";
  LCD_Print(again, 100, 150, 1, 0xf420, 0x0000);
  make_floor();
  buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
      Start = true;
    }
  }
  Game_Start();
  Start = false;
  while(!Start){
  buttonState2 = digitalRead(buttonPin2);
  buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
      Start = true;
      digitalWrite(PA_7,HIGH);
      Multiplayer = 0;
    }
    if(buttonState2 == LOW){
      Multiplayer = 1;
      Start = true;
      digitalWrite(PA_7,HIGH);
    }
  }
  FillRect(0, 0, 319, 219, 0x421b);
  String text3 = "Impossible Game!";
  LCD_Print(text3, 30, 20, 2, 0xffff, 0xD082);
   make_floor();
   String score = "Score:";
  LCD_Print(score, 200, 50, 2, 0x0000, 0xD082);
  Points = 0;
  xspike = 292;
  yB = 187;
  yB2 = 184;
  p1w = 0;
  p2w = 0;
 }

 if (p2w == 1){
  String text1 = "Player 2 Wins";
  LCD_Print(text1, 30, 20, 2, 0xffff, 0xD082);
  Start = false;
  FillRect(0, 0, 319, 219, 0x421b);
  while(!Start){
  String text1 = "Player 2 Wins";
  LCD_Print(text1, 30, 20, 2, 0xffff, 0xD082);
  String again = "Press Jump to Start again";
  LCD_Print(again, 100, 150, 1, 0xf420, 0x0000);
  make_floor();
  buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
      Start = true;
    }
  }
  Game_Start();
  Start = false;
  while(!Start){
  buttonState2 = digitalRead(buttonPin2);
  buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
      Start = true;
      digitalWrite(PA_7,HIGH);
      Multiplayer = 0;
    }
    if(buttonState2 == LOW){
      Multiplayer = 1;
      Start = true;
      digitalWrite(PA_7,HIGH);
    }
  }
  FillRect(0, 0, 319, 219, 0x421b);
  String text2 = "Impossible Game!";
  LCD_Print(text2, 30, 20, 2, 0xffff, 0xD082);
   make_floor();
   String score = "Score:";
  LCD_Print(score, 200, 50, 2, 0x0000, 0xD082);
  Points = 0;
  xspike = 302;
  yB = 187;
  yB2 = 184;
  p2w = 0;
  p1w = 0;
 }
}

void sd_highscore(){
  Serial.begin(9600);
   Serial.print("Checking Highscores");
   if (!SD.begin(4)) {
    Serial.println("None Available!");
    return;
  }
  Serial.println("We Found the Kush #420.");
  myFile = SD.open("highscore.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Are you worthy of the hall of fame?");
    myFile.println(Puntos);
    // close the file:
    myFile.close();
    Serial.println("done.");
    } else {
    // if the file didn't open, print an error:
    Serial.println("You Weren't Worthy ");
  }

  // re-open the file for reading:
  myFile = SD.open("highscore.txt");
  if (myFile) {
  Serial.println("Highscore List:");
   while (myFile.available()) {
      Serial.write(myFile.read());
    }
     myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("Empty");
  }
}
 void plataforma(){  
LCD_Bitmap(xspike, 90, 32, 17, platform);
FillRect(xspike+32,90,32,17,0x421b);
platx1 = xspike-32;
platx2 = xspike;
platy1 = 90;
 }
