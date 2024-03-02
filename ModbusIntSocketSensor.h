
#ifndef ModbusIntSocketSensor_h
#define ModbusIntSocketSensor_h

#include <string>
#include <sched.h>
#include <unistd.h>


#include "common.h"
#include "globals.h"

#include "utils.h"
#include "Time.h"
#include "Message.h"
#include "Message_Body.h"
#include "SocketSensorModbus.h"


class ModbusTCPFloatSensor
 : public SocketOpenSensorModbusTCP {

public:
    ModbusTCPFloatSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry&);

protected:
    virtual Message* makeMessage( const string&);

};

class ModbusTCPFloatOvenSensor
 : public SocketOpenSensorModbusTCP {

public:
    ModbusTCPFloatOvenSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry&);

protected:
    virtual Message* makeMessage( const string&);

};

class ModbusTCPIntSocketSensor
 : public SocketOpenSensorModbusTCP {

public:
    ModbusTCPIntSocketSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry&);

protected:
    virtual Message* makeMessage( const string&);

};

class ModbusIntSocketSensor
 : public SocketSensorModbus {

public:
    ModbusIntSocketSensor( const string&, const Address&, SocketNetwork*, const SModEntry&);

protected:
    virtual Message* makeMessage( const string&);

};

  class ModbusLongSocketSensor
  : public SocketSensorModbus {

  public:
	  ModbusLongSocketSensor( const string&, const Address&, SocketNetwork*, const SModEntry&);

  protected:
	  virtual Message* makeMessage( const string&);
};


class ModbusIntOpenSocketSensor
 : public SocketOpenSensorModbus {

public:
    ModbusIntOpenSocketSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry& );

protected:
    virtual Message* makeMessage( const string&);
};

class ModbusLongOpenSocketSensor
  : public SocketOpenSensorModbus {

  public:
	  ModbusLongOpenSocketSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry& );

  protected:
	  virtual Message* makeMessage( const string&);
};

  class ModbusFloat64OpenSocketSensor
  : public SocketOpenSensorModbus {

  public:
      ModbusFloat64OpenSocketSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry& );

  protected:
      virtual Message* makeMessage( const string&);
};


  class ModbusFloatOpenSocketSensor
  : public SocketOpenSensorModbus {

  public:
      ModbusFloatOpenSocketSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry& );

  protected:
      virtual Message* makeMessage( const string&);
};

  class ModbusFloatOvenOpenSocketSensor
  : public SocketOpenSensorModbus {

  public:
      ModbusFloatOvenOpenSocketSensor( const string&, const Address&, SocketOpenNetwork*, const SModEntry& );

  protected:
      virtual Message* makeMessage( const string&);
};

#endif
