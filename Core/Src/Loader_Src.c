#include "spi.h"
#include "main.h"
#include "gpio.h"
#include "W25QXX.h"

#define LOADER_OK 0x1
#define LOADER_FAIL 0x0

extern void SystemClock_Config(void);

/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1 : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int Init(void) 
{
  *(uint32_t*)0xE000EDF0 = 0xA05F0000; //enable interrupts in debug
    
  SystemInit();

/* ADAPTATION TO THE DEVICE
 *
 * change VTOR setting for H7 device
 * SCB->VTOR = 0x24000000 | 0x200;
 *
 * change VTOR setting for other devices
 * SCB->VTOR = 0x20000000 | 0x200;
 *
 * */

  SCB->VTOR = 0x20000000 | 0x200;

  __set_PRIMASK(0); //enable interrupts

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  
  __HAL_RCC_SPI3_FORCE_RESET(); //completely reset peripheral
  __HAL_RCC_SPI3_RELEASE_RESET();

  if(HAL_SPI_DeInit(&hspi3) != HAL_OK)
  {
     __set_PRIMASK(1); //disable interrupts
    return LOADER_FAIL;
  }

  MX_SPI3_Init();
  HAL_Delay(2);
  BSP_W25Qx_Init();
  
  __set_PRIMASK(1); //disable interrupts
  return LOADER_OK;
}


/**
  * Description :
  * Read data from the device 
  * Inputs    :
  *      Address       : Write location
  *      Size          : Length in bytes  
  *      buffer        : Address where to get the data to write
  * outputs   :
  *      R0             : "1" 			: Operation succeeded
  * 			  "0" 			: Operation failure
  * Note: Mandatory for all types except SRAM and PSRAM	
  */
int Read (uint32_t Address, uint32_t Size, uint8_t* buffer)
{ 
  if(BSP_W25Qx_Read(buffer, Address, Size) == W25Qx_OK)
    return LOADER_OK;
  else
    return LOADER_FAIL;
} 


/**
  * Description :
  * Write data from the device 
  * Inputs    :
  *      Address       : Write location
  *      Size          : Length in bytes  
  *      buffer        : Address where to get the data to write
  * outputs   :
  *      R0           : "1" 			: Operation succeeded
  *                     "0" 			: Operation failure
  * Note: Mandatory for all types except SRAM and PSRAM	
  */
int Write (uint32_t Address, uint32_t Size, uint8_t* buffer)
{
  if(BSP_W25Qx_Write(buffer, Address, Size) == W25Qx_OK)
    return LOADER_OK;
  else
    return LOADER_FAIL;
} 


/**
  * Description :
  * Erase a full sector in the device
  * Inputs    :
  *     None
  * outputs   :
  *     R0             : "1" : Operation succeeded
  * 			 "0" : Operation failure
  * Note: Not Mandatory for SRAM PSRAM and NOR_FLASH
  */
int MassErase (void)
{  
  if(BSP_W25Qx_Erase_Chip() == W25Qx_OK)
    return LOADER_OK;
  else
    return LOADER_FAIL;
}


/**
  * Description :
  * Erase a full sector in the device
  * Inputs    :
  *      SectrorAddress	: Start of sector
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : "1" : Operation succeeded
  * 			 "0" : Operation failure
  * Note: Not Mandatory for SRAM PSRAM and NOR_FLASH
  */
int SectorErase (uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{      
  EraseStartAddress = EraseStartAddress - EraseStartAddress % 0x10000;
  
  while (EraseEndAddress >= EraseStartAddress)
  {
    if(BSP_W25Qx_Erase_Block(EraseStartAddress) == W25Qx_OK)
      EraseStartAddress += 0x10000;
    else
      return LOADER_FAIL;
  }
  
  return LOADER_OK;	
}


/**
  * Description :
  * Calculates checksum value of the memory zone
  * Inputs    :
  *      StartAddress  : Flash start address
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Checksum value
  * Note: Optional for all types of device
  */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
  uint8_t missalignementAddress = StartAddress%4;
  uint8_t missalignementSize = Size ;
	int cnt;
	uint32_t Val;
  uint8_t value;
	
  StartAddress-=StartAddress%4;
  Size += (Size%4==0)?0:4-(Size%4);
  
  for(cnt=0; cnt<Size ; cnt+=4)
  {
    BSP_W25Qx_Read(&value, StartAddress , 1);
    Val = value;
    BSP_W25Qx_Read(&value, StartAddress + 1, 1);
    Val+= value<<8;
    BSP_W25Qx_Read(&value, StartAddress + 2, 1);
    Val+= value<<16;
    BSP_W25Qx_Read(&value, StartAddress + 3, 1);
    Val+= value<<24;
    
    if(missalignementAddress)
    {
      switch (missalignementAddress)
      {
        case 1:
          InitVal += (uint8_t) (Val>>8 & 0xff);
          InitVal += (uint8_t) (Val>>16 & 0xff);
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=1;
          break;
        case 2:
          InitVal += (uint8_t) (Val>>16 & 0xff);
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=2;
          break;
        case 3:   
          InitVal += (uint8_t) (Val>>24 & 0xff);
          missalignementAddress-=3;
          break;
      }  
    }
    else if((Size-missalignementSize)%4 && (Size-cnt) <=4)
    {
      switch (Size-missalignementSize)
      {
        case 1:
          InitVal += (uint8_t) Val;
          InitVal += (uint8_t) (Val>>8 & 0xff);
          InitVal += (uint8_t) (Val>>16 & 0xff);
          missalignementSize-=1;
          break;
        case 2:
          InitVal += (uint8_t) Val;
          InitVal += (uint8_t) (Val>>8 & 0xff);
          missalignementSize-=2;
          break;
        case 3:   
          InitVal += (uint8_t) Val;
          missalignementSize-=3;
          break;
      } 
    }
    else
    {
      InitVal += (uint8_t) Val;
      InitVal += (uint8_t) (Val>>8 & 0xff);
      InitVal += (uint8_t) (Val>>16 & 0xff);
      InitVal += (uint8_t) (Val>>24 & 0xff);
    }
    StartAddress+=4;
  }
  
  return (InitVal);
}


/**
  * Description :
  * Verify flash memory with RAM buffer and calculates checksum value of
  * the programmed memory
  * Inputs    :
  *      FlashAddr     : Flash address
  *      RAMBufferAddr : RAM buffer address
  *      Size          : Size (in WORD)  
  *      InitVal       : Initial CRC value
  * outputs   :
  *     R0             : Operation failed (address of failure)
  *     R1             : Checksum value
  * Note: Optional for all types of device
  */
uint64_t Verify (uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
  uint32_t InitVal = 0;
  uint32_t VerifiedData = 0;
  uint8_t TmpBuffer = 0x00;
	uint64_t checksum;
  
  Size *= 4;
        
  checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);
  
  while (Size>VerifiedData)
  {
    BSP_W25Qx_Read(&TmpBuffer, MemoryAddr + VerifiedData, 1);
         
    if (TmpBuffer != *((uint8_t*)RAMBufferAddr + VerifiedData))
      return ((checksum<<32) + MemoryAddr + VerifiedData);
        
    VerifiedData++;  
  }
       
  return (checksum<<32);
}
