/*
 * Dev_Inf.c
 *
 */
#include "Dev_Inf.h"
#include "W25QXX.h"

                 
/* This structure contains information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo = {
#endif
    "STM32F302R8+W25Q80",              // Device Name + version number
    SPI_FLASH,                       // Device Type
    0x00000000,                      // Device Start Address
    MEMORY_FLASH_SIZE,                   // Device Size in Bytes
    MEMORY_PAGE_SIZE,                    // Programming Page Size
    0xFF,                                // Initial Content of Erased Memory

    // Specify Size and Address of Sectors (view example below)
    { { (MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE),  // Sector Numbers,
        (uint32_t) MEMORY_SECTOR_SIZE },       //Sector Size

        { 0x00000000, 0x00000000 } } };