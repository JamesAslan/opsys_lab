#define PORT 0xbfe48000

void (*BIOS_printstr)(char *b) = 0x8007b980;

void __attribute__((section(".entry_function"))) _start(void)
{
	// Call PMON BIOS printstr to print message "Hello OS!"
	char a[]="Hello OS!";
	BIOS_printstr(a);
	while(1);
	//printstr(b);
	return;
}

