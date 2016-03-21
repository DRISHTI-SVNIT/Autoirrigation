
#ifndef extUSART
#define extUSART

/* function prototype */
void ext_usart_clock_init(void);
void ext_usart_pin_init(void);
void ext_usart_init(void);
void USART_Transmitchar(unsigned char);
void USART_TransmitNumber(unsigned long);
void USART_TransmitString(char *);
unsigned char USART_Receive( void );

#endif
