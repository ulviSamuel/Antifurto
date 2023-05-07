#ifndef timer_h
#define timer_h

class Timer 
{
  private:
  int           time;
  unsigned long t1;
  unsigned long dt;

  //-----------------------------------------------------------------------------------------------
  
  public:
  Timer() 
  {
    time   = 0;
    t1     = 0;
    dt     = 0; 
  }
  
  //-----------------------------------------------------------------------------------------------

  void setPause(int nSeconds)
  {   
    time   = nSeconds;
    t1     = millis();
  }

  //-----------------------------------------------------------------------------------------------

  bool check()
  {
    bool ret = false;
    dt       = millis() - t1;
    if (dt >= time)
      ret = true;
    return ret;
  }

  //-----------------------------------------------------------------------------------------------

  void setPrescaler(int prescaler)
    {
      TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
      switch(prescaler)
      {
        case 1:
          TCCR1B |= (1 << CS10);
          break;
        case 8:
          TCCR1B |= (1 << CS11);
          break;
        case 64:
          TCCR1B |= (1 << CS11) | (1 << CS10);
          break;
        case 256:
          TCCR1B |= (1 << CS12);
          break;
        case 1024:
          TCCR1B |= (1 << CS12) | (1 << CS10);
          break;
      }
    }
};

#endif