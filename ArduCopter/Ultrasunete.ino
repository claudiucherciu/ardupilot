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
            //disable ISR to avoid timing updates
            PCICR  &=   ~(1<<PCIE2); 
            
            //output high all TRIG pins;
            SENZOR_TRIG_PORT |= ((1<<SENZOR_FATA_TRIG) | (1<<SENZOR_SPATE_TRIG) | (1<<SENZOR_STANGA_TRIG) | (1<<SENZOR_DREAPTA_TRIG));
            
            //Compute last distances
            for(int i = 0; i < 4; i++) {
              
              if (distante_end_millis[i] > distante_start_millis) {
                unsigned long dist = (distante_end_millis[i] - distante_start_millis) / 58;       //sound speed for round-trip travel
                
                if ((dist > SENZOR_MIN_RANGE) && (dist < SENZOR_MAX_RANGE)) {
                  distante[i] = dist;
                  if(i == 0) {
                    senzorStart = distante_start_millis;
                    senzorStop  = distante_end_millis[i];
                  }
                }
              }
            }
            
            state++;
            break;
    case 1: 
            distante_start_millis = micros() + SENZOR_DELAY_RESPONSE;
            //output low all TRIG pins
            SENZOR_TRIG_PORT &= ~((1<<SENZOR_FATA_TRIG) | (1<<SENZOR_SPATE_TRIG) | (1<<SENZOR_STANGA_TRIG) | (1<<SENZOR_DREAPTA_TRIG));
            PCICR  |=   (1<<PCIE2); //enable ISR
            sei();
            
            state++;
            break;
    
    //wait between pulses
    case 2:
    case 3:
            state++;   
            break;
    default: state = 0;
  }  
}


ISR(PCINT2_vect) {
  uint8_t port = SENZOR_ECHO_PIN;
  sei();  //reenable interrupts, we saved what was critical
  
  unsigned long time = micros();
  
  //Check if any pin is now LOW, and log the time
  if((port & (1<<SENZOR_FATA_ECHO)) == 0) { 
      distante_end_millis[0] = time; 
  }
  
  if((port & (1<<SENZOR_SPATE_ECHO)) == 0) {
      distante_end_millis[1] = time;
  }
  
  if((port & (1<<SENZOR_STANGA_ECHO)) == 0) { 
      distante_end_millis[2] = time;
  }
  
  if((port & (1<<SENZOR_DREAPTA_ECHO)) == 0) {
      distante_end_millis[3] = time;
  }
  
}

