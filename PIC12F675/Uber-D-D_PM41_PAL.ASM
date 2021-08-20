                                                                                       
;                                                     ,-.             ,---,                 
;       ,---.                                     ,--/ /|   ,--,    .'  .' `\               
;      /__./|                   .--.            ,--. :/ | ,--.'|  ,---.'     \              
; ,---.;  ; |                 .--,`|  .--.--.   :  : ' /  |  |,   |   |  .`\  |  .--.--.    
;/___/ \  | |   ,--.--.       |  |.  /  /    '  |  '  /   `--'_   :   : |  '  | /  /    '   
;\   ;  \ ' |  /       \      '--`_ |  :  /`./  '  |  :   ,' ,'|  |   ' '  ;  :|  :  /`./   
; \   \  \: | .--.  .-. |     ,--,'||  :  ;_    |  |   \  '  | |  '   | ;  .  ||  :  ;_     
;  ;   \  ' .  \__\/: . .     |  | ' \  \    `. '  : |. \ |  | :  |   | :  |  ' \  \    `.  
;   \   \   '  ," .--.; |     :  | |  `----.   \|  | ' \ \'  : |__'   : | /  ;   `----.   \ 
;    \   `  ; /  /  ,.  |   __|  : ' /  /`--'  /'  : |--' |  | '.'|   | '` ,/   /  /`--'  / 
;     :   \ |;  :   .'   \.'__/\_: |'--'.     / ;  |,'    ;  :    ;   :  .'    '--'.     /  
;      '---" |  ,     .-./|   :    :  `--'---'  '--'      |  ,   /|   ,.'        `--'---'   
;             `--`---'     \   \  /                        ---`-' '---'                     
;                           `--`-'          
;	     _   _ _                     ______       ______ 
;	    | | | | |                    |  _  \      |  _  \
;	    | | | | |__   ___ _ __ ______| | | |______| | | |
;	    | | | | '_ \ / _ \ '__|______| | | |______| | | |
;	    | |_| | |_) |  __/ |         | |/ /       | |/ / 
;	     \___/|_.__/ \___|_|         |___/        |___/  
;							    'The Black Screen of Injection Acception'
;							
;
;
;		  __             _____ _____ _____   __ ___  ______ ________ _____ 
;		 / _|           |  __ \_   _/ ____| /_ |__ \|  ____/ /____  | ____|
;		| |_ ___  _ __  | |__) || || |       | |  ) | |__ / /_   / /| |__  
;		|  _/ _ \| '__| |  ___/ | || |       | | / /|  __| '_ \ / / |___ \ 
;		| || (_) | |    | |    _| || |____   | |/ /_| |  | (_) / /   ___) |
;		|_| \___/|_|    |_|   |_____\_____|  |_|____|_|   \___/_/   |____/  (c) 2021
; MPLAB IDE V5.20 / MPASM        
; 
; 
;										    
;                      +---\/---+
;                 3.5v |1*     8|  GND
;                 BIOS |2      7|  LED (optional, bridge to GND if not used)
;                 DATA |3      6|  N/C
;                 RST  |4      5|  D. Lid
;                      +--------+
;   Reset not optional, do not leave floating	                                                  
    
    LIST P=12F675
   #INCLUDE <P12F675.inc>
   
   
   ; __config 0xF1A4
 __CONFIG _FOSC_INTRCIO & _WDTE_OFF & _PWRTE_ON & _MCLRE_ON & _BOREN_OFF & _CP_OFF & _CPD_OFF

   
   cblock 0x20
	StringsTotal	    ; @20h	   
	StringsDuring	    ; @21h	   
	StringsPrior	    ; @22h	   
	CounterA	    ; @23h	   
	CounterB	    ; @24h
	CounterC	    ; @25h
	CounterD	    ; @26h
	
	
	fullbcount  ; @27h Decimal 9, after first pass it will be at 8 (for full byte counter, bit by bit)
	vacant	    ; @28h
	byte1	    ; @29h  ;10011010
	byte2	    ; @2Ah  ;10010011
	byte3	    ; @2Bh  ;11010010
	byte4	    ; @2Ch  ;10111010
	byte5	    ; @2Dh  ;01010111
	byte6	    ; @2Eh  ;01000000
	_vacant	    ; @2Fh
   endc
   			
    ORG 0x00					     ;Begin @ start of PROG MEM
    
    ;Factory Calibrate OSCCAL
    bsf	    0x03,5	;Bank 1
    call    3FFh	;Get the cal value
    movwf   OSCCAL	;Calibrate
    
   
   GOTO Setup
   
   
