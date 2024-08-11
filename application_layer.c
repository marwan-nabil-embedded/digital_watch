/* 
 * @File:   application.c
 * @Author: Marwan_Nabil
 * https://www.linkedin.com/in/marwan-nabil-30341b264/
 * digital _watch
 */
#define _XTAL_FREQ 4000000UL
#include"application.h"
Std_ReturnType ret=E_NOT_OK;
void tmr0_handler(void);
void app_intialize(void);
void stop_watch_mode(void);
void write_time_on_7seg(uint8 hours_t,uint8 minuts_t,uint8 seconds_t);
volatile sint8 hours=12,minuts=59,seconds=50,mode=0,base_time=0,mode_var=0
               ,flag=0,logic_val=0,x=0,y=0,z=0,counter=1;  
void increment(void);
void decrement(void);
interrupt_INTx_t up_switch={
  .EXT_InterruptHandler=&increment,
  .edge=INTERRUPT_RISING_EDGE,
  .mcu_pin.port=PORTB_INDEX,
  .mcu_pin.pin=GPIO_PIN1,
  .mcu_pin.direction=GPIO_DIRECTION_INPUT,
  .mcu_pin.logic=GPIO_LOW,
  .source=INTERRUPT_EXTERNAL_INT1,
};
interrupt_INTx_t down_switch={
  .EXT_InterruptHandler=&decrement,
  .edge=INTERRUPT_RISING_EDGE,
  .mcu_pin.port=PORTB_INDEX,
  .mcu_pin.pin=GPIO_PIN2,
  .mcu_pin.direction=GPIO_DIRECTION_INPUT,
  .mcu_pin.logic=GPIO_LOW,
  .source=INTERRUPT_EXTERNAL_INT2,
};
timer0_t tmr0_obj={
        .TMR0_InterruptHandler=tmr0_handler,
        .prescaler_enable=TIMER0_PRESCALER_ENABLE_CFG,
        .prescaler_value=TIMER0_PRESCALER_DIV_BY_16,
        .timer0_mode=TIMER0_TIMER_MODE,
        .timer0_preload_value=3036,
        .timer0_register_size=TIMER0_16BIT_REGISTER_MODE
    };
