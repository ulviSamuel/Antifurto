  #ifndef master_h
  #define master_h

  class Master
  {
    private:
    const static String masterKey;

    //---------------------------------------------------------------------------------------------

    public:
    String getMasterKey()
    {
      return masterKey;
    }
  };

  //-----------------------------------------------------------------------------------------------

  const String Master::masterKey = "242245225";

  //-----------------------------------------------------------------------------------------------

  #endif