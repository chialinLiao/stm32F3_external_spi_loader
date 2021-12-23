/**
  ******************************************************************************
  * @file    Loader_Src.h
  * @author  MCD Application Team
  * @brief   Header file of Loader_Src.c
  *           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOADER_SRC_H
#define __LOADER_SRC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32F3xx_hal.h"
#include "stm32F3xx_hal_spi.h"

#define KeepInCompilation __attribute__((used))

#define LOADER_OK 0x1
#define LOADER_FAIL 0x0

/* Private function prototypes -----------------------------------------------*/
KeepInCompilation int Init ();
KeepInCompilation int Read (uint32_t Address, uint32_t Size, uint8_t* Buffer);
KeepInCompilation int Write (uint32_t Address, uint32_t Size, uint8_t* Buffer);
KeepInCompilation int MassErase (void);
KeepInCompilation int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress);
KeepInCompilation uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement);

#endif /* __LOADER_SRC_H */
