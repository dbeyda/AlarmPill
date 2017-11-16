#include <LiquidCrystal.h>

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


int ALARMSOUNDER = 15; //pino do alarme
int led1 = 19;
int led2 = 18;
int led3 = 17;
int led4 = 16;
int minuto = 8*60; // minutos totais do dia atual
int minuto_alarme[4] = {8*60+1,12*60,12*60,12*60}; //vetor com os minutos dos alarmes para tela de ajuste
int minuto_alarme_real[4] = {8*60+1,12*60,12*60,12*60}; //vetor com os minutos dos alarmes reais
int alarme_onoff[4] = {1, 0, 0, 0}; // alarmes desligados(0) / ligados(1)
unsigned long hora_alarme_atual[4], minuto_alarme_atual[4]; // vetor com a hora e minuto de cada alarme, para exibição na tela
int lcd_key = 0, adc_key_in = 0, entradaSerial; 
int modo = 0; // 0 = padrao, 1 = atualiza hora, 2 = escolhe alarme, 3 = atualiza hora do alarme escolhido, 4 = atualiza on/off, 5 = alarme tocando
int n_alarme = 1; // compartimento (1 a 4)
int n_alarme_tocando = 0; //n do alarme que esta tocando
unsigned long horaatual, minutoatual,previusMillis=0,currentMillis;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7 );  // configuração de pinos do LCD


void loop() {
  AtualizaRelogio();
  ChecaCondicoesOnOff();
  delay(250);
  ChecaTeclado();
  atualizadisplay();
  delay(250);
  ChecaLed();
  ChecaBuzzer();
  Serial.print("\n");
  Serial.print(modo);
  Serial.print("\n");
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(ALARMSOUNDER, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}

void DesligaTodos() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}

void ChecaLed() {
Serial.print("checaLED");
  if(modo == 5 and n_alarme_tocando != 0) {
    Serial.print("IF");
    switch (n_alarme_tocando) {
      case 1: {
        digitalWrite(led1, HIGH);
        break;
      }
      case 2: {
        digitalWrite(led2, HIGH);
        break;
      }
      case 3: {
        digitalWrite(led3, HIGH);
        break;
      }
      case 4: {
        digitalWrite(led4, HIGH);
        break;
      }
    }
  }
}

void ChecaTeclado() {
  lcd_key = read_LCD_buttons();
        if (modo == 0) {
          switch(lcd_key)  {
            case btnSELECT:  {
              modo++;
              Serial.print(modo);
              break;
            }
          }
        }
        else if (modo == 1) {
          // atualiza hora
          switch (lcd_key) {
                case btnRIGHT: {
              minuto = minuto + 60;
              atualizadisplay();
          break;
    }
                case btnLEFT:  {
                  minuto = minuto - 60;
                  atualizadisplay();
                  break;
                }
                case btnUP:  {
                  minuto++;
                  atualizadisplay();
                  break;
                }
                case btnDOWN:  {
                  minuto--;
                  atualizadisplay();
                  break;
                }
                case btnSELECT:  {
                  modo++;
                  break;
                }
            }
        }
        
        else if (modo == 2)  {
          //escolhe numero do alarme
          switch (lcd_key)  {
            case btnUP:  {
              n_alarme++;
              if (n_alarme == 5)  {
                n_alarme = 1;
              }
              atualizadisplay();
             break;
            }
            
            case btnDOWN:  {
              n_alarme--;
              if (n_alarme == 0)  {
                n_alarme = 4;
              }
              atualizadisplay();
              break;
            }
            
            case btnSELECT:  {
              modo++;
              break;
            }
          }
        }
        
        else if (modo == 3)  {
        // atualiza hora do alarme
          switch (lcd_key) {
                case btnRIGHT: {
              minuto_alarme[n_alarme-1] = minuto_alarme[n_alarme-1] + 60;
              atualizadisplay();
          break;
    }
                case btnLEFT:  {
          minuto_alarme[n_alarme-1] = minuto_alarme[n_alarme-1] - 60;
                  atualizadisplay();
                  break;
                }
                case btnUP:  {
                  minuto_alarme[n_alarme-1]++;
                  atualizadisplay();
                  break;
                }
                case btnDOWN:  {
                  minuto_alarme[n_alarme-1]--;
                  atualizadisplay();
                  break;
                }
                case btnSELECT:  {
                  minuto_alarme_real[n_alarme-1] = minuto_alarme[n_alarme-1];
                  modo++;
                  break;

                }
            }
        }
        
        else if (modo == 4)  {
          //escolhe on off
          switch (lcd_key)  {
            case btnUP:  {
              if (alarme_onoff[n_alarme-1] == 0) alarme_onoff[n_alarme-1] = 1;
              else if (alarme_onoff[n_alarme-1] == 1) alarme_onoff[n_alarme-1] = 0; 
              atualizadisplay();
             break;
            }
            
            case btnDOWN:  {
              if (alarme_onoff[n_alarme-1] == 0) alarme_onoff[n_alarme-1] = 1;
              else if(alarme_onoff[n_alarme-1] == 1) alarme_onoff[n_alarme-1] = 0; 
              atualizadisplay();
             break;
            }
            
            case btnSELECT:  {
                  modo = 0;
                  n_alarme = 1;
                  break;
            }
          }
        }
          

        else if (modo == 5) {
          switch(lcd_key) {
            case btnSELECT: {
                                alarme_onoff[n_alarme_tocando-1] = 0;
                                n_alarme_tocando = 0;
                                modo = 0;
                                DesligaTodos();
              break;
            }
          }
        }
}

