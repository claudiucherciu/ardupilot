// Ultrasunete.ino

//handy macros 
#define output_low(port,pin) port &= ~(1<<pin) 
#define output_high(port,pin) port |= (1<<pin) 
#define set_input(portdir,pin) portdir &= ~(1<<pin) 
#define set_output(portdir,pin) portdir |= (1<<pin) 
#define read_pin(port, pin) port & (1<<pin) 


void ultrasunete_init() {

  //set TRIG pins as output
  set_output(SENZOR_TRIG_DDR, SENZOR_FATA_TRIG);
  set_output(SENZOR_TRIG_DDR, SENZOR_SPATE_TRIG);
  set_output(SENZOR_TRIG_DDR, SENZOR_STANGA_TRIG);
  set_output(SENZOR_TRIG_DDR, SENZOR_DREAPTA_TRIG);
  //output low all TRIG pins
  SENZOR_TRIG_PORT &= ~((1<<SENZOR_FATA_TRIG) | (1<<SENZOR_SPATE_TRIG) | (1<<SENZOR_STANGA_TRIG) | (1<<SENZOR_DREAPTA_TRIG));

  //enable PCINT for ECHO pins
  PCMSK2 |= ( (1<<SENZOR_FATA_ECHO) | (1<<SENZOR_SPATE_ECHO) | (1<<SENZOR_STANGA_ECHO) | (1<<SENZOR_DREAPTA_ECHO));
  //PCICR  |=   (1<<PCIE2); //enable ISR
}


void ultrasunete_50Hz() {

  static unsigned int state = 0;
  switch(state) {
    case 0:
            //disable ISR to avoid timing updates and race conditions
            PCICR  &=   ~(1<<PCIE2); 
            
            //output high all TRIG pins;
            SENZOR_TRIG_PORT |= ((1<<SENZOR_FATA_TRIG) | (1<<SENZOR_SPATE_TRIG) | (1<<SENZOR_STANGA_TRIG) | (1<<SENZOR_DREAPTA_TRIG));
            
            //Compute last distances
            for(int i = 0; i < 4; i++) {
              unsigned long timp = distante_end_millis[i] - distante_start_millis[i];
              if (timp > 0) {
                timp = timp / 58;
                if ((timp > SENZOR_MIN_RANGE) && (timp < SENZOR_MAX_RANGE)) {
                  timp = distante[i];
                }
              }
            }
            
            state++;
            break;
    case 1: 
            //output low all TRIG pins
            SENZOR_TRIG_PORT &= ~((1<<SENZOR_FATA_TRIG) | (1<<SENZOR_SPATE_TRIG) | (1<<SENZOR_STANGA_TRIG) | (1<<SENZOR_DREAPTA_TRIG));
            
            PCICR  |=   (1<<PCIE2); //enable ISR
            
            state++;
            break;
            
    default: state = 0;
  }  
}


ISR(PCINT2_vect) {
  
  static uint8_t lastStatus = 0;
  unsigned long time = micros();
  
  uint8_t changes = lastStatus ^ SENZOR_ECHO_PIN;
 
  if(changes & (1 << SENZOR_FATA_ECHO)) {  //SENZOR_FATA_ECHO was changed
    if(SENZOR_ECHO_PIN & (1<<SENZOR_FATA_ECHO)) { //pin is now high
        distante_start_millis[0] = time;
    }else{
        distante_end_millis[0] = time;
    }
  }
  
  if(changes & (1 << SENZOR_SPATE_ECHO)) {
    if(SENZOR_ECHO_PIN & (1<<SENZOR_SPATE_ECHO)) {
        distante_start_millis[1] = time;
    }else{
        distante_end_millis[1] = time;
    }
  }
  
  if(changes & (1 << SENZOR_STANGA_ECHO)) {  
    if(SENZOR_ECHO_PIN & (1<<SENZOR_STANGA_ECHO)) { 
        distante_start_millis[2] = time;
    }else{
        distante_end_millis[2] = time;
    }
  }
  
  if(changes & (1 << SENZOR_DREAPTA_ECHO)) {
    if(SENZOR_ECHO_PIN & (1<<SENZOR_DREAPTA_ECHO)) {
        distante_start_millis[3] = time;
    }else{
        distante_end_millis[3] = time;
    }
  }
  
  lastStatus = SENZOR_ECHO_PIN;
}

