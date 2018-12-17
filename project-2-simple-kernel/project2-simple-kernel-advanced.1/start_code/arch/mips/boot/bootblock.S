.text
.global main

main:
	# use your own code in here!
	# 1) call BIOS print string "It's bootblock!
	# 2) call BIOS read kernel in SD card.
	# 3) jump to kernel.
# 4) while(1) --> stop here
	la $a0 , msg
	jal 0x8007b980
	nop

	lw $a0 , kernel
	li $a1 , 0x00000200
	lb	$a2,  0xa08001ff
	sll	$a2, 9
	#li $a2 ,0x0000b000

	jal 0x8007b1cc
	nop

	jal 0xa0800200
stop:
	j stop
.data

msg: .ascii "It's a bootloader...\n"

# 1. PMON read SD card function address
# read_sd_card();
read_sd_card: .word 0x8007b1cc

# 2. PMON print string function address
# printstr(char *string)
printstr: .word 0x8007b980

# 3. PMON print char function address
# printch(char ch)
printch: .word 0x8007ba00

# 4. kernel address (move kernel to here ~)
kernel : .word 0xa0800200

# 5. kernel main address (jmp here to start kernel main!)
kernel_main : .word 0xa0800200
