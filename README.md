# stm32F3_external_spi_loader

1. dev_inf =>  填寫硬體描述 
2. loader_src => 只是一個轉接器, 內容請撰寫相關周邊驅動
3. 使用 stm32cubeide 建構
4. debug 時,   
 a. 請選擇原先.id  
 b. 並關閉 SCB->VTOR = 0x20000000 | 0x200; 指令(否則除錯時freerun會出錯)  
 
5. loader 是執行在 Ram, 建構時請選擇 linker.ld
6. 請勾選 discard unused sections (-WI,--gc-sections)
7. 上述勾選會造成 loader_src 的 API 未被建置, 所以請在main.c 呼叫讓其建置
