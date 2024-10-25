
#byte TRISG = 0xF98
#byte TRISF = 0xF97
#byte TRISE = 0xF96
#byte TRISD = 0xF95
#byte TRISC = 0xF94
#byte TRISB = 0xF93
#byte TRISA = 0xF92

#byte LATG = 0xF8F
#byte LATF = 0xF8E
#byte LATE = 0xF8D
#byte LATD = 0xF8C
#byte LATC = 0xF8B
#byte LATB = 0xF8A
#byte LATA = 0xF89

#bit    TRISE0 = TRISE.0
#bit    TRISE1 = TRISE.1
#bit    TRISE2 = TRISE.2

#bit    TRISC4 = TRISC.4

#bit    TRISB3 = TRISB.3

#bit    TRISA0 = TRISA.0
#bit    TRISA1 = TRISA.1
#bit    TRISA2 = TRISA.2
#bit    TRISA5 = TRISA.5




#bit    LATA0 = LATA.0
#bit    LATA1 = LATA.1
#bit    LATA2 = LATA.2
#bit    LATA3 = LATA.3

#byte PORTG = 0xF86
#bit    RG0 = PORTG.0
#bit    RG1 = PORTG.1
#bit    RG2 = PORTG.2
#bit    RG3 = PORTG.3
#bit    RG4 = PORTG.4
#bit    RG6 = PORTG.6
#bit    RG7 = PORTG.7
#byte PORTF = 0xF85
#bit    RF2 = PORTF.2
#bit    RF3 = PORTF.3
#bit    RF4 = PORTF.4
#bit    RF5 = PORTF.5
#bit    RF6 = PORTF.6
#bit    RF7 = PORTF.7

#byte PORTE = 0xF84
#bit    RE2 = PORTE.2


#byte PORTD = 0xF83
#byte PORTC = 0xF82
#bit    RC0 = PORTC.0
#bit    RC1 = PORTC.1
#bit    RC2 = PORTC.2
#bit    RC3 = PORTC.3
#bit    RC4 = PORTC.4
#bit    RC5 = PORTC.5
#bit    RC6 = PORTC.6
#bit    RC7 = PORTC.7

#byte PORTB = 0xF81
#bit    RB3 = PORTB.3

#byte PORTA = 0xF80
#bit    RA0 = PORTA.0
#bit    RA1 = PORTA.1
#bit    RA2 = PORTA.2
#bit    RA3 = PORTA.3
#bit    RA4 = PORTA.4
#bit    RA5 = PORTA.5
#bit    RA6 = PORTA.6
#bit    RA7 = PORTA.7


//__________________________________RTC___________________________________________
#byte RTCCON1 = 0xF5F
#bit    RTCPTR0 = RTCCON1.0
#bit    RTCPTR1 = RTCCON1.1
#bit    RTCOE = RTCCON1.2
#bit    HALFSEC = RTCCON1.3
#bit    RTCSYNC = RTCCON1.4
#bit    RTCWREN = RTCCON1.5
#bit    RTCEN = RTCCON1.7
#byte RTCCAL = 0xF5E
#byte RTCVALH = 0xF5D
#bit    WAITE0 = RTCVALH.0
#bit    WAITE1 = RTCVALH.1
#bit    WAITM0 = RTCVALH.2
#bit    WAITM1 = RTCVALH.3
#bit    WAITM2 = RTCVALH.4
#bit    WAITM3 = RTCVALH.5
#bit    WAITB0 = RTCVALH.6
#bit    WAITB1 = RTCVALH.7
//#byte RTCVALH = 0xF5D
#byte RTCVALL = 0xF5C
#byte ALRMCFG = 0xF5B
#bit    ALRMPTR0 = ALRMCFG.0
#bit    ALRMPTR1 = ALRMCFG.1
#bit    AMASK0 = ALRMCFG.2
#bit    AMASK1 = ALRMCFG.3
#bit    AMASK2 = ALRMCFG.4
#bit    AMASK3 = ALRMCFG.5
#bit    CHIME = ALRMCFG.6
#bit    ALRMEN = ALRMCFG.7
#byte ALRMRPT = 0xF5A
#byte ALRMVALH = 0xF59
#byte ALRMVALL = 0xF58
#byte RTCCON2 = 0xF57
#bit    RTCSECSEL0 = RTCCON2.0
#bit    RTCSECSEL1 = RTCCON2.1
#bit    RTCCLKSEL0 = RTCCON2.2
#bit    RTCCLKSEL1 = RTCCON2.3
#bit    PWCSPRE = RTCCON2.4
#bit    PWCCPRE = RTCCON2.5
#bit    PWCPOL = RTCCON2.6
#bit    PWCEN = RTCCON2.7


#byte EECON2 = 0xF7E

#byte OSCCON = 0xFD3

#byte T1CON  = 0xFCD

#bit  T1CON7 = T1CON.7
#bit  T1CON6 = T1CON.6
//!#bit  T1CON5 = T1CON.5
//!#bit  T1CON4 = T1CON.4
//#bit  T1CON7 = T1CON.7
//#bit  T1CON6 = T1CON.6

#bit  SOSCEN1 = T1CON.3
#bit  TMR1ON = T1CON.0

#byte T3CON  = 0xFB1
#bit  SOSCEN3 = T3CON.3


#byte PIE3 = 0xFA3
#bit  RC2IE = PIE3.5
