1.STMCubeIDE Bulid Post Cmd for Produution .stdlr 
cmd.exe /C copy/Y  "${BuildArtifactFileBaseName}.elf" "..\W25Q40_CUSTOMER_STMF302R8.stldr"

========================== split line  ============================

使用官方 example code 建構 external loader, 官方範例 Flash 皆使用 QSPI + 映射模式達成, 但STM32F3 系列無 QSPI, 僅能使用 SPI 來達成 (無映射模式)

參考網路作法, 概述如下
1. flash loader 選擇 nor-flash type 不選擇 spi-flash type.  (選擇 spi-flash 嘗試建立但失敗)
2. 映射位址為 0x90000000, 把 CubeProgrammer 端傳送過來的位址減去映射位置 => 等於實際 flash 位址

參考網址:
https://community.st.com/s/question/0D53W00000UAthc/stm32f3-external-loader-spi-flash-based
https://www.twblogs.net/a/5efc40b2e94612e3fce9de73
  


 