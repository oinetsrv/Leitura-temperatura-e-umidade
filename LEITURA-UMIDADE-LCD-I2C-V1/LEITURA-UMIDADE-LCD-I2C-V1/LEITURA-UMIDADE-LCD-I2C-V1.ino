/*
 ----------------------------------------------------------
|   PROJETO  : Leitura de Umidade e temperatura.           |
|              Componentes: Display LCD com mod I2C        |
|                           Sensor de temperatura DS18B20  |
|                           Sensor de umidade indefinido   |
|   AUTOR    : STENIO RODRIGUES                            |
|   DATA     : 07/03/2022                                  |
|   OBJETIVO :                                             |
|             Apontar conversão de sensor de umidade       |
|             Apontar leitura bruta do sensor de umidade   |
|             Apontar conversão de sensor de temperatua    |
|             Apontar leitura bruta do sensor de temp.     |
|             Definir escala, precisão, desvio padrão      |
 ----------------------------------------------------------
    // =================================================================================
    // --- Bibliotecas Auxiliares       --- //
    // =================================================================================
    // --- Mapeamento de Hardware       --- //
    // =================================================================================
    // --- Instâncias                   --- //
    // =================================================================================
    // --- Protótipo das Funções        --- //
    // =================================================================================
    // --- Variáveis Globais            --- //
    // =================================================================================
    // --- Interrupções                 --- //
    // =================================================================================
    // --- Configurações Iniciais SETUP --- //
    // =================================================================================
    // --- Loop Infinito                --- //
    // =================================================================================
    // --- Desenvolvimento das Funções  --- //
    // =================================================================================

 */

// --- Bibliotecas Auxiliares       --- //

#include <avr/io.h>  // wachdog

#include <avr/wdt.h> // wachdog

#include <Wire.h>    // rede I2C

#include <LiquidCrystal_I2C.h> // LCD

// =================================================================================

// ---  mapeamento de hardware      --- //
// pino A2 pin 25   Sensor temperatura conversao A/D
// pino A3 pin 26   Sensor umidade     conversao A/D
// pino A4 pin 27   I2C SDA
// pino A5 pin 28   I2c SCL

// =================================================================================
// --- Variáveis Globais            --- //

// =================================================================================
// --- Instâncias                   --- //
//Inicializa o display no endereco 0x27 
// Instancia é criar um objeto baseado em uma classe
LiquidCrystal_I2C lcd(0x27,16,2);

// --- Protótipo das Funções        --- //
//Mensagem inicial do display.
void inicio_display ();

// =================================================================================
// --- Configurações Iniciais SETUP --- //
void setup() {
   wdt_disable();       // desativa o dog
   wdt_enable(WDTO_8S); //    /*#define WDTO_15MS,30MS,60MS,120MS,250MS,500MS,1S,2S,4S,8S*/

   Serial.begin(9600);  // inicia a rede RS232
   Serial.println("Nome arquivo:LEITURA-UMIDADE-LCD-I2C-V1...\n");
   lcd.init();          // iniciar display LCD
   inicio_display ();
  
} // end setup
// =================================================================================
// --- Loop Infinito                --- //
void loop() {
   wdt_reset            (     );
   //inicio_display ();
   
} // end loop
// =================================================================================
// --- Desenvolvimento das Funções  --- //
// =================================================================================
void inicio_display (){
  lcd.setBacklight(HIGH);       // liga luz display
  lcd.setCursor(0,0);           // configura posição  do cursor
  lcd.print("Iniciando!");
  lcd.setCursor(0,1);
  lcd.print("Coletando dados.");
  delay(500);
  lcd.setBacklight(LOW);
  delay(1000);
  lcd.setBacklight(HIGH); 
  //lcd.setBacklight(HIGH);
  //delay(1000);
}//inicio_display 
 
