#include "xprintf_init.h"
#include "usart.h"

#ifndef UASRT_X
#define UASRT_X USART6
#endif
#ifndef _BV
#define	_BV(bit) (1<<(bit))
#endif


static volatile struct {
	uint16_t	tri, twi, tct;
	uint16_t	rri, rwi, rct;
	uint8_t		tbuf[X_UART_TXB];
	uint8_t		rbuf[X_UART_RXB];
} x_Fifo;


//#ifndef USARTX_HANDLE
//#define USARTX_HANDLE USARTX##_IRQHandler
//#endif
//void USARTX_HANDLE (void)
//{
////	uint32_t sr = USART1_SR;	/* Interrupt flags */
//	uint32_t sr = UASRT_X->SR;
//	uint8_t d;
//	int i;


//	if (sr & _BV(5)) {	/* RXNE is set: Rx ready */
////		d = USART1_DR;	/* Get received byte */
//		d = UASRT_X->DR;	/* Get received byte */
//		i = x_Fifo.rct;
//		if (i < X_UART_RXB) {	/* Store it into the rx fifo if not full */
//			x_Fifo.rct = ++i;
//			i = x_Fifo.rwi;
//			x_Fifo.rbuf[i] = d;
//			x_Fifo.rwi = ++i % X_UART_RXB;
//		}
//	}

//	if (sr & _BV(7)) {	/* TXE is set: Tx ready */
//		i = x_Fifo.tct;
//		if (i--) {	/* There is any data in the tx fifo */
//			x_Fifo.tct = (uint16_t)i;
//			i = x_Fifo.tri;
////			USART1_DR = x_Fifo.tbuf[i];
//			UASRT_X->DR = x_Fifo.tbuf[i];
//			x_Fifo.tri = ++i % X_UART_TXB;
//		} else {	/* No data in the tx fifo */
////			USART1_CR1 &= ~_BV(7);		/* Clear TXEIE - Disable TXE irq */
//			UASRT_X->CR1 &= ~_BV(7);		/* Clear TXEIE - Disable TXE irq */
//		}
//	}
//}



//int x_uart_test (void)
//{
//	return x_Fifo.rct;
//}



//uint8_t x_uart_getc (void)
//{
//	uint8_t d;
//	int i;

//	/* Wait while rx fifo is empty */
//	while (!x_Fifo.rct) ;

//	i = x_Fifo.rri;			/* Get a byte from rx fifo */
//	d = x_Fifo.rbuf[i];
//	x_Fifo.rri = ++i % X_UART_RXB;
//	__disable_irq();
//	x_Fifo.rct--;
//	__enable_irq();

//	return d;
//}



//void x_uart_putc (uint8_t d)
//{
//	int i;

//	/* Wait for tx fifo is not full */
//	while (x_Fifo.tct >= X_UART_TXB) ;

//	i = x_Fifo.twi;		/* Put a byte into Tx fifo */
//	x_Fifo.tbuf[i] = d;
//	x_Fifo.twi = ++i % X_UART_TXB;
//	__disable_irq();
//	x_Fifo.tct++;
////	USART1_CR1 |= _BV(7);	/* Set TXEIE - Enable TXE irq */
//	UASRT_X->CR1 |= _BV(7);
//	__enable_irq();
//}

void x_uart_putc (uint8_t d){
	
 	while((UASRT_X->SR&0X40)==0);//循环发送,直到发送完毕   
	UASRT_X->DR = (uint8_t) d;      

}

uint8_t x_uart_getc (void){
  return 0;
}

void x_printf_init(void){
  xdev_out(x_uart_putc);
	xdev_in(x_uart_getc);
}
