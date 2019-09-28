#include <DHT.h>
#include <LiquidCrystal.h>

#define pinSensorTemperatura 8
#define DHTTYPE DHT22


DHT sensorDHT(pinSensorTemperatura, DHTTYPE);

LiquidCrystal lcd (2,3,4,5,6,7);
float total = 0.0;
char buf1[17];

typedef struct TIME{
  
  int _1ms; //1milisegundo
  int _1seg;  //1segundo

}Time;

Time time;

// Variavel contadora
int cont = 0;
int contAmosta = 0;

float amostras [300]; 

ISR(TIMER1_OVF_vect){
  time._1ms = 1;
  cont++;
  if (cont == 200){
    time._1seg = 1;
    cont = 0;
  }
  TCNT1 = 0xFF05;
}
  
void setup()
{
    // Inicializando Timer1 - Configurado para 1ms (overflow)
  TCCR1A = 0b00000000;
  TCCR1B = 0b00000011; //Prescaler 1:64
   
  TCNT1 = 0xFF05; // 0d65258 - onde começa a contar

  // Habilitar Timer TOTE1 -> Timer Overflow Interrupt Enable 1
  TIMSK1 = TIMSK1 || 0b00000001; // OR - Para manter a configuração anterior
  
  lcd.begin(16,2);
  sensorDHT.begin();
  Serial.begin(9600);
}

void loop()
{

    if (time._1seg){
    amostras[contAmosta] = sensorDHT.readTemperature();
    total+=amostras[contAmosta]; //VARIÁVEL LOCAL QUE ARMAZENA A TEMPERATURA EM GRAUS CELSIUS
    time._1seg = 0;
    Serial.print("Amostra nº ");
    Serial.print(contAmosta);
    Serial.print(": ");
    Serial.println(amostras[contAmosta]);
    contAmosta++;
    
  }
  
  if (contAmosta == 300){
    
    char str_temp[6];
    float media = total/contAmosta;
    dtostrf(media, 4, 2, str_temp);
    sprintf(buf1,"%s", str_temp);
    
    lcd.home();
    lcd.print("Temperatura:");
    lcd.setCursor(0,1);
    lcd.print(buf1);
    contAmosta = 0;
    total = 0.0;
  }
}
