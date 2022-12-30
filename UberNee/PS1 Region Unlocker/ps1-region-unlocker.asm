
; UberChip / UberNee
;	'The Black Screen of Injection Acception' 
;			(C) 30/12/22 VajSkids Consoles
;  ______  _____ __       ______ _____ _____ _____ _____ _   _        _   _ _   _  _     _____ _____  _   __ ___________ 
;  | ___ \/  ___/  |      | ___ \  ___|  __ \_   _|  _  | \ | |      | | | | \ | || |   |  _  /  __ \| | / /|  ___| ___ \
;  | |_/ /\ `--.`| |______| |_/ / |__ | |  \/ | | | | | |  \| |______| | | |  \| || |   | | | | /  \/| |/ / | |__ | |_/ /
;  |  __/  `--. \| |______|    /|  __|| | __  | | | | | | . ` |______| | | | . ` || |   | | | | |    |    \ |  __||    / 
;  | |    /\__/ /| |_     | |\ \| |___| |_\ \_| |_\ \_/ / |\  |      | |_| | |\  || |___\ \_/ / \__/\| |\  \| |___| |\ \ 
;  \_|    \____/\___/     \_| \_\____/ \____/\___/ \___/\_| \_/       \___/\_| \_/\_____/\___/ \____/\_| \_/\____/\_| \_|    
; MPLAB IDE V5.20 / MPASM        
; 
; Sometime's displays 2 'X' Characters at the black screen
; Sometime's displays correct text as per region of disc
; Sometime's fails
; Version 1 - no timing signal (may need to probe for some timing signals to get reliability to a solid 100%
; 
;										    
;                      +---\/---+
;                 3.5v |1*     8|  GND
;              D2 BIOS |2      7|  N/C
;                 N/C  |3      6|  N/C
;                 N/C  |4      5|  N/C
;                      +--------+    

   
    LIST P=12F675
   #INCLUDE <P12F675.inc>
   
   
 __CONFIG _FOSC_INTRCIO & _WDTE_OFF & _PWRTE_ON & _MCLRE_OFF & _BOREN_OFF & _CP_OFF & _CPD_OFF
 
 
 cblock 0x20
	   
	CounterA	   	   
	CounterB	    
	CounterC	    
	CounterD	    

   endc

     			
    ORG 0x00					     
    
    
    bsf	    0x03,5	
    call    3FFh	
    movwf   0x90	
     
    
    goto Setup
    
 seconddelay
;PIC Time Delay = 5.28000300 s with Osc = 4000000 Hz
		movlw	D'27'
		movwf	CounterC
		movlw	D'202'
		movwf	CounterB
		movlw	D'16'
		movwf	CounterA
patchlength	decfsz	CounterA,1
		goto	patchlength
		decfsz	CounterB,1
		goto	patchlength
		decfsz	CounterC,1
		goto	loop
		retlw	0
		return
firstdelay
;PIC Time Delay = 6.11000300 s with Osc = 4000000 Hz
		movlw	D'31'
		movwf	CounterC
		movlw	D'255'
		movwf	CounterB
		movlw	D'250'
		movwf	CounterA
loop		decfsz	CounterA,1
		goto	loop
		decfsz	CounterB,1
		goto	loop
		decfsz	CounterC,1
		goto	loop
		retlw	0
		return  
BIOSPATCH
		bsf	0x03,5	
		bcf	0x85,5	
		return				    
RELEASEBIOS
		bsf	0x03,5	
		BSF	0x85,5	
		return	
Setup				
	clrf    0X9F		
	movlw	0x2D     	
	movwf	0x85
	bcf	0x83,5		
	clrf    0x05		
	movlw	B'111'		
	movwf	0x19		
			

goto main	
main	
		call firstdelay
	
		call BIOSPATCH
		
		call seconddelay
		
		call RELEASEBIOS
	
			
	finished
	goto finished
	
			
    end	;   
    
