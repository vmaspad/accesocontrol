#include <Servo.h>

#include <Key.h>
#include <Keypad.h>

#include <LiquidCrystal.h>

#include <SPI.h>
#include <MFRC522.h>


#define RST_PIN  9    //Pin 9 para el reset del RC522
#define SS_PIN  10   //Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); ///Creamos el objeto para el RC522
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,16,2);
 
const byte rowsCount = 4;
const byte columsCount = 3;
 char password[]="1234";  //Contraseña de 4 dígitos
char clave[4];            //Aqui guardamos los caracteres presionados en el teclado
int conteo=0;   
char keys[rowsCount][columsCount] = {
   { '1','2','3'},
   { '4','5','6'},
   { '7','8','9'},
   { '*','0','#'}
};
const byte rowPins[rowsCount] = { 2, 3, 4, 5 };
const byte columnPins[columsCount] = { A0, 7, 8 };
 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);

Servo servoMotor;

void setup() {
  Serial.begin(9600); //Iniciamos La comunicacion serial
  SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos el MFRC522
  Serial.println("Control de acceso:");
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight
//servo setup
servoMotor.attach(6);
}

bool accessControlCount;

byte ActualUID[4]; //almacenará el código del Tag leído
byte Usuario1[4]= { 0x47, 0xA0, 0x7B, 0x52   } ; //código del usuario 1
byte Usuario2[4]= {0x04, 0x62, 0xCA, 0xA2, } ; //código del usuario 2

void loop() {
char tecla = keypad.getKey(); //se alamacena en la variable "tecla" el caracter presionado

 
  if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
              
                  // Enviamos serialemente su UID
                  Serial.print(F("Card UID:"));
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                          ActualUID[i]=mfrc522.uid.uidByte[i];          
                  } 
                  Serial.print("     ");                 
                  //comparamos los UID para determinar si es uno de nuestros usuarios  
                  if(compareArray(ActualUID,Usuario1) ||clave[0]==password[0]&&clave[1]==password[1]&&clave[2]==password[2]&&clave[3]==password[3]){
                    switch(accessControlCount){
                      case 0:
                        Serial.println("Acceso concedido...");
                        lcd.println("Acceso concedido...");
                        delay(200);
                        lcd.clear();
                        servoMotor.write(90);
                        accessControlCount = 1;
                        break;
                      default:
                        Serial.println("esta en 1");
                        servoMotor.write(0);
                        accessControlCount = 0;
                        break;
                    }
                    
                  }
                  else if(compareArray(ActualUID,Usuario2)||clave[0]==password[0]&&clave[1]==password[1]&&clave[2]==password[2]&&clave[3]==password[3]){
                    Serial.println("Acceso concedido...");
                    lcd.println("Acceso concedido...");
                    delay(3000);
                    lcd.clear();
              
                    servoMotor.write(90);
            } 
                  else{
                    Serial.println("Acceso denegado...");
                     lcd.println("Acceso denegado...");
                      delay(3000);
                       lcd.clear();
                    
                 
                  }
                  // Terminamos la lectura de la tarjeta tarjeta  actual
                  mfrc522.PICC_HaltA();
          
            }
      
  }
 
 
 /* teclado */
  if (tecla != NO_KEY)         //¿Se ha presionado alguna tecla?
  {
    clave[conteo]=tecla;          //Almacenamos caracter por caracter en el arreglo clave[]
    lcd.println(clave[conteo]);    //Se muestra en el puerto serial la tecla presionada
    Serial.println(clave[conteo]);
    lcd.clear();
    
    conteo=conteo+1;              //Se incremente en 1 la variable conteo
    if(conteo==4)  //Si 4 teclas fueron presionadas se verifica si la clave es la correcta o no
    {
      if(clave[0]==password[0]&&clave[1]==password[1]&&clave[2]==password[2]&&clave[3]==password[3])
      {
           Serial.println("Acceso concedido...");
        delay(3000);
        lcd.clear();
        servoMotor.write(0);
        delay(500);
        servoMotor.write(90);
        //Suena un solo bip en el buzzer indicando que la calve es correcta
        //Activamos el módulo relay durante 3 segundos para la apertura de una puerta
        conteo=0;//Seteamos la variable conteo al valor 0
      }
      //Si la clave es incorrecta sonará dos veces el buzzer
      if (conteo!=0){
           lcd.println("Acceso denegado...");
        delay(3000);
        lcd.clear();
      }
      conteo=0;  //Seteamos la variable conteo al valor 0
    }
  }
}

//Función para comparar dos vectores
 boolean compareArray(byte array1[],byte array2[])
{
  if(array1[0] != array2[0])return(false);
  if(array1[1] != array2[1])return(false);
  if(array1[2] != array2[2])return(false);
  if(array1[3] != array2[3])return(false);
  return(true);
}