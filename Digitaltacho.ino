/*  
  Microcontroller Projekt - Labor Technische Informatik 1
  HsKa - Winersemster 2019/20
  Verwendeter Controller - Arduino Uno

  Name:     Tobias Hasheider
  IZ-Name:    hato1033
  Matrikelnr.:  71177
  Prjekt:     Digitaltacho mit Schaltanzeige mittels Hall-Sensor
*/


/***************************************____Bibliotheken einbinden____********************************************************************************************************************************************/

#include <U8x8lib.h>      
#include <U8g2lib.h>

/******************************************************************____OLED initialisieren____*****************************************************************************************************************/

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);         

/*************************************____Eine Bitmap f�r die darstellung eines kleinen Drehzahlmessers____********************************************************************************************/
                              
#define Tacho_width 25                            // Breite der Bitmap definieren 
#define Tacho_height 25                           // H�he der Bitmap definieren
static const unsigned char Tacho_bits[] U8X8_PROGMEM = {            // Bitmap Grafik definieren
   0x00, 0xff, 0x01, 0x00, 0xc0, 0x11, 0x07, 0x00, 0x70, 0x10, 0x1c, 0x00,
   0x10, 0x10, 0x10, 0x00, 0x1c, 0x00, 0x70, 0x00, 0x24, 0x00, 0x4c, 0x00,
   0x46, 0x00, 0xce, 0x00, 0x02, 0x00, 0x87, 0x00, 0x03, 0x80, 0x83, 0x01,
   0x01, 0xc0, 0x01, 0x01, 0x01, 0xe0, 0x00, 0x01, 0x01, 0x70, 0x00, 0x01,
   0x0f, 0x38, 0xe0, 0x01, 0x01, 0x18, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x80, 0x00,
   0x00, 0x00, 0xc4, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x70, 0x00,
   0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00 };

/******************************************____Pins definieren____****************************************************************************************************************/

byte led_gruen_1 = 7;       // Pins f�r die LEDs definieren   
byte led_gruen_2 = 6;
byte led_blau = 5;
byte led_rot = 4;
byte led_weiss = 3;
byte sensor = 0;          // Pin f�r den Sensor definieren

/******************************************____Variablen definieren____****************************************************************************************************************/

volatile long startzeit;        // Variable fuer den Beginn der Zeitmessung
volatile long vergangene_zeit;      // Variable fuer die Zeitdifferenz
int rpm;                // Variable fuer die Berechnung der Drehzahl
volatile int counter = 0;       // Ein Counter um die LEDs und Anzeige nach bestimmter Zeit zurueckzusetzen
double V_cm_min, V_m_s, S;        // Variablen fuer die Geschwindigkeitsberechnung
int drehungen;              // Variable fuer die Berechnung der zur�ckgelegten Strecke

/*****************************************____Setup Funktion____*****************************************************************************************************************/

void setup()
{
  pinMode(led_gruen_1, OUTPUT);       //--// 
  digitalWrite(led_gruen_1, LOW);         //
  pinMode(led_gruen_2, OUTPUT);           //
  digitalWrite(led_gruen_2, LOW);         //
  pinMode(led_blau, OUTPUT);              //----> LEDs als Output definieren und ausschalten
  digitalWrite(led_blau, LOW);            //
  pinMode(led_rot, OUTPUT);               //
  digitalWrite(led_rot, LOW);             //
  pinMode(led_weiss, OUTPUT);             //
  digitalWrite(led_weiss, LOW);       //--//
  
  attachInterrupt(sensor, zeit, FALLING);     // Pin fuer Interrupt ausw�hlen
                          // Festlegen welche Funktion bei ausloesen des Interrupts ausgefuehrt werden soll
                          // Falling damit ausloest, sobald Mangnet in die Naehe kommt 

  startzeit = micros();       // einmal anfaengliche Startzeit festlegen, indem mit der micros-Funktion die aktuelle Zeit gemerkt wird

/*****************************************____Display Setup____*****************************************************************************************************************/

  u8g2.begin();
  u8g2.clearDisplay();          // Display leeren
  u8g2.setFont(u8g_font_unifont);     // Standartschrift festlegen
}

/********************************************____Loop Funktion____**************************************************************************************************************/

