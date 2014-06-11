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
  output_low(SENZOR_TRIG_PORT, SENZOR_FATA_TRIG);
  output_low(SENZOR_TRIG_PORT, SENZOR_SPATE_TRIG);
  output_low(SENZOR_TRIG_PORT, SENZOR_STANGA_TRIG);
  output_low(SENZOR_TRIG_PORT, SENZOR_DREAPTA_TRIG);
  
  sei();
}


void ultrasunete_50Hz() {

  static unsigned int state = 0;
  
  
  if(state % 2 == 0) {  //send TRIG on even numbers
                
    //output high TRIG pin;
    switch(state/2) {
      case 0:
        SENZOR_TRIG_PORT |= (1<<SENZOR_FATA_TRIG);
        break;
      case 1:
        SENZOR_TRIG_PORT |= (1<<SENZOR_SPATE_TRIG);
        break;
      case 2:
        SENZOR_TRIG_PORT |= (1<<SENZOR_STANGA_TRIG);
        break;
      case 3:
        SENZOR_TRIG_PORT |= (1<<SENZOR_DREAPTA_TRIG);
        break;
    }
    
    state++;
            
  }else{    //read ECHO on odd numbers

    //mark the ECHO start moment
    distante_start_millis = micros() + SENZOR_DELAY_RESPONSE;
    
    //output low TRIG pin
    SENZOR_TRIG_PORT &= ~((1<<SENZOR_FATA_TRIG) | (1<<SENZOR_SPATE_TRIG) | (1<<SENZOR_STANGA_TRIG) | (1<<SENZOR_DREAPTA_TRIG));
    
    //enable PCINT for ECHO pins
    switch((state-1) /2) {
      case 0:
        PCMSK2 |= (1<<SENZOR_FATA_ECHO);
        break;
      case 1:
        PCMSK2 |= (1<<SENZOR_SPATE_ECHO);
        break;
      case 2:
        PCMSK2 |= (1<<SENZOR_STANGA_ECHO);
        break;
      case 3:
        PCMSK2 |= (1<<SENZOR_DREAPTA_ECHO);
        break;
    }
    
    PCICR  |= (1<<PCIE2); //enable ISR
    
    
    state++;
    
  }
  
  if (state > 7) { 
    state = 0;
    compute_distances(); //update all sensor distances
  }
}


void compute_distances() {
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
}//compute_distances()

ISR(PCINT2_vect) {
  uint8_t port = SENZOR_ECHO_PIN;
  //sei();  //reenable interrupts, we saved what was critical
  
  unsigned long time = micros();
  
  //Check if any pin is now LOW, and mark the time
  if((port & (1<<SENZOR_FATA_ECHO)) == 0) { 
      distante_end_millis[0] = time;
      PCMSK2 &= ~(1<<SENZOR_FATA_ECHO); //disable PCINT on the pin
  }
  
  if((port & (1<<SENZOR_SPATE_ECHO)) == 0) { 
      distante_end_millis[1] = time;
      PCMSK2 &= ~(1<<SENZOR_SPATE_ECHO);
  }
  
  if((port & (1<<SENZOR_STANGA_ECHO)) == 0) { 
      distante_end_millis[2] = time;
      PCMSK2 &= ~(1<<SENZOR_STANGA_ECHO);
  }
  
  if((port & (1<<SENZOR_DREAPTA_ECHO)) == 0) { 
      distante_end_millis[3] = time;
      PCMSK2 &= ~(1<<SENZOR_DREAPTA_ECHO);
  }
  
}

