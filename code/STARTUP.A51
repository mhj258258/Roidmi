$NOMOD51
NAME    ?C_STARTUP

EXTRN CODE (?C_START)
PUBLIC  ?C_STARTUP


EXTRN	IDATA	(STACK)

                CSEG    AT      0
?C_STARTUP:     LJMP    STARTUP1



?C_C51STARTUP		SEGMENT	CODE
								RSEG			?C_C51STARTUP
STARTUP1:
                MOV     SP, #STACK-1
                LJMP    ?C_START

                END