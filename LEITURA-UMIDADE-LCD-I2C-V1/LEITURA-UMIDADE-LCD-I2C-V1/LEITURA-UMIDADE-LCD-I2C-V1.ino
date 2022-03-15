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
    
   objetivos:
   OK  -  Fazer leitura de sinal anaógico.
   OK  -  Rotina que reconheça estabilidade no sinal entre intervalo de tempo.
   OK  -  Apresentar no display leitura A/D convertida.

   EXTRUTURA DO PROGRAMA!
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
unsigned long timeold = 0;
float ad_tara  = 595    ,
      ad_carga          ,
      ad_offset         ,
      ad_ganho = 0.00   ,
      ad_calc           ,
      f_calib  = 0.2139  , //0.0513  0.2567GANHO SO CONSIDERANDO ULTIMA LEITURA
      ad_aux            ;
int   ler_umidade,                     // marcador usado para sequenciar a leitura
      intervalo_entre_leituras = 1000; // tempo em milisegundos para nova leitura de temperatura e umidade
// =================================================================================
// --- Instâncias                   --- //
//Inicializa o display no endereco 0x27 
// Instancia é criar um objeto baseado em uma classe
// Referente ao display
LiquidCrystal_I2C lcd(0x27,16,2);
// Referente ao sensor de temperatura
OneWire oneWire(dados);  /*Protocolo OneWire*/
// Referente ao sensor de temperatura
DallasTemperature sensors(&oneWire); /*encaminha referências OneWire para o sensor*/
// --- Protótipo das Funções        --- //
// Mensagem inicial do display.
void  inicio_display    ();
// Informa leitura temperatura e umidade
void  display_1         ();
// Conversão A/D via registradores extrair o máximo de resolução
int   ad_umidade        ();
// Converte a temperatura com resolução máxima
float ad_temperatura    ();
// Rotina que coleta a tara.
float leitura_tara      ();
// Rotina que coleta a carga
float leitura_carga     ();
/*    Neste método uma curva polinomial pode ser
      criada através de todos os dados do intervalo ou dos dados bem
      próximos do final. Desta forma, a curva resultante pode ser
      estendida além do intervalo de dados conhecidos.
      https://www.ufsj.edu.br/portal2-repositorio/File/nepomuceno/mn/12MN_Interpola.pdf */
// Função baseada na extrapolação Polinomial básica 
float leitura_umidade   ();
// =================================================================================
// --- Configurações Iniciais SETUP --- //
void setup() {
   wdt_disable(); // desativa o dog
   wdt_enable(WDTO_8S); //    /*#define WDTO_15MS,30MS,60MS,120MS,250MS,500MS,1S,2S,4S,8S*/
   Serial.begin(9600); // inicia a rede RS232
   Serial.println("Nome arquivo:LEITURA-UMIDADE-LCD-I2C-V1...\n");
   lcd.init(); // iniciar display LCD
   inicio_display();
   sensors.begin(); /*inicia biblioteca*/
   ADCSRA = 0x93; // ou binário  1001 0011 pagina 262 define resolução
   ADMUX = 0x43; // ou binário  0100 0011 // define o canal de entrada
   // exemplo básico de conversão 
   //T = { [(ADCH << 8) | ADCL] - TOS} /k 
   // https://pdf1.alldatasheet.com/datasheet-pdf/view/313656/ATMEL/ATmega328P.html pagina 262 referencia técnica
   // bloco que armazena tara
   while (ler_umidade < 3) {
      if (ad_tara == leitura_tara() && ler_umidade <= 4) {
         lcd.setCursor(0, 1);
         lcd.print("               ");
         lcd.setCursor(0, 1);
         lcd.print("Armazenando Tara");
         ler_umidade++;
      } // end 
      delay(250);
      leitura_tara();
      delay(250);
      wdt_reset();

   } // end while
   ler_umidade = 0;
   wdt_reset();
   inicio_display();
   lcd.clear();

} // end setup
// =================================================================================
// --- Loop Infinito                --- //
void loop() {
   wdt_reset(); // SE TRAVAR O PROCESSADOR O DOG RESETA TUDO!!!
   //inicio_display ();

   // intevalo de leitura para comparar delta do sinal 
   // O USO DO MILIS É PARA NÃO PARAR O PROCESSAMENTO!
   if (millis() - timeold >= intervalo_entre_leituras) {
      int long timeold_2 = (millis() - timeold);
      display_1();
      timeold = millis();
   } // end if

   // CRIAR ROTINA DE LEITURA SEM CARGA E COM CARGA

} // end loop
// =================================================================================
// --- Desenvolvimento das Funções  --- //
// =================================================================================
void inicio_display() {
   lcd.setBacklight(HIGH); // liga luz display
   lcd.setCursor(0, 0); // configura posição  do cursor
   lcd.print("Iniciando!");
   lcd.setCursor(0, 1);
   lcd.print("Coletando dados.");
   delay(500);
   lcd.setBacklight(LOW);
   delay(1000);
   lcd.setBacklight(HIGH);
   //lcd.setBacklight(HIGH);
   //delay(1000);
} //inicio_display 
// =================================================================================

