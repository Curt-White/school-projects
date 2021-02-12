; a3part1.asm
; CSC 230: Spring 2018
;
; Student name: Curtis White
; Student ID: V00891960
; Date of completed work: March 20 , 2018
;
; *******************************
; Code provided for Assignment #3
;
; Author: Mike Zastre (2018-Mar-08)
; 
; This skeleton of an assembly-language program is provided to help you
; begin with the programming tasks for A#3. As with A#2, there are 
; "DO NOT TOUCH" sections. You are *not* to modify the lines
; within these sections. The only exceptions are for specific
; changes announced on conneX or in written permission from the course
; instructor. *** Unapproved changes could result in incorrect code
; execution during assignment evaluation, along with an assignment grade
; of zero. ****
;
; I have added for this assignment an additional kind of section
; called "TOUCH CAREFULLY". The intention here is that one or two
; constants can be changed in such a section -- this will be needed
; as you try to test your code on different messages.
;


; =============================================
; ==== BEGINNING OF "DO NOT TOUCH" SECTION ====
; =============================================
;
; In this "DO NOT TOUCH" section are:
;
; (1) assembler directives setting up the interrupt-vector table
;
; (2) "includes" for the LCD display
;
; (3) some definitions of constants we can use later in the
;     program
;
; (4) code for initial setup of the Analog Digital Converter (in the
;     same manner in which it was set up for Lab #4)
;     
; (5) code for setting up our three timers (timer1, timer3, timer4)
;
; After all this initial code, your own solution's code may start.
;

.cseg
.org 0
	jmp reset

; location in vector table for TIMER1 COMPA
;
.org 0x22
	jmp timer1

; location in vector table for TIMER4 COMPA
;
.org 0x54
	jmp timer4

.include "m2560def.inc"
.include "lcd_function_defs.inc"
.include "lcd_function_code.asm"

.cseg

; These two constants can help given what is required by the
; assignment.
;
#define MAX_PATTERN_LENGTH 10
#define BAR_LENGTH 6

; All of these delays are in seconds
;
#define DELAY1 0.5
#define DELAY3 0.1
#define DELAY4 0.01


; The following lines are executed at assembly time -- their
; whole purpose is to compute the counter values that will later
; be stored into the appropriate Output Compare registers during
; timer setup.
;

#define CLOCK 16.0e6 
.equ PRESCALE_DIV=1024  ; implies CS[2:0] is 0b101
.equ TOP1=int(0.5+(CLOCK/PRESCALE_DIV*DELAY1))

.if TOP1>65535
.error "TOP1 is out of range"
.endif

.equ TOP3=int(0.5+(CLOCK/PRESCALE_DIV*DELAY3))
.if TOP3>65535
.error "TOP3 is out of range"
.endif

.equ TOP4=int(0.5+(CLOCK/PRESCALE_DIV*DELAY4))
.if TOP4>65535
.error "TOP4 is out of range"
.endif


reset:
	; initialize the ADC converter (which is neeeded
	; to read buttons on shield). Note that we'll
	; use the interrupt handler for timer4 to
	; read the buttons (i.e., every 10 ms)
	;
	ldi temp, (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	sts ADCSRA, temp
	ldi temp, (1 << REFS0)
	sts ADMUX, r16


	; timer1 is for the heartbeat -- i.e., part (1)
	;
    ldi r16, high(TOP1)
    sts OCR1AH, r16
    ldi r16, low(TOP1)
    sts OCR1AL, r16
    ldi r16, 0
    sts TCCR1A, r16
    ldi r16, (1 << WGM12) | (1 << CS12) | (1 << CS10)
    sts TCCR1B, temp
	ldi r16, (1 << OCIE1A)
	sts TIMSK1, r16

	; timer3 is for the LCD display updates -- needed for all parts
	;
    ldi r16, high(TOP3)
    sts OCR3AH, r16
    ldi r16, low(TOP3)
    sts OCR3AL, r16
    ldi r16, 0
    sts TCCR3A, r16
    ldi r16, (1 << WGM32) | (1 << CS32) | (1 << CS30)
    sts TCCR3B, temp

	; timer4 is for reading buttons at 10ms intervals -- i.e., part (2)
    ; and part (3)
	;
    ldi r16, high(TOP4)
    sts OCR4AH, r16
    ldi r16, low(TOP4)
    sts OCR4AL, r16
    ldi r16, 0
    sts TCCR4A, r16
    ldi r16, (1 << WGM42) | (1 << CS42) | (1 << CS40)
    sts TCCR4B, temp
	ldi r16, (1 << OCIE4A)
	sts TIMSK4, r16

    ; flip the switch -- i.e., enable the interrupts
    sei

; =======================================
; ==== END OF "DO NOT TOUCH" SECTION ====
; =======================================


; *********************************************
; **** BEGINNING OF "STUDENT CODE" SECTION **** 
; *********************************************
	
	;; init stack pointer
	ldi r16, high(RAMEND)
	out SPH, r16
	ldi r16, low(RAMEND)
	out SPL, r16
	
	;; init data locations to zero
	ldi r16, 0 
	sts PULSE, r16
	sts BUTTON_PREVIOUS, r16
	sts BUTTON_CURRENT,	r16
	sts BUTTON_LENGTH, r16
	ldi r16, 0
	ldi XL, low(BUTTON_COUNT)
	ldi XH, high(BUTTON_COUNT)
	st X+, r16
	st X, r16
	
	//load dotdash pattern address into counter
	ldi XL, low(COUNTER)
	ldi XH, high(COUNTER)
	ldi r16, high(DOTDASH_PATTERN)
	st X+, r16
	ldi r16, low(DOTDASH_PATTERN)
	st X, r16
	
	//fill the dotdashpattern with spaces
	call initdotdash

	rcall lcd_init
	
start:
	;;use timer 3 in polling
	in temp, TIFR3
	sbrs temp, OCF3A
	rjmp start

	ldi temp, 1<<OCF3A
	out TIFR3, temp
	
	;;call functions to update different parts of the display
	call update_heartbeat
	call update_number
	call update_stars
	call update_dotdash

	rjmp start
stop:
    rjmp stop

;;this function intializes the dot dash space with spaces to prevent unwanted intial characters on the lcd
initdotdash:
	push r16
	push r19
	push XL
	push XH
	
	;go to dotdash pattern memory location
	ldi XL, low(DOTDASH_PATTERN)
	ldi XH, high(DOTDASH_PATTERN)
	ldi r16, ' '
	ldi r19, 10
	
	;;fill memory location with zero
	initial:
		cpi r19,0
		breq endinit
		st X+, r16
		dec r19
		rjmp initial
		endinit:
		
	pop XH
	pop XL
	pop r19
	pop r16

	ret

;;update the dotdash pattern on the screen
update_dotdash:
	push r16
	push r17
	push r18
	push r19
	push XL
	push XH

	ldi XL, low(DOTDASH_PATTERN)
	ldi XH, high(DOTDASH_PATTERN)
	
	;;use r19 as a counter with max chars 10 in the pattern
	ldi r19, 10
	ldi r17, 0
	ldi r18, 0
	;;go to top left of the lcd
	push r17
	push r18
	rcall lcd_gotoxy
	pop r18
	pop r17
	
	;;cycle through data space location DOTDASH_PATTERN and put it on the screen
	;;exit once we have put 10 chars on the screen
	loop2:
		ld r16, X+
		cpi r19,0
		breq exitdotdash
		push r16
		rcall lcd_putchar
		pop r16
		dec r19
		rjmp loop2

	exitdotdash:
	pop XH
	pop XL
	pop r19
	pop r18
	pop r17
	pop r16

	ret

;;update the stars on the screen
update_stars:
	.def temp = r16
	.def counter1 = r17
	.def xcoor = r18
	.def ycoor = r19
	push temp
	push counter1
	push xcoor
	push ycoor
	
	;;if button current is 1 put stars if it is 0 then put a space
	lds temp, BUTTON_CURRENT
	cpi temp, 0
	breq spaces
	ldi temp, '*'
	rjmp skip1
	spaces:
	ldi temp, ' '
	skip1:

	;;counter hold the number of stars to print
	ldi counter1, 6
	ldi ycoor, 1
	ldi xcoor, 0
	
	;;goto the coordinates in the registers in the bottom left of the lcd
	push ycoor
	push xcoor
	rcall lcd_gotoxy
	pop xcoor
	pop ycoor
	
	;;while counter is not zero print stars across the screen, 6 stars exactly
	printloop:
		cpi counter1, 0
		breq exit1
		push temp
		rcall lcd_putchar
		pop temp
		dec counter1
		rjmp printloop
	exit1:
	pop ycoor
	pop xcoor
	pop counter1
	pop temp
	.undef counter1
	.undef xcoor
	.undef ycoor
	.undef temp
	ret

;;updates the number displayed on the lcd
update_number:
	.def numberH = r19
	.def numberL = r18
	push numberL
	push numberH
	push r20
	push r21
	push r22
	push r23
	push XL
	push XH
	push YL
	push YH
	
	;;load button_count in y register
	ldi YL, low(BUTTON_COUNT)
	ldi YH, high(BUTTON_COUNT)
	ld numberH, Y+
	ld numberL, Y+
	
	;;push the numbver of buttons pressed onto the stack
	push numberH
	push numberL

	ldi XL, low(DISPLAY_TEXT)
	ldi XH, high(DISPLAY_TEXT)

	push XH
	push XL
	
	;;call the function provided to us to convert number to characters
	rcall to_text
	
	;;pop everything into junk register
	pop r2
	pop r2
	pop r2
	pop r2 
	
	;;go to the bottom right in the lcd
	ldi r18, 1
	ldi r19,11
	push r18
	push r19
	rcall lcd_gotoxy
	pop r19
	pop r18
	
	;;print all of the characters in the display text location onto the screen
	ldi r22, 5
	loop:
		cpi r22, 0
		breq exit
		ld r23, X+
		push r23
		rcall lcd_putchar
		pop r23
		dec r22

		rjmp loop
		
	exit:
	pop YH
	pop YL
	pop XH
	pop XL
	pop r23
	pop r22
	pop r21
	pop r20
	pop numberH
	pop numberL

	.undef numberH
	.undef numberL

	ret

;; update the heart beat 
update_heartbeat:
	push r18
	push r19
	push r16
	push r17
	
	;;load pulse into r16 if pulse is zero load arrows in to r16 and r17 else load spaces
	lds r16, PULSE
	tst r16
	brne space
	ldi r16, '<'
	ldi r17, '>'
	rjmp arrow
	space:
	ldi r16, ' '
	ldi r17, ' '
	arrow:
	
	;;go to space on lcd and print the contents of r16 and r17
	ldi r18, 0
	ldi r19,14
	push r18
	push r19
	rcall lcd_gotoxy
	pop r19
	pop r18

	;left arrow or space
	push r16
	rcall lcd_putchar
	pop r16

	;right arrow or space
	push r17
	rcall lcd_putchar
	pop r17

	pop r17
	pop r16
	pop r19
	pop r18

	ret

;;;;THIS IS THE CODE FROM THE EXAMPLE GIVEN TO US BY DR. ZASTRE
;;;;NOT MY WORK
to_text:
	.set PARAMETER_OFFSET = 16
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r0
	push r16
	push YH
	push YL
	push ZH
	push ZL
	in YH, SPH
	in YL, SPL

	ldd r19, Y+PARAMETER_OFFSET+3
	ldd r18, Y+PARAMETER_OFFSET+2

	andi r19, 0b01111111

	clr r0
	clr r23
	ldi r16, '0'

	decimal_next:
		clr r22

	decimal_10000:
		cpi r23, 0
		brne decimal_1000
		ldi r20, low(10000)
		ldi r21, high(10000)
		rjmp decimal_loop
	
	decimal_1000:
		cpi r23,1
		brne decimal_100
		ldi r20, low(1000)
		ldi r21, high(1000)
		rjmp decimal_loop
	
	decimal_100:
		cpi r23, 2
		brne decimal_10
		ldi r20, low(100)
		ldi r21, high(100)
		rjmp decimal_loop
		
	decimal_10:
		cpi r23, 3
		brne decimal_1
		ldi r20, low(10)
		ldi r21, high(10)
		rjmp decimal_loop

	decimal_1:
		mov r22, r18
		rjmp decimal_write

	decimal_loop:
		inc r22
		sub r18,r20
		sbc r19, r21
		brpl decimal_loop
		dec r22
		add r18, r20
		adc r19, r21

	decimal_write:
		ldd ZH, Y+PARAMETER_OFFSET+1
		ldd ZL, Y+PARAMETER_OFFSET+0
		add ZL, r23
		adc ZH, r0
		add r22, r16

		st Z, r22

		inc r23
		cpi r23, 5
		breq decimal_exit
		rjmp decimal_next

	decimal_exit:
		pop ZL
		pop ZH
		pop YL
		pop YH
		pop r16
		pop r0
		pop r23
		pop r22
		pop r21
		pop r20
		pop r19
		pop r18

		ret

;;interrupt for timer 1 which changes the value in pulse to either a one or zero depending on the 
;;previous value stored in the location PULSE
timer1:
	;save state
	push r16
	push r17
	in r16, SREG
	push r16
	
	;;load and flip pulses value from zero to one or one to zero
	ldi r17,1
	lds r16, PULSE
	eor r16, r17
	sts PULSE, r16

	;restore state
	pop r16
	out SREG, r16
	pop r17
	pop r16

	reti

; Note there is no "timer3" interrupt handler as we must use this
; timer3 in a polling style within our main program.

timer4:
	push r16 
	push r17
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push XL
	push XH
	push YL
	push YH
	push ZL
	push ZH

	;save sreg
	in r16, SREG
	push r16

	;store the previous button count into the Button_previous location
	lds r16,BUTTON_CURRENT
	sts BUTTON_PREVIOUS, r16
	
	;check to see if the button is currently being pressed
	lds r16, ADCSRA
	ori r16, 0x40
	sts ADCSRA, r16
	
	wait:
		lds r16, ADCSRA
		andi r16, 0x40
		brne wait

	lds r17, ADCL
	lds r18, ADCH
	
	;max value that will be received if the button is pressed is 0x352
	ldi XL, low(0x352)
	ldi XH, high(0x352)
	
	cp r17, XL
	cpc r18, XH
	
	;if the current value is greater than 0x352 than the button is not pressed
	brsh notpressed
	
	;while the button is pressed we increment button length and store back to dataspace
	lds r21, BUTTON_LENGTH
	inc r21
	sts BUTTON_LENGTH, r21
	
	;load 1 into r16 if button is pressed
	ldi r16, 1
	rjmp skip
	notpressed:
	;load 0 to r16 if button is not pressed
	ldi r16,0 
	skip:
	;store r16s value to dataspace in button current
	sts BUTTON_CURRENT, r16
	lds r17, BUTTON_PREVIOUS
	
	;load DOTDASH_PATTERNs address from counter
	lds ZH, COUNTER
	lds ZL, COUNTER + 1
	
	;check to see if in BUTTON_PREVIOUS the button was pressed but it is currently not pressed
	cpi r17,0
	breq checkforincrement
		cpi r16, 1
		breq checkforincrement
		;based on the button length we chose to put either a dash or a dot
		lds r21, BUTTON_LENGTH
		cpi r21, 20
		brlo dot
		ldi r21, '-'
		st Z+, r21
		rjmp end1
		dot:
		ldi r21, '.'
		st Z+, r21
	end1:
	;store the location of the next place in DOTDASH _PATTERN that we will place the next dot or dash
	sts COUNTER, ZH
	sts COUNTER + 1, ZL
	
	;clear button length because the button was released
	ldi r21,0
	sts BUTTON_LENGTH, r21
	
	;if button was previously pressed but not currently
	checkforincrement:
	cpi r17,1
	breq noinc
		cpi r16, 0
		breq noinc
		;;load in button count
		ldi XL, low(BUTTON_COUNT)
		ldi XH, high(BUTTON_COUNT)
		ld r20, X+
		ld r19, X+ 
		;load one into two registers
		ldi r28, low(1)
		ldi r29, high(1)
		;add one too the total
		add r19, r28
		adc r20, r29
		;store back to the button_count location
		ldi XL, low(BUTTON_COUNT)
		ldi XH, high(BUTTON_COUNT)
		st X+ , r20
		st X+, r19
		
	noinc:
	pop r16
	out SREG, r16
	pop ZH
	pop ZL
	pop YH
	pop YL
	pop XH
	pop XL
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r17
	pop r16
	
    reti

; ***************************************************
; **** END OF FIRST "STUDENT CODE" SECTION ********** 
; ***************************************************

; ################################################
; #### BEGINNING OF "TOUCH CAREFULLY" SECTION ####
; ################################################

; The purpose of these locations in data memory are
; explained in the assignment description.
;

.dseg

PULSE: .byte 1
COUNTER: .byte 2
DISPLAY_TEXT: .byte 16
BUTTON_CURRENT: .byte 1
BUTTON_PREVIOUS: .byte 1
BUTTON_COUNT: .byte 2
BUTTON_LENGTH: .byte 1
DOTDASH_PATTERN: .byte MAX_PATTERN_LENGTH

; ##########################################
; #### END OF "TOUCH CAREFULLY" SECTION ####
; ##########################################
