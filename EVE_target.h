/*
@file    EVE_target.h
@brief   include file for EVE_target.c
@version 4.0
@date    2019-02-11
@author  Rudolph Riedel

*/ 


#ifndef EVE_TARGET_H_
#define EVE_TARGET_H_

#if defined (EVE_DMA)

extern uint8_t EVE_dma_buffer[4100];
extern volatile uint16_t EVE_dma_buffer_index;
extern volatile uint8_t EVE_dma_busy;

void EVE_init_dma(void);
void EVE_start_dma_transfer(void);

#endif

#endif /* EVE_TARGET_H_ */