void loop()
{
  counter++;                // Counter hochzaehlen lassen

  if (vergangene_zeit != 0 && counter <= 10)      // Solange ein Signal kommt (die Zeitdifferenz ungleich Null) und der Counter kleiner 1 ist berechnet wird berechnet                          //
  {                         // Counterwert etwas h�her setzen, damit auch bei niedrigen Drerhzahlen Werte berechnet und somit ausgegeben werden

    rpm = 60000000 / abs(vergangene_zeit);      // Drehzahl berechnen
    V_cm_min = (rpm * 0.2);             // Geschwindigkeit in cm pro Minute berechnen
    V_m_s = abs(V_cm_min) / 6000;         // Geschwindigkeit in Meter pro Sekunde Umrechnen
  }
  else                      // Wenn kein Signal kommt und der Counter nicht resetet wird, sind Geschwindigkeit und Drehzahl Null
  {
    rpm = 0;
    V_m_s = 0.00;
  }

  S = drehungen * 0.2;          // Zurueckgelegte Strecke berechnen

  u8g2.firstPage();         // Funktion f�r die Displayausgabe aufrufen
  do
  {
    draw();
  } while (u8g2.nextPage());

  /*****************************************____Schleifen f�r die Schlatung der Schaltanzeigen-LEDs____*****************************************************************************************************************/

  if (rpm > 200 && counter <= 1)          // Ab bestimmter Drehzahl leuchtet die LED, solange ein regelmaessiges Signal kommt
  {
    digitalWrite(led_gruen_1, HIGH);      // LED anschalten
  }
  else                      // Wenn Drehzahl unter dem Wert LED aus
  {
    digitalWrite(led_gruen_1, LOW);       // LED ausschalten
  }

  if (rpm > 400 && counter <= 1)        //--/
  {                                         //    
    digitalWrite(led_gruen_2, HIGH);        //
  }                                         //
  else                                      //
  {                                         //
    digitalWrite(led_gruen_2, LOW);         //
  }                                         //
                                            //
  if (rpm > 600 && counter <= 1)            //
                                            //
    digitalWrite(led_blau, HIGH);           //
                                            //----> Wie bei led_gruen_1
  else                                      //
  {                                         //
    digitalWrite(led_blau, LOW);            //
  }                                         //
                                            //
  if (rpm > 800 && counter <= 1)            //
  {                                         //
    digitalWrite(led_rot, HIGH);            //
  }                                         //
  else                                      //
  {                                         //  
    digitalWrite(led_rot, LOW);             //
  }                                     //--/ 

  if (rpm > 1000 && counter <= 1)         // Ab diesem Drehzahlwert gibt es eine Schaltwarnung mittels blinkenden LEDs
  {
    digitalWrite(led_weiss, HIGH);        // Durch kurze Pausen zwischen dem an- und ausschalten der LEDs wird ein Blinken erzeugt
    delay(25);                  
    digitalWrite(led_weiss, LOW);
    delay(25);
  }
  else                      // Es blinkt solange der Die Drehzahl groesser ist als der festgelegte Wert
  {
    digitalWrite(led_weiss, LOW);       // LEDs ausschalten
  }
}

/************************************____Interrupt Funktion____**********************************************************************************************************************/

void zeit()
{
  counter = 0;        // Bei jedem Interrupt wird der Counter resetet, damit Werte zur�ckgesetzt und LEDs ausgeschaltet werden
  drehungen++;        // Radumdrehungen dauerhaft hochz�hlen wenn Pin ausgeloest wird um zur�ckgelegten Weg zu berechnen

  vergangene_zeit = startzeit - micros();     // Zeitdifferenz berechnen 
  startzeit = micros();             // neue Startzeit festlegen
}

/*************************************____Funktion f�r die Displayausgabe____*********************************************************************************************************************/

void draw()
{
  if (rpm < 950)                // Solange die Drehzahl kleiner ist als der Wert -> normale Tachoanzeige ausgeben
  {
    u8g2.drawXBMP(102, 1, Tacho_width, Tacho_height, Tacho_bits);     // kleines Tacho-Symbol anzeigen (Bitmap)

    u8g2.setFont(u8g_font_unifont);             // Schriftart f�r Tachoanzeige festlegen
    u8g2.setCursor(45, 10);                 // Auswaehlen, wo Text angezeigt werden soll
    u8g2.print("TACHO");                  // Ueberschrifttext ausgeben
    u8g2.drawLine(44, 12, 84, 12);              // Ueberschrift unterstreichen

/*********************************************____Tachowerte____***********************************************************************************************************************/

    u8g2.setCursor(0, 29);                  // Ausw�hlen, wo Text angezeigt werden soll
    u8g2.print("RPM: ");                  // Anzeigen, welcher Wert 
    u8g2.println(rpm);                    // Wert ausgeben
    u8g2.print(" U/min");                 // Einheit


    u8g2.setCursor(0, 44);                //--/
    u8g2.print("Speed: ");                    //
    u8g2.println(V_m_s);                      //
    u8g2.print(" m/s");                       //
                                              //----> Wie bei RPM
    u8g2.setCursor(0, 62);                    //
    u8g2.print("Weg: ");                      //
    u8g2.println(S);                          //
    u8g2.print(" m");                     //--/
  }
  else                    // Wenn Drehzahl ueber Schwellwert -> Schaltaufvorderung wird angezeit (kurz bevor LEDs blinken) 
  {
    u8g2.setFont(u8g2_font_logisoso22_tf);          // Neue Schriftart festlegen
    u8g2.drawFrame(0, 0, 128, 64);              // Kasten am Displayrand erzeugen
    u8g2.setCursor(1, 44);
    u8g2.print("SCHALTEN!");
  }
}
