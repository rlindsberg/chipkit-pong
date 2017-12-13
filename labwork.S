  # labwork.S
  # Written 2015 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1 2015-2016.
  # The macros PUSH and POP are in the public domain.

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm

  # You can add your own code here
	.data
	.align 2
mytime:	.word 0x5957	#mytime is initialized to 59:57, 59 minutes 57 seconds
timstr:	.ascii "text more text lots of text\0"
	.text


.global hexasc
hexasc:
	#slti $a1,$a0,0x000a	#if a0 less than 11, a1 becomes 1, else becomes 0
	bgt $a0, 0x0009, isLetter # does the above and below commented stuff with one instruction
	#beq $a1,0,isLetter	#if a1 is 0, the value is above 9 and it is a letter, so go to isLetter. Else, keep reading
	nop
	
	addi $v0,$a0,0x0030	#if it didnt branch above, it is a number, and this gets the correct ASCII
	jr	$ra		#go back to main
	nop
	
	isLetter:
	addi $v0,$a0,0x0037	#for letters. 0x37 is what was needed to add to 10 to get A, which is 0x41 = 65(10). 65-10 = 55. 55 = 0x37
	jr	$ra		#go back to main
	nop

	
.global time2string	
#time2string converts time info inte a string of printable chars, with a null-byte as an end of string marker.
#time2string takes 2 parameters. a0 contains adress of an area in memory, that points to an output from time2string. 16 lsb in a1 containts time-info, four digits, 4 bits each.
#a0 must after the function have six ascii chars. minutes : seconds(nullbyte). so 59:57(null). hexasc should be used.
time2string:
	PUSH $ra
	
	#0010000
	beq $a1,0x0010000,ifHOUR 
	nop
	
	#digit1
	PUSH $a0		#a0 has to be the argument to hexasc, but we need it later.. so push it
	andi $a0,$a1,0xf000	#and the time numbers with 11000000 to only get the top number
	srl $a0,$a0,12		#shift bit right to get the first minute digit as the last number
	jal hexasc
	nop
	POP $a0
	sb $v0, ($a0)		#store the resulting ascii char in memory adress which is in a0

	#digit2
	PUSH $a0
	andi $a0,$a1,0x0f00	#and the time numbers with 00110000 to get 2nd digit
	srl $a0, $a0, 8
	jal hexasc
	nop
	POP $a0
	sb $v0, 1($a0)		#now store in a0 +1
	
	# pop/push t1? lecture note: callee can freely use t0 to t9 without saving them
	#Colon
	li $t1, 0x3a		#load the colon into t1
	sb $t1, 2($a0)		#store in a0 +2
	
	#digit3
	PUSH $a0
	andi $a0, $a1, 0xf0	#get 3rd (first seconds) digit
	srl $a0, $a0, 4
	jal hexasc
	nop
	POP $a0
	sb $v0, 3($a0)
	
	#digit4
	PUSH $a0
	andi $a0, $a1, 0xf
	jal hexasc
	nop
	POP $a0
	sb $v0, 4($a0)
	
	#nullbyte
	li $t1, 0x00
	sb $t1, 5($a0)		#after storing in a0+5, we should have a memory like 35 39 3a 35 38 00 , the string of numbers
		
	POP $ra
	jr $ra
	nop
	

ifHOUR:
	#HOUR = 0x48 , 0x4f, 0x55, 0x52
	li $t1, 0x52554f48
	sw $t1, ($a0)
	li $t2, 0x00
	sb $t2, 4($a0)
	POP $ra
	jr $ra
	nop