;=============================================================================================================   
Setup				
	clrf    0X9F		; clear ANSEL, all pins digital I/O bar reserved
	;bsf	0x85,2		; make GP2 / Pin5 (drivelid) an input
	;bsf	0x85,0		; make GP0 / Pin7 start as an input
	;bsf	0x85,5		; make GP5 / Pin2 BIOS start as input
	movlw	B'00101101'     ; As above in single line
	movwf	0x85
	bcf	0x83,5		; bank 0		
	clrf    0x05		; init GPIO
	movlw	B'111'		; comparator off
	movwf	0x19		;	    (CMCON)
			
;=============================================================================================================
	

    goto    cont
		

;/////////////////////////////////////////////////////////////////////////
Genuine
    bsf	    0x03,5	; bank 1  
    bcf	    0x85,0	; set LED pin as output
    call    LEDON
    call    _1second	; on for 1s
    call    LEDOFF
    call    _10seconds  ; off for 10s
    goto    Genuine	; Endless loop in this mode, no exit without powerdown / reset
;/////////////////////////////////////////////////////////////////////////
	
	
cont
;---------------------------------------------------------------------------------------	
		; Store integers @ FREG locations set in header
   
    movlw   B'10011010'		; Store SCEE string in RAM
    movwf   byte1
    movlw   B'10010011'
    movwf   byte2
    movlw   B'11010010'
    movwf   byte3
    movlw   B'10111010'
    movwf   byte4
    movlw   B'01010111'
    movwf   byte5
    movlw   B'01000000'
    movwf   byte6
    
   movlw    D'8'		; Full bit by bit byte counter
   movwf    fullbcount		; Store counter and
   
   movlw    D'5'
   movwf    StringsPrior	; Move 5 into this variable (@22h)
   	
   movlw    D'30'		; 30
   movwf    StringsDuring	 
   
   movlw    D'79'		; 79
   movwf    StringsTotal	 
;---------------------------------------------------------------------------------------
   
   
   
	 ;goto	inject		; DEBUG	
	 goto	StartUpDelay 

	 
	 
;*******************LED SUBROUTINES******************
LEDOFF				    
		bsf	    0x03,5	; bank 1	
		bsf	    0x85,0	; set LED pin as input
		bcf	    0x83,5	; bank 0
		return
		
LEDON	
		bsf	    0x03,5	; bank 1
		bcf	    0x85,0	; set LED pin as output
		bcf	    0x83,5	; bank 0
		return
;****************************************************	
		


;*******************DELAY SUBROUTINES****************
bitdelay		
;PIC Time Delay = 0.00397500 s with Osc = 4000000 Hz
		movlw	D'6'
		movwf	CounterB
		movlw	D'40'
		movwf	CounterA
bloop		decfsz	CounterA,1
		goto	bloop
		decfsz	CounterB,1
		goto	bloop
		return



_10seconds
		
;PIC Time Delay = 10.00000200 s with Osc = 4000000 Hz
		movlw	D'51'
		movwf	CounterC
		movlw	D'187'
		movwf	CounterB
		movlw	D'223'
		movwf	CounterA
_10loop		decfsz	CounterA,1
		goto	_10loop
		decfsz	CounterB,1
		goto	_10loop
		decfsz	CounterC,1
		goto	_10loop
		retlw	0
		return
		
		
_1second
		;PIC Time Delay = 1.00000200 s with Osc = 4000000 Hz
		movlw	D'6'
		movwf	CounterC
		movlw	D'19'
		movwf	CounterB
		movlw	D'173'
		movwf	CounterA
_1.loop		decfsz	CounterA,1
		goto	_1.loop
		decfsz	CounterB,1
		goto	_1.loop
		decfsz	CounterC,1
		goto	_1.loop
		retlw	0
		return
;****************************************************	
	

;//////////////////////////////////////////////////////////////////////////////////////////////////		
stringdelay		
		call	    LEDOFF	
		;Check Drive Lid Status
		bcf	    0x03,5	; Bank 0 for lid check
		btfsc	    0x05,2	; pin 5 high?
		goto	    _newdisc	; jump here if early exit by interrupting injections with drivelid = open
					; else just continue
					
		decfsz	    StringsTotal,1	    
		goto	    _stringdelay
		goto	    StealthMode
_stringdelay
		
		;goto	inject; DEBUG
		
;PIC Time Delay = 0.16700000 s with Osc = 4000000 Hz
		movlw	D'217'
		movwf	CounterB
		movlw	D'225'
		movwf	CounterA
