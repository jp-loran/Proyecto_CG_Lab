#include <16F877A.h>
/**********TEMPERATURA**********/
#device *=16
#device adc=10
float temperatura;
/********VOLTAJE***********/
int16 valor_adc;
float voltaje;
/*******LCD**********/
#use delay(clock=20M)
#define LCD_ENABLE_PIN PIN_D6 /* ASIGNAMOS EL PIN "B0" COMO SALIDA PARA E */
#define LCD_RS_PIN PIN_D7 /* ASIGNAMOS EL PIN "B1" COMO SALIDA PARA RS*/
#define LCD_RW_PIN PIN_D1 /* ASIGNAMOS EL PIN "B2" COMO SALIDA PARA RW*/
#define LCD_DATA4 PIN_D5 /* ASIGNAMOS EL PIN "B3" COMO SALIDA PARA D4*/
#define LCD_DATA5 PIN_D4/* ASIGNAMOS EL PIN "B4" COMO SALIDA PARA D5*/
#define LCD_DATA6 PIN_D3/* ASIGNAMOS EL PIN "B5" COMO SALIDA PARA D6*/
#define LCD_DATA7 PIN_D2/* ASIGNAMOS EL PIN "B6" COMO SALIDA PARA D7*/

#define up  PIN_B7
#define down  PIN_B6
#define enter  PIN_B5
#define back  PIN_B4
#include <lcd.c>/*libreria para usar el lcd*/
#FUSES NOWDT                    /* No Watch Dog Timer*/
#FUSES HS                       /* High speed Osc (> 4mhz)*/
#FUSES PUT                      /* Power Up Timer*/
#FUSES NOPROTECT                /* Code not protected from reading*/
#FUSES NOBROWNOUT               /* No brownout reset*/
//#FUSES NOMCLR                     /* Master Clear pin enabled*/
#FUSES NOLVP                    /* No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O*/



void main()
{

/************LCD*************/
boolean bandera=false;
int cont=0;
lcd_init ();

     while(TRUE)
     {
          
          if(input(enter)==0){
            bandera=true;
          }
          if(input(back)==0){
            bandera=false;
          }
          if(bandera){
           /**************SUBMENUS*****************/
          if(cont==0){
                 /*********TEMPERATURA*********/
                setup_adc_ports(RA0_ANALOG);
                setup_adc(ADC_CLOCK_INTERNAL);
                setup_COUNTERS(RTCC_internal.rtcc_div_1);
                set_adc_channel(0);
                temperatura=(float)read_adc()/2;
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "Temperatura          "); /*ESCRIBE EL TEXTO EN PANTALLA*/
                lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "%.2f GRADOS            ",temperatura); /*TEMPERATURA*/
                 
            }else if(cont==1){
                 /*********Voltaje*********/
                setup_adc_ports(ALL_ANALOG);
                setup_adc(ADC_CLOCK_INTERNAL);
                setup_COUNTERS(RTCC_internal.rtcc_div_1);;
                set_adc_channel(1);
                valor_adc=read_adc();
                voltaje=5.0*valor_adc/1023.0;
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "VOLTIMETRO           "); 
                  lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "%f volts              ",voltaje); /*VOLTIMETRO*/
            }else if(cont==2){
             /*********AMPERIMETRO*********/
                setup_adc_ports(ALL_ANALOG);
                setup_adc(ADC_CLOCK_INTERNAL);
                setup_COUNTERS(RTCC_internal.rtcc_div_1);;
                set_adc_channel(2);
                valor_adc=read_adc();
                voltaje=5.0*valor_adc/1023.0;
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "AMPERIMETRO          "); 
                lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "%f mA             ",voltaje*1000); /*AMPERIMETRO*/
            }else{
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "NV"); /*ESCRIBE EL TEXTO EN PANTALLA*/
                  lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "No valido    "); /*NO VALIDO*/
            }
          }else{
          /***************SOLO CAMBIE EN MENU PRINCIPAL****************/
            if(input(down)==0 && cont<=1 && cont>=-1){
              cont+=1;// 0   1   2  
              delay_ms(200);
            }
            if(input(up)==0 && cont>=1 && cont<=2){
              cont--;//2  1  0
              delay_ms(200);
            }
          /********MENU PRINCIPAL*********/
            if(cont==0){
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "PROYECTO PJI"); /*ESCRIBE EL TEXTO EN PANTALLA*/
                lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "*Temperatura  "); /*TEMPERATURA*/
                 
            }else if(cont==1){
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "PROYECTO PJI"); 
                  lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "*Voltimetro  "); /*VOLTIMETRO*/
            }else if(cont==2){
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "PROYECTO PJI"); 
                lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "*Amperimetro  "); /*AMPERIMETRO*/
            }else{
                lcd_gotoxy (1,1);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "PROYECTO PJI"); /*ESCRIBE EL TEXTO EN PANTALLA*/
                  lcd_gotoxy (1,2);/*(COLUMNA,FILA)*/
                 printf (lcd_putc, "No valido    "); /*NO VALIDO*/
            }
          }
          
          
     }
}
