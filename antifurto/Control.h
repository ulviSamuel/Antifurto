  #ifndef control_h
  #define control_h

  #include <EEPROM.h>

  #include "Master.h"

  //-----------------------------------------------------------------------------------------------

  #define maxKeyNum (5)

  //-----------------------------------------------------------------------------------------------

  class Control
  {
    private:
    Master        master;
    unsigned long previousMillis;  
    unsigned long interval;
    bool          systemON;
    bool          alarmON;
    bool          masterON;
    bool          slaveON;
    bool          timerActStmON;
    bool          timerMotDecON;
    bool          timerAllON;
    bool          systemActived;
    bool          masterFunctON;
    int           ledState;
    int           slaveCounter;
    int           motionCounter;
    int           repeatTimCount;

    //---------------------------------------------------------------------------------------------

   public:
   Control()
   {
     master         = Master();
     previousMillis = 0;
     interval       = 0;
     systemON       = true;
     alarmON        = false;
     masterON       = false;
     slaveON        = false;
     timerActStmON  = false;
     timerAllON     = false;
     timerMotDecON  = false;
     systemActived  = false;
     masterFunctON  = false;
     ledState       = 0;
     repeatTimCount = 0;
     slaveCounter   = (int)EEPROM.read(0);
   }

   //----------------------------------------------------------------------------------------------

   bool getMasterON()
   {
     return masterON;
   }

   bool getSlaveON()
   {
     return slaveON;
   }

   int getSlaveCounter()
   {
     return slaveCounter;
   }

   bool getAlarmON()
   {
     return alarmON;
   }

   bool getSystemON()
   {
     return systemON;
   }

   bool getTimerActStmON()
   {
     return timerActStmON;
   }
   
   bool getTimerAllON()
   {
     return timerAllON;
   }

   bool getTimerMotDecON()
   {
     return timerMotDecON;
   }

   bool getSystemActived()
   {
     return systemActived;
   }

   bool getMasterFunctON()
   {
     return masterFunctON;
   }

   int getLedState()
   {
     return ledState;
   }
   
   int getMotionCounter()
   {
     return motionCounter;
   }

   int getRepeatTimCount()
   {
     return repeatTimCount;
   }



   void setMasterON(bool newMasterON)
   {
     masterON = newMasterON;
   }

   void setSlaveON(bool newSlaveON)
   {
     slaveON = newSlaveON;
   }

   void setAlarmON(bool newAlarmON)
   {
     alarmON = newAlarmON;
   }

   void setSystemON(bool newSystemON)
   {
     systemON = newSystemON;
   }

   void setTimerActStmON(bool newTimerActStmON)
   {
     timerActStmON = newTimerActStmON;
   }
   
   void setTimerAllON(bool newTimerAllON)
   {
     timerAllON = newTimerAllON;
   }

   void setTimerMotDecON(bool newTimerMotDecON)
   {
     timerMotDecON = newTimerMotDecON;
   }

   void setSystemActived(bool newSystemActived)
   {
     systemActived = newSystemActived;
   }

   void setMasterFunctON(bool newMasterFunctON)
   {
     masterFunctON = newMasterFunctON;
   }

   //----------------------------------------------------------------------------------------------

   void resetMotionCounter()
   {
     motionCounter = 0;
   }

   //----------------------------------------------------------------------------------------------

   void increaseLedStates()
   {
     ledState ^= 1;
   }

   //----------------------------------------------------------------------------------------------

   void incrementRepeatTimCount()
   {
     ++repeatTimCount;
   }

   //----------------------------------------------------------------------------------------------

   void resetRepeatTimCount()
   {
     repeatTimCount = 0;
   }

   //----------------------------------------------------------------------------------------------

   void increaseMotionCounter()
   {
     ++motionCounter;
   }

   //----------------------------------------------------------------------------------------------

   bool isMaster(String key)
   {
      return (key == master.getMasterKey()) ? true : false;
   }

   //----------------------------------------------------------------------------------------------

   bool isASlave(String key)
   {
     bool isSlave;
     if(slaveCounter != 0)
     {
       char strtoArray[10];
       key.toCharArray(strtoArray, 10);   
       for(int idx = 1; idx < 9 * slaveCounter + 1; idx = idx + 9)
       {
         isSlave = true;
         for(int jdx = 0 + idx; jdx < 9 + idx; ++jdx)
         {
           if((char)EEPROM.read(jdx) != strtoArray[jdx - idx])
           {
             isSlave = false;
             break;
           }
         }
         if(isSlave)
          break;         
       }
     }
     return isSlave;
   }

   //----------------------------------------------------------------------------------------------

   void storeNewKey(String key)
   {
     if(slaveCounter < maxKeyNum)
     {
       char strtoArray[10];
       key.toCharArray(strtoArray, 10);
       for(int idx = 9 * slaveCounter + 1; idx < (9 * slaveCounter + 1) + 9; ++idx)
        EEPROM.write(idx, strtoArray[idx - (9 * slaveCounter + 1)]);
       ++slaveCounter;
       EEPROM.write(0, slaveCounter);
     }
   }

   //----------------------------------------------------------------------------------------------

   void resetKeys()
   {
     for(int idx = 0; idx < 9 * slaveCounter + 1; ++idx)
     {
       EEPROM.write(0, 0);
     }
    slaveCounter = 0;
   }
  };

  #endif