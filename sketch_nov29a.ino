/*Программа для управления камерой*/
#include <SoftwareSerial.h>
int sensorIris = A2;     // канал вправо/влево 
int sensorFocus = A1;    // канал фокус 
int sensorZoom = A0;   // канал зум
int zoom = 0;          // переменная для хранения значения с потенциометра фокуса камеры
int focus = 0;
int iris = 0;

const byte focusNull = 0x00;  // Зума нет
const byte zoomIn = 0x20;  // Зум +  0x01
const byte zoomOut = 0x40;   // Зум -  0x80  
const byte focusFar = 0x80;
const byte focusNear = 0x01;
const byte irisClose = 0x04;
const byte irisOpen =  0x02;

SoftwareSerial newSerial = SoftwareSerial(7, 8);
void setup()
{
   pinMode(7, INPUT); // TX
   pinMode(8, OUTPUT); // RX
   newSerial.begin(2400); // включаем uart 
}
// отправка данных камере
// структура посылки Pelco-D
/*
|  byte 1  |   byte 2   |  byte 3  |  byte 4  | byte 5 | byte 6 | byte 7 |
 ---------- ------------ ---------- ---------- -------- -------- --------
|Sync 0xFF | Cam Adress | Command1 | Command2 | Data 1 | Data 2 |Checksum|
*/
void sendData(byte camNum, byte command_1, byte command_2, byte panSpeed, byte tiltSpeed) 
{
   int modSum = 0;                 // начальное нулевое значение контрольной суммы 
   byte dataVal[6] = {0xFF, camNum, command_1, command_2, panSpeed, tiltSpeed}; // вектор формата комманды Pelco-D
   for (int i=0; i<6; i++)         // цикл отправки посылки камере
   {
      newSerial.write(dataVal[i]);  // отправить байт в UART
      if (i > 0)                   // контрольная сумма начинает считаться со второго байта, поэтому первый 0xFF пропускаем
         modSum += dataVal[i];     // суммируем байты контрольной суммы
   }
   modSum %= 256;                  // контрольная сумма делиться по модуля на 256 в dec или по другому на 100 в hex
   newSerial.write(modSum);         // отправить контрольную сумму
}
void stopping()
{
   byte dataVal[7] = {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01};
   for (int i=0; i<7; i++)
   {
      newSerial.write(dataVal[i]);  // отправить байт в UART   
   } 
}
void loop() 
{
   zoom = analogRead(sensorZoom);  // считывание значения потенциометра фокуса камеры в соответствующую переменную
   focus = analogRead(sensorFocus);
   iris = analogRead(sensorIris); 
   
   if (zoom > 600)
   {
      sendData(0x01, focusNull, zoomIn, 0x00, 0x00);      // вызывается функция отправки байта
      while (zoom > 600) 
      {
         focus = analogRead(sensorZoom);
         newSerial.println(zoom);   
         delay (500);   
      }
   } 
   if (zoom < 450)
   {
      sendData(0x01, focusNull, zoomOut, 0x00, 0x00);      // вызывается функция отправки байта  
      while (zoom < 450) 
      {
         focus = analogRead(sensorZoom);   
        newSerial.println(zoom);
        delay (500);   
      }
   } 
   if (focus > 600)
   {
      sendData(0x01, focusNull, focusNear, 0x00, 0x00);      // вызывается функция отправки байта
      while (focus > 600) 
      {
         focus = analogRead(sensorFocus);
         newSerial.println(focus);   
         delay (500);   
      }
   } 
   if (focus < 450)
   {
      sendData(0x01, focusNull, focusFar, 0x00, 0x00);      // вызывается функция отправки байта  
      while (zoom < 450) 
      {
         focus = analogRead(sensorFocus);   
        newSerial.println(focus);
        delay (500);   
      }
   }
   if (iris > 600)
   {
    sendData(0x01, irisOpen, 0x00, 0x00, 0x00);
    while (iris >600)
    {
      iris = analogRead(sensorIris);
    }
   }
   if (iris <450)
   {
    sendData(0x01, irisClose, 0x00, 0x00,  0x00);
    while (iris < 450)
    {
      iris = analogRead(sensorIris);
    }
   }

   stopping();
   
}