void ChecaBuzzer() {
  if (modo == 5) {
    int i;
    for(i = 0; i < 255; i = i + 10) {
      analogWrite(ALARMSOUNDER, i);
      delay(50);
    }

    for(i = 255; i > 1; i = i - 10) {
      analogWrite(ALARMSOUNDER, i);
      delay(50);
    }
  }
  else
  {
      analogWrite(ALARMSOUNDER, 0);
  }   
}

void ChecaLeds()  {
  if (modo == 5 && n_alarme_tocando != 0) {

  }

}

void ChecaCondicoesOnOff() {
  int i;
  Serial.print("\nminuto_alarme :");
  Serial.print(minuto_alarme[0]);
  Serial.print("       minuto_alarme_real :");
  Serial.print(minuto_alarme_real[0]);        
  Serial.print("      minuto :");
  Serial.print(minuto);
  Serial.print("      alarme_onoff: " );
  Serial.print(alarme_onoff[0]);        
  for(i = 0; i<4; i++) {
                if ((minuto_alarme_real[i] == minuto) && (alarme_onoff[i] == 1))  {
      modo = 5;
                        n_alarme_tocando = i+1;
      break;
    }
  }
}

void AtualizaRelogio() {
  unsigned long currentMillis = millis(); 
  if (previusMillis > currentMillis) { 
    unsigned long previusMillis=0;
  }
  if ( (currentMillis-previusMillis) > (60000) ) { 
    minuto++;    
    previusMillis=currentMillis;
  }
  horaatual = minuto / 60;
  minutoatual = minuto - ( horaatual * 60 );
  if (horaatual>=24) {
    horaatual = 0;
    minuto = minutoatual;
  }
  int i;
  for(i = 0;i<4;i++)  {
    hora_alarme_atual[i] = minuto_alarme[i] / 60;
    minuto_alarme_atual[i] = minuto_alarme[i] - ( hora_alarme_atual[i] * 60);
    if (hora_alarme_atual[i] >= 24) {
      hora_alarme_atual[i] = 0;
      minuto_alarme[i] = (int)minuto_alarme_atual[i];

    }
  }
}

void atualizadisplay() {
  switch (modo) {
    case 0: {
      lcd.clear();

      lcd.print("AlarmPill");
                        lcd.setCursor(15, 0);
      lcd.setCursor(0,1);
      lcd.print("David e Henrique");
      lcd.setCursor(9,0);
      if (horaatual<10) { 
        lcd.print(" ");
      }
      lcd.print(horaatual);
      lcd.print(":");
      if (minutoatual<10) 
        { lcd.print("0");
      }
      lcd.print(minutoatual);
      lcd.print(" ");
      lcd.setCursor(5,1);
      lcd.setCursor(13,1);
      break;
    }
    case 1: {
      lcd.clear();

      lcd.print("Acertar Hora");
      lcd.setCursor(0,1);
      lcd.print("Hora:");
      if (horaatual<10) 
        { lcd.print(" ");
      }
      lcd.print(horaatual);
      lcd.print(":");
      if (minutoatual<10) { 
        lcd.print("0");
      }
      lcd.print(minutoatual);
      lcd.print(" ");
      break;
    }
    case 2: {
      lcd.clear();

      lcd.print("Num do alarme");
      lcd.setCursor(0,1);
      lcd.print("N:");
      lcd.print(n_alarme);
      break;
    }
    case 3: {
      lcd.clear();

      lcd.print("Acertar Alarme ");
      lcd.print(n_alarme);
      lcd.setCursor(0,1);
      lcd.print("Hora:");
      if (hora_alarme_atual[n_alarme-1]<10) 
        { lcd.print(" ");
      }
      lcd.print(hora_alarme_atual[n_alarme-1]);
      lcd.print(":");
      if (minuto_alarme_atual[n_alarme-1]<10) { 
        lcd.print("0");
      }
      lcd.print(minuto_alarme_atual[n_alarme-1]);
      lcd.print(" ");
      break;
    }
    
                case 4:  {
                        lcd.clear();

      lcd.print("Status do alarme");
      lcd.setCursor(7,1);
      if (alarme_onoff[n_alarme-1] == 0) lcd.print("Off");
                        if (alarme_onoff[n_alarme-1] == 1) lcd.print("On");
      break;
    }
                  
                case 5: {
      lcd.clear();

      lcd.print("ITS PILL TIME!!!");
      lcd.setCursor(0, 1);
      lcd.print("Caixa num ");
      lcd.print(n_alarme_tocando);
      break;
    }
  }
}


int read_LCD_buttons() {
  adc_key_in = analogRead(0);      // read the value from the sensor 
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; 
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 195)  return btnUP; 
  if (adc_key_in < 380)  return btnDOWN; 
  if (adc_key_in < 555)  return btnLEFT; 
  if (adc_key_in < 790)  return btnSELECT;   
  return btnNONE;  // when all others fail, return this...
}

