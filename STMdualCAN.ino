#include <HardwareCAN.h>
#define T_DELAY 10
HardwareCAN canBus(CAN1_BASE);
CanMsg msg ;
byte counter;
//enum {CAN33, CAN95} canswitch;
int canswitch;

void SendCANmessage(long id=0x001, byte dlength=8, byte d0=0x00, byte d1=0x00, byte d2=0x00, byte d3=0x00, byte d4=0x00, byte d5=0x00, byte d6=0x00, byte d7=0x00)
{
  // Initialize the message structure
  // A CAN structure includes the following fields:
  msg.IDE = CAN_ID_STD;          // Indicates a standard identifier ; CAN_ID_EXT would mean this frame uses an extended identifier
  msg.RTR = CAN_RTR_DATA;        // Indicated this is a data frame, as opposed to a remote frame (would then be CAN_RTR_REMOTE)
  msg.ID = id ;                  // Identifier of the frame : 0-2047 (0-0x3ff) for standard idenfiers; 0-0x1fffffff for extended identifiers
  msg.DLC = dlength;                   // Number of data bytes to follow

  // Prepare frame : send something
  msg.Data[0] = d0 ;
  msg.Data[1] = d1 ;
  msg.Data[2] = d2 ;
  msg.Data[3] = d3 ;
  msg.Data[4] = d4 ;
  msg.Data[5] = d5 ;
  msg.Data[6] = d6 ;
  msg.Data[7] = d7 ;

//  digitalWrite(PC13, LOW);    // turn the onboard LED on
  CANsend(&msg) ;      // Send this frame            
//  digitalWrite(PC13, HIGH);   // turn the LED off 
  delay(T_DELAY);  
}

 void CAN_33(void)
{
  CAN_STATUS Stat ;
  canBus.map(CAN_GPIO_PB8_PB9);
  Stat = canBus.begin(CAN_SPEED_33, CAN_MODE_NORMAL);
  canBus.filter(0, 0, 0);
  canBus.set_irq_mode();
  Stat = canBus.status();
  if (Stat != CAN_OK)
  {digitalWrite(PC13, LOW);
   delay(5000);}
 }

void CAN_95(void)
{
  CAN_STATUS Stat ;
  canBus.map(CAN_GPIO_PB8_PB9);
  Stat = canBus.begin(CAN_SPEED_95, CAN_MODE_NORMAL);
  canBus.filter(0, 0, 0);
  canBus.set_irq_mode();
  Stat = canBus.status();
  if (Stat != CAN_OK)
  {digitalWrite(PC13, LOW);
   delay(5000);}
//     /* Your own error processing here */ ;   // Initialization failed
//  delay(T_DELAY);
}

 
 CAN_TX_MBX CANsend(CanMsg *pmsg) 
{
  CAN_TX_MBX mbx;

  do 
  {
    mbx = canBus.send(pmsg) ;
#ifdef USE_MULTITASK
    vTaskDelay( 1 ) ;                 // Infinite loops are not multitasking-friendly
#endif
  }
  while(mbx == CAN_TX_NO_MBX) ;       // Waiting outbound frames will eventually be sent, unless there is a CAN bus failure.
  return mbx ;
}


void handler_pin(void);

byte msgD0 = 0x00;

void setup()
{
	canswitch = 0;
//	pinMode(28,INPUT);
    pinMode(1, OUTPUT);
    pinMode(2, INPUT);
    pinMode(4, OUTPUT);
    pinMode(3, INPUT);
    pinMode(6, OUTPUT);
    pinMode(5, INPUT);

    // Setup pin Timer
    Timer2.setChannel1Mode(TIMER_OUTPUTCOMPARE);
    Timer2.setPeriod(5); // in microseconds (15 is OK for CAN_33, 5 for 95)
    Timer2.setCompare1(1);      // overflow might be small
    Timer2.attachCompare1Interrupt(handler_pin);

	pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH);
  delay(10);
  digitalWrite(PC13, LOW);
  delay(1000);
  digitalWrite(PC13, HIGH);
  delay(1000);
  counter = 0;
}

void a33() 
{
  delay(T_DELAY);
  SendCANmessage(0x033,1,counter);       
  delay(T_DELAY);
}
  
void b95() 
{
  delay(T_DELAY);
  SendCANmessage(0x095,1,counter);       
  delay(T_DELAY);
}

void loop() 
{

	CAN_33();
	canswitch = 0;
	a33();

	delay(T_DELAY);

	CAN_95();
	canswitch = 1;
	b95();

	delay(T_DELAY);
	counter++;
}
  
void handler_pin(void) {
	
switch(canswitch)
	{
	case 0:
    digitalWrite(6, digitalRead(2));
    digitalWrite(1, digitalRead(5));
	break;	
	
	case 1:
    digitalWrite(4, digitalRead(2));
    digitalWrite(1, digitalRead(3));
	break;	
	}
	
} 
  
  
