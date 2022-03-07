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

#include <OneWire.h>  

#include <DallasTemperature.h>
// =================================================================================

// ---  mapeamento de hardware      --- //
#define dados A2 

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

OneWire oneWire(dados);  /*Protocolo OneWire*/

DallasTemperature sensors(&oneWire); /*encaminha referências OneWire para o sensor*/
// --- Protótipo das Funções        --- //
//Mensagem inicial do display.
void inicio_display ();
//Testa leitura sensor.
void teste_sensor_temp ();
// Conversão A/D via registradores extrair o máximo de resolução
int ad_conversor();

// =================================================================================
// --- Configurações Iniciais SETUP --- //
void setup() {
   wdt_disable();       // desativa o dog
   wdt_enable(WDTO_8S); //    /*#define WDTO_15MS,30MS,60MS,120MS,250MS,500MS,1S,2S,4S,8S*/

   Serial.begin(9600);  // inicia a rede RS232
   Serial.println("Nome arquivo:LEITURA-UMIDADE-LCD-I2C-V1...\n");
   lcd.init();          // iniciar display LCD
   inicio_display ();
   sensors.begin(); /*inicia biblioteca*/
   ADCSRA   =  0x93; // ou binário  1001 0011 pagina 262 define resolução
   ADMUX    =  0x43;  // ou binário  0100 0011 // define o canal de entrada
   // exemplo básico de conversão 
   //T = { [(ADCH << 8) | ADCL] - TOS} /k 
   // https://pdf1.alldatasheet.com/datasheet-pdf/view/313656/ATMEL/ATmega328P.html referencia técnica
  
} // end setup
// =================================================================================
// --- Loop Infinito                --- //
void loop() {
   wdt_reset            (     );
   //inicio_display ();
   teste_sensor_temp ();
   delay(500);
   ad_conversor();
   delay(500);
   
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
// =================================================================================
void teste_sensor_temp (){
   sensors.requestTemperatures(); /* Envia o comando para leitura da temperatura */
   lcd.clear();
   lcd.setCursor(0,0);           // configura posição  do cursor
   lcd.print("Leitura:");
   lcd.setCursor(9,0);   
   lcd.print(sensors.getTempCByIndex(0)); 
   lcd.setCursor(14,0);   
   lcd.print("C");
   //delay(500);
}// end teste_sensor_temp
// =================================================================================
int ad_conversor(){
   static int  analogl,analogh,log;
      ADCSRA   |=    (1<<ADSC);           // inicia a conversão!

      while(!(ADCSRA    &=~(1<<ADIF)));   // Aguarda o final da conversão!

      ADCSRA   |=    (1<<ADIF);           // limpa o ADIF na transição LOW /HIGH.

      analogl  =  ADCL;                   // Armaz. o byte menos significativo.

      analogh   =  ADCH;                   // Armaz. o byte mais  significativo.

      log   =  (analogh<<8) | analogl;  // calcula o valor para 10 bits!
     return log;
}// end  int ad_conversor
 