sloop		decfsz	CounterA,1
		goto	sloop
		decfsz	CounterB,1
		goto	sloop
		goto	inject
					
;//////////////////////////////////////////////////////////////////////////////////////////////////			
					
		
				
;BIOS STUFF**********************************************************************************		
BIOSPATCH
		bsf	0x03,5	;bank 1
		bcf	0x85,5			    ;Clear BIOS bit in TRISIO (now an output)
		return				    ;Jump back to Injection cycle
RELEASEBIOS
		bsf	0x03,5	;bank 1		    ;Sw to bank 1 for TRISIO REG
		BSF	0x85,5			    ;Set BIOS bit in TRISIO (now an input)
		return
;********************************************************************************************			
		
		
		
		
StartUpDelay
;PIC Time Delay = 4.80000100 s with Osc = 4000000 Hz
		movlw	D'25'
		movwf	CounterC
		movlw	D'90'
		movwf	CounterB
		movlw	D'178'
		movwf	CounterA
stloop		decfsz	CounterA,1
		goto	stloop
		decfsz	CounterB,1
		goto	stloop
		decfsz	CounterC,1
		goto	stloop
		goto	check_genuine


check_genuine		
		
;============= GENUINE?? ==============================================================
	bcf	0x03,5		; Bank 0 for GPIO check
	btfss   0x05,2		; Modchip Disabled? (check pin5 at boot)
	goto	inject	
	goto	Genuine		; Go here if lid's open on boot, else continues, need to check after start up delay!
;======================================================================================	
		
	
	
;BIOSpatch=============================================================================
inject
		bsf	    0x03,5		    ; bank 1
		decfsz	    StringsPrior,1	    ; Conditional branches
		goto	    _inject		    
		call	    BIOSPATCH
		
		
_inject

		decfsz	    StringsDuring,1	    
		goto	    injectLED
		call	    RELEASEBIOS	
;======================================================================================		
;Injection Routine=====================================================================
injectLED	
		call	    LEDON
		goto	    byte1_
		
byte1_			    ;//////////////////////////////////////////////////
		
		btfsc	    byte1,7
		goto	    highbit1
		goto	    lowbit1
highbit1
    bsf	    0x03,5	    ; bank 1	
    bsf	    0x85,4	    ; release (make input)
    rlf	    byte1	    ; rotate bits left
    call    bitdelay	    ; COMMENT FOR DEBUG
    decf    fullbcount    ; Dec bit counter
    goto    byte1_
			    
lowbit1
    bsf	    0x03,5	    ; bank 1
    bcf	    0x85,4	    ; make output, pull line
    rlf	    byte1	    ; rotate bits left
    call    bitdelay	    ; COMMENT FOR DEBUG
    decfsz  fullbcount      ; last bit of first byte is '0'- branch goes here
    goto    byte1_	    ; branch condition unmet, inject next bit
    bsf	    fullbcount,3    ; branch condition met (bcounter=0) so reset to 8 for next byte
    rlf	    byte1	    ; rotate left 1 more time (reset byte to original position for next cycle)
    goto    byte2_	    ; then go to next byte 

byte2_			    ;//////////////////////////////////////////////////
		
		btfsc	    byte2,7
		goto	    highbit2
		goto	    lowbit2
highbit2
    bsf	    0x03,5	    ; bank 1	
    bsf	    0x85,4	    ; release (make input)
    rlf	    byte2	    ; rotate bits left
    call    bitdelay	    ; COMMENT FOR DEBUG
    decfsz  fullbcount      ; last bit of second byte is '1'- branch goes here
    goto    byte2_
    bsf	    fullbcount,3    ; branch condition met (bcounter=0) so reset to 8 for next byte
    rlf	    byte2	    ; rotate left 1 more time (reset byte to original position for next cycle)
    goto    byte3_	    ; then go to next byte 
			    
lowbit2
    bsf	    0x03,5	    ; bank 1
    bcf	    0x85,4	    ; make output, pull line
    rlf	    byte2 	    ; rotate bits left
    call    bitdelay	    ; COMMENT FOR DEBUG
    decf    fullbcount	    
    goto    byte2_	    ; branch condition unmet, inject next bit

byte3_			    ; ETC, branch to string delay on last byte
			    ;//////////////////////////////////////////////////
		btfsc	    byte3,7 
		goto	    highbit3
		goto	    lowbit3
highbit3
    bsf	    0x03,5
    bsf	    0x85,4
    rlf	    byte3
    call    bitdelay	    ; COMMENT FOR DEBUG
    decf    fullbcount	    
    goto    byte3_
			    