segment_t seg1={
    .segment_pins[0].port=PORTC_INDEX,
    .segment_pins[0].pin=GPIO_PIN0,
    .segment_pins[0].logic=GPIO_LOW,
    .segment_pins[0].direction=GPIO_DIRECTION_OUTPUT,
    
    .segment_pins[1].port=PORTC_INDEX,
    .segment_pins[1].pin=GPIO_PIN1,
    .segment_pins[1].logic=GPIO_LOW,
    .segment_pins[1].direction=GPIO_DIRECTION_OUTPUT,
    
    .segment_pins[2].port=PORTC_INDEX,
    .segment_pins[2].pin=GPIO_PIN2,
    .segment_pins[2].logic=GPIO_LOW,
    .segment_pins[2].direction=GPIO_DIRECTION_OUTPUT,
    
    .segment_pins[3].port=PORTC_INDEX,
    .segment_pins[3].pin=GPIO_PIN3,
    .segment_pins[3].logic=GPIO_LOW,
    .segment_pins[3].direction=GPIO_DIRECTION_OUTPUT,
    
    .segment_type=SEGMENT_COMMON_CATHODE
  
};
pin_config_t control_1={
    .port=PORTC_INDEX,
    .pin=GPIO_PIN4,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t control_2={
    .port=PORTC_INDEX,
    .pin=GPIO_PIN5,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t control_3={
    .port=PORTC_INDEX,
    .pin=GPIO_PIN6,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t control_4={
    .port=PORTC_INDEX,
    .pin=GPIO_PIN7,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t control_5={
    .port=PORTD_INDEX,
    .pin=GPIO_PIN0,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t control_6={
    .port=PORTD_INDEX,
    .pin=GPIO_PIN1,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};

pin_config_t stop_switch={
    .port=PORTD_INDEX,
    .pin=GPIO_PIN2,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t start_switch={
    .port=PORTD_INDEX,
    .pin=GPIO_PIN3,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};


pin_config_t hours_minuts={
    .port=PORTD_INDEX,
    .pin=GPIO_PIN4,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_INPUT
};

pin_config_t mode12_mode24={
    .port=PORTD_INDEX,
    .pin=GPIO_PIN5,
    .logic=GPIO_LOW,
    .direction=GPIO_DIRECTION_OUTPUT
};
pin_config_t *controls[6]={&control_1,&control_2,&control_3,&control_4,&control_5,&control_6};
pin_config_t *control_switches[3]={&stop_switch,&start_switch,&hours_minuts,&mode12_mode24};

int main(){
    app_intialize();
    while(1){  
        /******** main watch program ********/
        write_time_on_7seg(hours,minuts,seconds);
        /******** stop watch program ******/
        stop_watch_mode();
    }
    return 0;
}

/**
 * @brief : timer interrupt function it happen every 1000ms 
 *          increment seconds , minuts and hours of watch 
 */
void tmr0_handler(void){ 
    base_time++;
    seconds++;
    logic_val=0;
    flag=1;
    ret=gpio_pin_read_logic(&mode12_mode24,&logic_val);
    if(logic_val==GPIO_HIGH){ /* 12 mode */
        if(seconds>=60) {seconds=0;minuts++;}
        if(minuts>=60)  {seconds=0;minuts=0;hours++;}
        if(hours>12)    {seconds=0;minuts=0;hours=1;}
    }    
    else if(logic_val==GPIO_LOW){ /* 24 mode */
        if(seconds>=60) {seconds=0;minuts++;}
        if(minuts>=60)  {seconds=0;minuts=0;hours++;}
        if(hours>24)    {seconds=0;minuts=0;hours=1;}
    }
}
/**
 * 
 * @param hours_t
 * @param minuts_t
 * @param seconds_t
 * @brief : write the watch value on 7segment display by concept of persistence of vision
 */
void write_time_on_7seg(uint8 hours_t,uint8 minuts_t,uint8 seconds_t){
            ret=gpio_pin_write_logic(&control_6,GPIO_HIGH);
            ret=seven_segement_write_number(&seg1,(uint8)(hours_t/10));        
            ret=gpio_pin_write_logic(&control_1,GPIO_LOW);
            
            __delay_us(60);
            
            ret=gpio_pin_write_logic(&control_1,GPIO_HIGH);
            ret=seven_segement_write_number(&seg1,(uint8)(hours_t%10));
            ret=gpio_pin_write_logic(&control_2,GPIO_LOW);
            
            __delay_us(60);
            
            ret=gpio_pin_write_logic(&control_2,GPIO_HIGH);
            ret=seven_segement_write_number(&seg1,(uint8)(minuts_t/10));
            ret=gpio_pin_write_logic(&control_3,GPIO_LOW);
            
            __delay_us(60);
           
            ret=gpio_pin_write_logic(&control_3,GPIO_HIGH);
            ret=seven_segement_write_number(&seg1,(uint8)(minuts_t%10));
            ret=gpio_pin_write_logic(&control_4,GPIO_LOW);
            
            __delay_us(60);
            
            ret=gpio_pin_write_logic(&control_4,GPIO_HIGH);
            ret=seven_segement_write_number(&seg1,(uint8)(seconds_t/10));
            ret=gpio_pin_write_logic(&control_5,GPIO_LOW);
            
            __delay_us(60);
            
            ret=gpio_pin_write_logic(&control_5,GPIO_HIGH);
            ret=seven_segement_write_number(&seg1,(uint8)(seconds_t%10));
            ret=gpio_pin_write_logic(&control_6,GPIO_LOW);

}
/**
 * @brief : initialize seven seg , switches ,timer0 
 */
void app_intialize(void){
        /*seven segment initialization */
        ret=seven_segement_intialize(&seg1);
        for(uint8 i=0;i<6;i++){
            ret=gpio_pin_intialize(controls[i]);
            /*we disable all 7segments*/
            ret=gpio_pin_write_logic(controls[i],GPIO_HIGH);
        }
        /* control_switches initialization */
        for(uint8 y=0;y<4;y++){
            ret=gpio_pin_intialize(control_switches[y]);
        }
        /*timer 0 peripheral initialization */
        ret=Timer0_Init(&(tmr0_obj));
        ret=Interrupt_INTx_Init(&up_switch);
        ret=Interrupt_INTx_Init(&down_switch);
}
void stop_watch_mode(void){
    /******** if start_stop_watch switch is on ********/
    logic_val=0;
    gpio_pin_read_logic(&stop_switch,&logic_val);
        if(logic_val==1){
            x=0;y=0;z=0;
            while(1){
                logic_val=0;
                gpio_pin_read_logic(&start_switch,&logic_val);
                if(logic_val==1){
                    logic_val=0; 
                    break;
                }
                else{
                    write_time_on_7seg(x,y,z);
                    if(flag==1){
                        z++;
                        if(x==12){x=0;y=0;z=0;}
                        else{
                               if(z==60){
                                  y++;
                                  z=0;
                                }
                               if(y==60){
                                  x++;
                                  y=0;
                                }
                        }
                        flag=0;
                    }
                }
            }
        }
    counter=1;
}
void increment(void){
logic_val=0;
ret=gpio_pin_read_logic(&mode12_mode24,&logic_val);
    if(logic_val==GPIO_HIGH){       /* 12 mode hours */
        logic_val=0;
        ret=gpio_pin_read_logic(&hours_minuts,&logic_val);
        if(logic_val==GPIO_HIGH){ /* increment hours */
            if(12<=hours)       {hours=1;}
            else if(hours<12 && hours>=1){hours++;}
            else{/* nothing */}
        }
        else if(logic_val==GPIO_LOW){ /* increment mintus */
            if(minuts>=60)       {hours++;minuts=0;seconds=0;}
            else if(minuts<60 && minuts>=0){minuts++;seconds=0;}
            else{/* nothing */}
        } 
    }
    else if(logic_val==GPIO_LOW){   /* 24 mode hours */
        logic_val=0;
        ret=gpio_pin_read_logic(&hours_minuts,&logic_val);
        if(logic_val==GPIO_HIGH){ /* increment hours */
            if(24<=hours)       {hours=1;}
            else if(hours<24 && hours>=1){hours++;}
            else{/* nothing */}
        }
        else if(logic_val==GPIO_LOW){ /* increment mintus */
            if(minuts>=60)       {hours++;minuts=0;seconds=0;}
            else if(minuts<60 && minuts>=0){minuts++;seconds=0;}
            else{/* nothing */}
        } 
    }
}


void decrement(void){
logic_val=0;
ret=gpio_pin_read_logic(&mode12_mode24,&logic_val);
    if(logic_val==GPIO_HIGH){       /* 12 mode hours */
        logic_val=0;
        ret=gpio_pin_read_logic(&hours_minuts,&logic_val);
        if(logic_val==GPIO_HIGH){ /* decrement hours */
            if(1==hours)       {hours=12;}
            else if(hours<=12 && hours>1){hours--;}
            else{/* nothing */}
        }
        else if(logic_val==GPIO_LOW){ /* decrement mintus */
            if(0==minuts)       {minuts=59;seconds=0;}
            else if(minuts<60 && minuts>0){minuts--;seconds=0;}
            else{/* nothing */}
        } 
    }
    else if(logic_val==GPIO_LOW){   /* 24 mode hours */
        logic_val=0;
        ret=gpio_pin_read_logic(&hours_minuts,&logic_val);
        if(logic_val==GPIO_HIGH){ /* decrement hours */
            if(1==hours)       {hours=24;}
            else if(hours<=24 && hours>1){hours--;}
            else{/* nothing */}
        }
        else if(logic_val==GPIO_LOW){ /* decrement mintus */
            if(0==minuts)       {minuts=59;seconds=0;}
            else if(minuts<60 && minuts>0){minuts--;seconds=0;}
            else{/* nothing */}
        } 
    }
}
