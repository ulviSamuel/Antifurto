  #include "Arduino.h"
  #include "HardwareSerial.h"
  #ifndef view_h
  #define view_h

  #include <Wire.h>
  #include <SPI.h>
  #include <Adafruit_PN532.h>
  #include <LiquidCrystal.h>

  #include "Control.h"
  #include "Timer.h"

  //-----------------------------------------------------------------------------------------------

  #define pir             (8)
  #define ledRed          (2)
  #define ledYellow       (5)
  #define resetButton     (6)
  #define PN532_SCK       (13)
  #define PN532_MOSI      (12)
  #define PN532_SS        (10)
  #define PN532_MISO      (9)
  #define hallSensor      (3)
  #define serviceBuzzer   (7)
  #define alarmBuzzer     (4)
  #define lcdLed          (11)
  #define activDisacPause (15000)
  #define motionTimeRange (30000)
  #define allarmDuration  (180000)
  #define maxMotions      (3)
  
  //-----------------------------------------------------------------------------------------------

  Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
  LiquidCrystal  lcd(A0, A1, A2, A3, A4, A5);

  //-----------------------------------------------------------------------------------------------
  
  class View
  {
      private:
      Control control;
      Timer   timer;
      Timer   timerAlarm;

      //-------------------------------------------------------------------------------------------

      public:
      View()
      {
        control    = Control();
        timer      = Timer();
        timerAlarm = Timer();
        timer.setPause(0);
      }

      //-------------------------------------------------------------------------------------------

      void setup()
      {
        lcd.begin(16, 2);
        pinMode(lcdLed, OUTPUT);
        standby();
        pinMode(ledRed, OUTPUT);
        pinMode(serviceBuzzer, OUTPUT);
        pinMode(alarmBuzzer, OUTPUT);
        pinMode(ledYellow, OUTPUT);
        pinMode(resetButton, INPUT);
        pinMode(hallSensor, INPUT);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledYellow, LOW);
        nfc.begin();
        nfc.setPassiveActivationRetries(5);
        nfc.SAMConfig();
      }

      //-------------------------------------------------------------------------------------------

      void execute()
      {
        readCard();
        motionDetected();
        openedDoor();
        startAlarm();
        resetKeys();
        activeSystemPt2();
      }

      //-------------------------------------------------------------------------------------------
      private:
      void readCard()
      {
        uint8_t keys[] = {0, 0, 0, 0, 0, 0, 0};
        uint8_t keyLength;
        bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, keys, &keyLength);
        if(success)
        {
          String key = "";
          for (uint8_t i=0; i < keyLength; i++) 
            key += (String)keys[i];
          if(control.isMaster(key))
            masterMode();
          else
          {
            if(control.isASlave(key))
              slaveMode();
            else
              storeNewKey(key);
          }
        }
      }

      //------------------------------------------------------------------------------------------- 

      void masterMode()
      {
        if(!control.getSlaveON())
        {
          if(!control.getMasterON() || control.getSystemON())
            keyManager();
          else
            keyManagerCancelled();
        }
      }

      //-------------------------------------------------------------------------------------------

      void slaveMode()
      {
        if(!control.getMasterON())
        {
          if(!control.getSystemON())
          {
            buzz(serviceBuzzer, 4186, 100);
            delay(100);
            buzz(serviceBuzzer, 4186, 100);            
            activeSystem();
          }
          else
          {
            control.setSlaveON(true);
            closeSystem();
          }
        }
      }

      //-------------------------------------------------------------------------------------------

      void keyManager()
      {
        control.setMasterON(true);
        closeSystem();
        lcd.print("CARD MASTER");
        noStandby();
        delay(1500);
        masterMenu();
      }

      //-------------------------------------------------------------------------------------------

      void masterMenu()
      {
        lcd.clear();
        lcd.print("GESTIONE CHIAVI");
        lcd.setCursor(0, 1);
        lcd.print("Chiavi slave:");
        lcd.setCursor(15, 1);
        lcd.print(control.getSlaveCounter());
        control.setMasterFunctON(true);
      }

      //-------------------------------------------------------------------------------------------

      void keyManagerCancelled()
      {
        control.setMasterFunctON(false);
        buzz(serviceBuzzer, 4186, 100);
        delay(100);
        buzz(serviceBuzzer, 4186, 100);
        lcd.setCursor(0, 1);
        lcd.print("Annullata       ");
        delay(3000);
        standby();
        activeSystem();
      }

      //-------------------------------------------------------------------------------------------

      void standby() 
      {
        digitalWrite(lcdLed, LOW);
        lcd.clear();
      }

      //-------------------------------------------------------------------------------------------

      void noStandby() 
      {
        digitalWrite(lcdLed, HIGH);
      }

      //-------------------------------------------------------------------------------------------

      void activeSystem()
      {
        if(!control.getTimerActStmON())
        {
          control.setTimerActStmON(true);
          control.setSystemON(true);
          lcd.print("CHIAVE VALIDA");
          noStandby();
          delay(1500);
          lcd.clear();
          lcd.print("ANTIFURTO");
          lcd.setCursor(0, 1);
          lcd.print("INSERITO...");
          timer.setPause(activDisacPause);
        }
      }

      //-------------------------------------------------------------------------------------------

      void activeSystemPt2()
      {
        if(control.getTimerActStmON() && timer.check())
        {
          control.setTimerActStmON(false);
          buzz(serviceBuzzer, 4186, 100);
          digitalWrite(ledRed, HIGH);
          lcd.clear();
          lcd.print("ANTIFURTO");
          lcd.setCursor(0, 1);
          lcd.print("ATTIVO");
          delay(3000);
          standby();
          control.resetMotionCounter();
          control.setMasterON(false);
          control.setSlaveON(false);
        }
      }      

      //-------------------------------------------------------------------------------------------

      void closeSystem()
      {
        digitalWrite(alarmBuzzer, LOW);
        control.setTimerActStmON(false);
        control.setSystemON(false);
        control.setAlarmON(false);
        control.resetMotionCounter();
        control.setTimerMotDecON(false);
        control.resetRepeatTimCount();
        control.setTimerAllON(false);
        buzz(serviceBuzzer, 4186, 500);
        digitalWrite(ledRed, LOW);
        lcd.clear();
        lcd.print("CHIAVE VALIDA");
        noStandby();
        delay(1500);
        lcd.clear();
        lcd.print("ANTIFURTO");
        lcd.setCursor(0, 1);
        lcd.print("DISATTIVO");
        delay(3000);
        systemActivated();
        standby();
      }

      //-------------------------------------------------------------------------------------------

      void systemActivated()
      {
        if(control.getSystemActived())     
        {
          control.setSystemActived(false);
          lcd.clear();
          lcd.print("ATTENZIONE!");
          lcd.setCursor(0, 1);
          lcd.print("INTRUSIONE");
          delay(5000);
        }   
      }

      //-------------------------------------------------------------------------------------------

      void storeNewKey(String key)
      {
        if (control.getMasterFunctON()) 
        {
          lcd.clear();
          lcd.print("Chiave rilevata!");
          delay(1000);
          control.storeNewKey(key);
          lcd.setCursor(0, 1);
          lcd.print("Chiave salvata!");
          delay(3000);
          masterMenu();
        }
      }

      //-------------------------------------------------------------------------------------------

      void motionDetected()
      {
        if (digitalRead(pir) == HIGH && control.getSystemON() && !control.getAlarmON() && !control.getTimerActStmON())
        {
          if(!control.getTimerMotDecON())
            startMotionDetectedTimer();
          else
            increaseAndCheckMotions();
          if(control.getMotionCounter() >= maxMotions)
          {
            control.setAlarmON(true);
            timer.setPause(activDisacPause);
          }
        }
      }

      //-------------------------------------------------------------------------------------------

      void startMotionDetectedTimer()
      {
        timer.setPause(motionTimeRange);
        control.increaseMotionCounter();
        control.setTimerMotDecON(true);
        digitalWrite(ledYellow, HIGH);
        delay(1000);
        digitalWrite(ledYellow, LOW);
      }

      //-------------------------------------------------------------------------------------------

      void increaseAndCheckMotions()
      {
        control.increaseMotionCounter();
        digitalWrite(ledYellow, HIGH);
        delay(1000);
        digitalWrite(ledYellow, LOW);
        if(timer.check())
        {
          control.resetMotionCounter();
          control.increaseMotionCounter();
          control.setTimerMotDecON(false);
        }
      }

      //-------------------------------------------------------------------------------------------

      void openedDoor()
      {
        if (digitalRead(hallSensor) == LOW && control.getSystemON() && !control.getAlarmON())
        {
           control.setAlarmON(true);
           timer.setPause(activDisacPause);
        }
      }

      //-------------------------------------------------------------------------------------------

      void startAlarm()
      {
        if (control.getSystemON() && control.getAlarmON() && timer.check()) 
        {
          if(!control.getTimerAllON())
          {
            control.setTimerAllON(true);
            lcd.clear();
            lcd.print("ALLARME!!!");
            noStandby();
            timerAlarm.setPrescaler(1024);
            timerAlarm.setPause(allarmDuration/6);
          }
          else
            startAlarmPt2Rele();      
        }
      }

      //-------------------------------------------------------------------------------------------
      
      void startAlarmPt2()
      {
        control.setSystemActived(true);
        buzz(alarmBuzzer, 4186, 100);
        delay(10);
        if(timerAlarm.check())
        {
          control.incrementRepeatTimCount();
          if(control.getRepeatTimCount() == 6)
          {
            control.resetRepeatTimCount();
            control.setAlarmON(false);
            control.setTimerAllON(false);
            standby(); 
          }  
          else
          {
            timerAlarm.setPrescaler(1024);
            timerAlarm.setPause(allarmDuration/6);
          }    
        }
      }

      //-------------------------------------------------------------------------------------------
      
      void startAlarmPt2Rele()
      {
        control.setSystemActived(true);
        digitalWrite(alarmBuzzer, HIGH);
        if(timerAlarm.check())
        {
          control.incrementRepeatTimCount();
          if(control.getRepeatTimCount() == 6)
          {
            control.resetRepeatTimCount();
            control.setAlarmON(false);
            control.setTimerAllON(false);
            digitalWrite(alarmBuzzer, LOW);
            standby(); 
          }  
          else
          {
            timerAlarm.setPrescaler(1024);
            timerAlarm.setPause(allarmDuration/6);
          }    
        }
      }

      //-------------------------------------------------------------------------------------------

      void buzz(int targetPin, long frequency, long length)
      {
        long delayValue = 1000000/frequency/2;
        long numCycles = frequency * length/ 1000;
        for (long i=0; i < numCycles; i++)
        {
          digitalWrite(targetPin,HIGH);
          delayMicroseconds(delayValue);
          digitalWrite(targetPin,LOW);
          delayMicroseconds(delayValue);
        }
      }

      //-------------------------------------------------------------------------------------------

      void resetKeys()
      {
         if (digitalRead(resetButton) == HIGH && control.getMasterFunctON()) 
         {
           control.resetKeys(); 
           lcd.clear();
           lcd.print("Reset chiavi...");
           delay(3000);
           masterMenu();
        }  
      }
  };

  #endif