lowbit3
    bsf	    0x03,5	    
    bcf	    0x85,4	    
    rlf	    byte3	    
    call    bitdelay	    ; COMMENT FOR DEBUG
    decfsz  fullbcount	    ; branch for this byte (last bit is '0')
    goto    byte3_	    
    bsf	    fullbcount,3    ; reset   
    rlf	    byte3	    ; reset
    goto    byte4_	    ; branch to next byte
    
byte4_			    ;//////////////////////////////////////////////////
		btfsc	    byte4,7 
		goto	    highbit4
		goto	    lowbit4
highbit4
    bsf	    0x03,5
    bsf	    0x85,4
    rlf	    byte4
    call    bitdelay	    ; COMMENT FOR DEBUG
    decf    fullbcount	    
    goto    byte4_
			    
lowbit4
    bsf	    0x03,5	    
    bcf	    0x85,4	    
    rlf	    byte4	    
    call    bitdelay	    ; COMMENT FOR DEBUG
    decfsz  fullbcount      ; branch for this byte (last bit is '0')
    goto    byte4_	    
    bsf	    fullbcount,3    ; reset   
    rlf	    byte4	    ; reset
    goto    byte5_	    ; branch to next byte  
    
byte5_			    ;//////////////////////////////////////////////////
		
		btfsc	    byte5,7
		goto	    highbit5
		goto	    lowbit5
highbit5
    bsf	    0x03,5
    bsf	    0x85,4	    
    rlf	    byte5	    
    call    bitdelay	    
    decfsz  fullbcount	    
    goto    byte5_
    bsf	    fullbcount,3    
    rlf	    byte5	    
    goto    byte6_	  
			    
lowbit5
    bsf	    0x03,5	    
    bcf	    0x85,4	    
    rlf	    byte5	  
    call    bitdelay	    
    decf    fullbcount	    
    goto    byte5_	    
    
byte6_			    ;//////////////////////////////////////////////////	   
			   
		btfsc	    byte6,7 
		goto	    highbit6
		goto	    lowbit6
highbit6
    bsf	    0x03,5
    bsf	    0x85,4
    rlf	    byte6
    call    bitdelay	    ; COMMENT FOR DEBUG
    decf    fullbcount	    
    goto    byte6_
			    
lowbit6
    bsf	    0x03,5	    
    bcf	    0x85,4	    
    rlf	    byte6	    
    call    bitdelay	    ; COMMENT FOR DEBUG
    decfsz  fullbcount      ; branch for this byte (last bit is '0')
    goto    byte6_	    
    bsf	    fullbcount,3    ; reset   
    rlf	    byte6	    ; reset
    goto    stringdelay	    ; branch to string delay
;========================================================================================    


;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
StealthMode
    bcf	    0x83,5	; Bank 0 for the GPIO checks on GP2 / Pin5
    call    LEDON
    btfsc   0x05,2	; drivelid check
    goto    _newdisc	; skip this instruction if lid = closed
    call    _1second	; LED on for 1s
    call    LEDOFF
    call    _1second	; off for 4s with a drivelid check between each
    btfsc   0x05,2	; 1s delay
    goto    _newdisc
    call    _1second
    btfsc   0x05,2
    goto    _newdisc	; If drive lid is open on any check
    call    _1second	; goto to 'newdisc'
    btfsc   0x05,2
    goto    _newdisc
    call    _1second
    goto    StealthMode ; loop only broken by drive lid = open
;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  
    

;.........................................................................    
_newdisc			;Reset all variables here, in case of early exit, or injection cycle completion

    bsf		0x03,5		; bank 1	 	
    bsf		0x85,4		; DATA input (for early exit of inject)
    bsf		0x85,5		; BIOS input (early exit of inject during patch)
    bcf		0x83,5		; bank0
    
   movlw    D'8'		; Full bit by bit byte counter
   movwf    fullbcount		; Store counter and
   
   movlw    D'5'
   movwf    StringsPrior	; Move 5 into this variable (@22h)
   	
   movlw    D'30'		; 30
   movwf    StringsDuring	 
   
   movlw    D'79'		; 79
   movwf    StringsTotal	
    goto	    newdisc
;.........................................................................		
		
		
;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
newdisc
		
    call    LEDON	; leave LED on while lids open
    bcf	    0x03,5	; bank 0
    btfss   0x05,2	; check if lids still open
    goto    inject
    goto    newdisc	; if it's still open loop the routine, else execute the above
			; goto instruction and return to the injection routine
;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ 
    
			
			
	
			
    end	;   requirement, but never reached
    
