<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>

<CsInstruments>
;adapted by Bryan Dunphy, from http://floss.booktype.pro/csound/g-granular-synthesis/, example 05G02_selfmade_grain.csd written by Iain McCurdy

sr = 48000 
kr = 480
ksmps = 100 
nchnls = 2 
0dbfs = 1

; the name of the sound file used is defined as a string variable -
; - as it will be used twice in the code.
; This simplifies adapting the orchestra to use a different sound file
gSfile = "Rain_1.wav"

; waveform used for granulation
giSound  ftgen 1,0,1048576,1,gSfile,0,0,0

; window function - used as an amplitude envelope for each grain
giWFn   ftgen 2,0,16384,9,0.5,1,0

seed 0 ; seed the random generators from the system clock
gaSendL init 0  ; initialize global audio variables
gaSendR init 0

;**************************************************************************************
  instr 1 ; triggers instrument 2
;**************************************************************************************

kFreq     	random  	2, 	25 
kMetVal		metro   	0.2,	0.00000001		
kTrigVal	samphold	kFreq,	kMetVal	

kTrigger	metro		kTrigVal
kNoteLen	random		0.05,		0.2
kWSize		random		80,		85

schedkwhen kTrigger,0,0,2,0,kNoteLen,kWSize ;trigger instr. 2 for 40s

  endin

;**************************************************************************************
  instr 2 ; generates granular synthesis textures
;**************************************************************************************

;define the input variables
ifn1        =          	giSound
ilen        =          	nsamp(ifn1)/sr
iPtrStart   random     	1,ilen-1
iPtrTrav    random     	-1,1
ktimewarp   line       	iPtrStart,p3,iPtrStart+iPtrTrav
kamp        linseg     	0,p3/2,0.2,p3/2,0
iresample   random     	-24,24.99
iresample   =          	semitone(int(iresample))
ifn2        =          	giWFn
ibeg        =          	0
;iwsize     random     	20,50000
iwsize      =          	p4
irandw      =          	iwsize/3
ioverlap    =		8 
itimemode   =          	1

; create a stereo granular synthesis texture using sndwarp
aSigL,aSigR sndwarpst  kamp,ktimewarp,iresample,ifn1,ibeg,\
                              iwsize,irandw,ioverlap,ifn2,itimemode

; envelope the signal with a lowpass filter
kcf         expseg     50,p3/2,12000,p3/2,50
aSigL       moogvcf2    aSigL, kcf, 0.5
aSigR       moogvcf2    aSigR, kcf, 0.5

; add a little of our audio signals to the global send variables -
; - these will be sent to the reverb instrument (2)
gaSendL     =          gaSendL+(aSigL*0.4)
gaSendR     =          gaSendR+(aSigR*0.4)

            outs       aSigL,aSigR
  endin

;**************************************************************************************
  instr 3 ; reverb (always on)
;**************************************************************************************

aRvbL,aRvbR reverbsc   gaSendL,gaSendR,0.85,8000

            outs       aRvbL,aRvbR

;clear variables to prevent out of control accumulation
            clear      gaSendL,gaSendR

  endin

</CsInstruments>

<CsScore>
; p1 p2 p3
i 1  0  3600 ; triggers instr 2
i 3  0  3600 ; reverb instrument
e
</CsScore>
</CsoundSynthesizer>
