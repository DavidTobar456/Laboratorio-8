/*
 * File:   poslab8.c
 * Author: david
 *
 * Created on 6 de octubre de 2021, 09:07 PM
 */


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 4000000

void config_io(void);
void config_osc(void);
void config_int(void);
void config_adcon(void);
void config_tmr0(void);
uint16_t mapear_valor(uint16_t valor);
void separar_valor(uint16_t valor);
uint8_t valor_a_display(uint8_t valor);

uint8_t disp_most, v2, centenas, decenas, unidades, disp1, disp2, disp3;
uint16_t vmapeado;

void __interrupt() isr(){
    if(PIR1bits.ADIF){
        if(ADCON0bits.CHS==0b1100){
            PORTA=ADRESH;
            ADCON0bits.CHS=0b1010;
        }else if(ADCON0bits.CHS==0b1010){
            v2=ADRESH;
            ADCON0bits.CHS=0b1100;
        }
        PIR1bits.ADIF=0;
    }
    if(INTCONbits.T0IF){
        disp_most++;
        if(disp_most>2){
            disp_most = 0;
        }
        INTCONbits.T0IF=0;
        TMR0=100;
        switch(disp_most){
            case 0:
                PORTE = 0b001;
                PORTC = disp1 + 0b10000000;
                break;
            case 1:
                PORTE = 0b010;
                PORTC = disp2;
                break;
            case 2:
                PORTE = 0b100;
                PORTC = disp3;
                break;
            default:
                PORTE = 0;
                PORTC = 0;
                break;
        }
    }
}

void main(void) {
    config_io();
    config_osc();
    config_int();
    config_adcon();
    config_tmr0();
    disp_most = 0;
    while(1){
        if(!ADCON0bits.GO){
            __delay_us(50);
            ADCON0bits.GO=1;
        }
        vmapeado = mapear_valor(v2);
        separar_valor(vmapeado);
        disp1 = valor_a_display(centenas);
        disp2 = valor_a_display(decenas);
        disp3 = valor_a_display(unidades);
        
    }
}

void config_io(void){
    ANSEL=0;
    ANSELHbits.ANS12=1;
    ANSELHbits.ANS10=1;
    
    TRISA=0;
    TRISC=0;
    TRISE=0;
    TRISBbits.TRISB0=1;
    TRISBbits.TRISB1=1;
    
    PORTA=0;
    PORTC=0;
    PORTE=0;
    return;
}

void config_osc(void){
    OSCCONbits.SCS=1;
    OSCCONbits.IRCF=0b110;
    return;
}

void config_int(void){
    INTCONbits.GIE=1;
    INTCONbits.PEIE=1;
    INTCONbits.T0IE=1;
    INTCONbits.T0IF=0;
    
    PIE1bits.ADIE=1;
    PIR1bits.ADIF=0;
    return;
}

void config_adcon(void){
    ADCON0bits.ADON=1;
    ADCON0bits.CHS=0b1100;
    ADCON0bits.ADCS=0b01;
    __delay_us(50);
    ADCON0bits.GO=1;
    
    ADCON1bits.ADFM=0;
    ADCON1bits.VCFG0=0;
    ADCON1bits.VCFG1=0;
    return;
}

void config_tmr0(void){
    OPTION_REGbits.T0CS=0;
    OPTION_REGbits.PSA=0;
    OPTION_REGbits.PS=0b110;
    TMR0=100;
    INTCONbits.T0IF=0;
}

uint16_t mapear_valor(uint16_t valor){
    uint16_t mapeo = (valor*100)/51;
    return mapeo;
}

void separar_valor(uint16_t valor){
    unidades = valor % 10;
    valor = valor - unidades;
    decenas = (valor%100)/10;
    valor = valor - decenas*10;
    centenas = valor/100;
}

uint8_t valor_a_display(uint8_t valor){
    uint8_t display;
    switch(valor){
        case 0:
            display = 0b00111111;
            break;
        case 1:
            display = 0b00000110;
            break;
        case 2:
            display = 0b01011011;
            break;
        case 3:
            display = 0b01001111;
            break;
        case 4:
            display = 0b01100110;
            break;
        case 5:
            display = 0b01101101;
            break;
        case 6:
            display = 0b01111101;
            break;
        case 7:
            display = 0b00000111;
            break;
        case 8:
            display = 0b01111111;
            break;
        case 9:
            display = 0b01100111;
            break;
        default:
            display = 0b00000000;
            break;
    }
    return display;
}
