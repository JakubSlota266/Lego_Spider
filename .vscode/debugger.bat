set PATH=c:\Users\jakub\.vscode\HusarionTools\bin\;%PATH%
cd c:\Users\jakub\Desktop\lego_spider || exit 1
start /wait st-flash write myproject.bin 0x08010000 || exit 1
start st-util
arm-none-eabi-gdb %*