void display_1() {
   //lcd.setBacklight(LOW);
   sensors.requestTemperatures(); /* Envia o comando para leitura da temperatura */
   lcd.clear();
   lcd.setCursor(0, 0); // configura posição  do cursor
   lcd.print("Temperatura:");
   lcd.setCursor(12, 0);
   lcd.print(ad_temperatura());
   lcd.setCursor(16, 0);
   lcd.print("C");
   // manipulação segunda linha display
   ad_calc = leitura_umidade();
   lcd.setCursor(0, 1);
   lcd.print("               ");
   lcd.setCursor(0, 1);
   lcd.print("U:");
   lcd.setCursor(2, 1);
   lcd.print(ad_calc);
   lcd.setCursor(8, 1);
   lcd.print("%");
   lcd.setCursor(10, 1); // configura posição  do cursor
   lcd.print("AD:");
   lcd.setCursor(13, 1);
   lcd.print(ad_umidade());
   delay(1000);

} // end display_1
// =================================================================================
float leitura_tara() {
   //ad_tara = 595;
   ad_tara = ad_umidade();
   return ad_tara;
} // end ad_tara
// =================================================================================

float leitura_carga() {
   ad_carga = ad_umidade();
   return ad_carga;
} // end ad_tara
// =================================================================================
float leitura_umidade() {
   ler_umidade = 1;

   while (ler_umidade != 3) {

      // armazena a carga
      if (ad_carga == leitura_carga() && ler_umidade == 1) {
         lcd.setCursor(0, 1);
         lcd.print("               ");
         lcd.setCursor(0, 1);
         lcd.print("CARG estavel");
         lcd.setCursor(13, 1);
         lcd.print(ad_carga);
         delay(1000);
         ler_umidade = 2;
      } else if (ler_umidade == 1) {
         leitura_carga();
      } // end if
      // calculo baseado em amostras
      if (ler_umidade == 2) {
         ad_aux = float(ad_carga) - float(ad_tara);

         // carga
         //ad_calc = ( f_calib * (float)ad_aux - 263.18 );
         // delta
         ad_calc = (f_calib * (float) ad_aux + 9.8417);
         //y =  0.2962x - 165.83     CONSIDERANDO SÓ O VALOR COM CARGA CONVERTIDO EM AD
         //y =  0.2139x + 9.8417      CONSIDERANDO DELTA ENTRE TARA E CARGA

         ler_umidade = 3;
         return ad_calc;
         break;
      }// end if
   }// end while

} // end leitura_umidade
// =================================================================================
float ad_temperatura() {
   float log;
   sensors.requestTemperatures(); /* Envia o comando para leitura da temperatura */
   log = sensors.getTempCByIndex(0);
   return log;
} // end ad_temperatura
// =================================================================================

int ad_umidade() {
   static int analogl, analogh, log;
   ADCSRA |= (1 << ADSC); // inicia a conversão!
   while (!(ADCSRA &= ~(1 << ADIF))); // Aguarda o final da conversão!
   ADCSRA |= (1 << ADIF); // limpa o ADIF na transição LOW /HIGH.
   analogl = ADCL; // Armaz. o byte menos significativo.
   analogh = ADCH; // Armaz. o byte mais  significativo.
   log = (analogh << 8) | analogl; // calcula o valor para 10 bits!
   return log;
} // end  ad_umidade
// =================================================================================