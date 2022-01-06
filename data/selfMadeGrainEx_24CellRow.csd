<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>

<CsInstruments>
;adapted by Bryan Dunphy, from http://floss.booktype.pro/csound/g-granular-synthesis/, example 05G02_selfmade_grain.csd written by Iain McCurdy

sr = 44100
ksmps = 32
nchnls = 2
0dbfs = 1

; the name of the sound file used is defined as a string variable -
; - as it will be used twice in the code.
; This simplifies adapting the orchestra to use a different sound file
gSfile = "24CellRow_mono.wav"

; waveform used for granulation
giSound  ftgen 1,0,262144,1,gSfile,0,0,0

; window function - used as an amplitude envelope for each grain
giWFn   ftgen 2,0,1024,9,0.5,1,0

seed 0 ; seed the random generators from the system clock
gaSend init 0  ; initialize global audio variables

  instr 1 ; triggers instrument 2
ktrigger  metro   0.08         ;metronome of triggers. One every 12.5s
schedkwhen ktrigger,0,0,2,0,4,15 ;trigger instr. 2 for 40s
  endin

  instr 2 ; generates granular synthesis textures

ktimewarp random 1, 4	;length of "24CellRow_mono.wav"
kresample init 1		;do not change pitch
ibeg = 0			;start at beginning
iwsize = 4410			;window size in samples with
irandw = 882			;bandwidth of a random number generator
itimemode = 0			;ktimewarp is "time" pointer
ioverlap = 20 

aSig sndwarp .5, ktimewarp, kresample, giSound, ibeg, iwsize, irandw, ioverlap, giWFn, itimemode

; envelope the signal with a lowpass filter
kcf         expseg     50,p3/2,12000,p3/2,50
aSig       moogvcf2    aSig, kcf, 0.5
; add a little of our audio signals to the global send variables -
; - these will be sent to the reverb instrument (2)
gaSend     =          gaSend+(aSig*0.4)
            outs       aSig,aSig
  endin

  instr 3 ; reverb (always on)
aRvbL,aRvbR reverbsc   gaSend,gaSend,0.85,8000
            outs       aRvbL,aRvbR
;clear variables to prevent out of control accumulation
            clear      gaSend
  endin

</CsInstruments>

<CsScore>
; p1 p2 p3
;i 1  0  3600 ; triggers instr 2
i 2 0 10 
i 3  0  3600 ; reverb instrument
e
</CsScore>
</CsoundSynthesizer>
