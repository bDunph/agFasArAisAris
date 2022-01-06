<CsoundSynthesizer>
<CsOptions>
; Select audio/midi flags here according to platform
; Audio out   Audio in    No messages
-odac          ;;;RT audio I/O

--nodisplays

</CsOptions>
<CsInstruments>

; Initialize the global variables.
sr = 48000 
kr = 480 
ksmps = 100 
nchnls = 2

; Set 0dbfs to 1
0dbfs = 1

; set up empty table to write real-time audio to
giTableLen	=	-5*sr	
giAudioBuf	ftgen	1, 0, giTableLen, 2, 0
giAudioBuf2	ftgen	2, 0, giTableLen, 2, 0

; window function - used as an amplitude envelope for each grain
giWFn   	ftgen 	0, 0, 16384, 9, 0.5, 1, 0

seed 0 ; seed the random generators from the system clock

;**************************************************************************************
instr 1 ; Modal Instrument
;**************************************************************************************

iamp    init ampdbfs(-12)

kGaussRange	=	10		
kModeFreq1	=	472.7		
kModeFreq2	=	928.5	

kRangeMin	gauss	kGaussRange	
kRangeMin += 65
kRangeMax	gauss	kGaussRange
kRangeMax += 80	
kcpsMin		gauss	kGaussRange * 0.1
kcpsMin += 4
kcpsMax		gauss	kGaussRange * 0.1
kcpsMax += 6

kFreqScale	rspline	kRangeMin,	kRangeMax,	kcpsMin,	kcpsMax

kRangeMin2	gauss	kGaussRange * 0.01
kRangeMin2 += -21
kRangeMax2	gauss	kGaussRange * 0.01
kRangeMax2 += -15

kWgbowAmpVal	rspline	kRangeMin2,	kRangeMax2,	kcpsMin,	kcpsMax

kRangeMin3	gauss	kGaussRange * 0.05
kRangeMin3 += 2	
kRangeMax3	gauss	kGaussRange * 0.05
kRangeMax3 += 4	

kWgbowPressureVal	rspline	kRangeMin3,	kRangeMax3,	kcpsMin,	kcpsMax

kRangeMin4	gauss	kGaussRange * 0.0004
kRangeMin4 += 0.127236	
kRangeMax4	gauss	kGaussRange * 0.0004
kRangeMax4 += 0.15	

kWgbowPosVal	rspline	kRangeMin4,	kRangeMax4,	kcpsMin,	kcpsMax

kRangeMin5	gauss	kGaussRange * 0.05
kRangeMin5 += 2	
kRangeMax5	gauss	kGaussRange * 0.05
kRangeMax5 += 10	

kWgbowVibF	rspline	kRangeMin5,	kRangeMax5,	kcpsMin,	kcpsMax

kRangeMin6	gauss	kGaussRange * 0.01
kRangeMin6 += -21	
kRangeMax6	gauss	kGaussRange * 0.01
kRangeMax6 += -15	

kWgbowVibAmp	rspline	kRangeMin6,	kRangeMax6,	kcpsMin,	kcpsMax

; mallet excitator----------------------------------

; to simulate the shock between the excitator and the resonator
;krand	random	1,	10	
;ashock  mpulse ampdbfs(-1), krand,	2
;
;; felt excitator from mode.csd
;;aexc1	mode	ashock,	80 * (kFreqScale + 1.0),	8
;aexc1	mode	ashock,	80,	8
;aexc1 = aexc1 * iamp
;
;;aexc2	mode	ashock,	188 * (kFreqScale * 1.0),	3
;aexc2	mode	ashock,	188,	3
;aexc2 = aexc2 * iamp
;
;aexc	sum	aexc1,	aexc2

; bow excitator-------------------------------------

kamp = ampdbfs(kWgbowAmpVal)
kfreq = kFreqScale 
kpres = kWgbowPressureVal
krat = kWgbowPosVal 
kvibf = kWgbowVibF 
kvamp = ampdbfs(kWgbowVibAmp)

aexc	wgbow	kamp,	kfreq,	kpres,	krat,	kvibf,	kvamp

;"Contact" condition : when aexc reaches 0, the excitator looses 
;contact with the resonator, and stops "pushing it"
aexc limit	aexc,	0,	3*iamp 

; ratios from http://www.csounds.com/manual/html/MiscModalFreq.html
ares1	mode	aexc,	(100 + p5) + kGaussRange,	220 + kGaussRange; * 0.8

ares2	mode	aexc,	(142 + p5) + kGaussRange,	280 + kGaussRange; * 0.4

ares3	mode	aexc,	(211 + p5) + kGaussRange,	200 + kGaussRange; * 0.2

ares4	mode	aexc,	(247 + p5) + kGaussRange,	220 + kGaussRange; * 0.2

ares5	mode	aexc,	467.9 + p5,	140 + kGaussRange * (kModeFreq1 * 0.1)	

ares6	mode	aexc,	935.8 + p5, 	140 + kGaussRange * (kModeFreq2 * 0.1)	

ares	sum	ares1,	ares2,	ares3,	ares4,	ares5, ares6

gaOut = (aexc * 0.001) + (ares * 0.166)

	;outs	gaOut,	gaOut

aIndx	phasor 1/5
tablew	gaOut, aIndx, p4, 1, 0, 1 

;ftprint	giAudioBuf

endin

;**************************************************************************************
instr 2 ; Triggers Instrument 3 
;**************************************************************************************

kFn	init 1
kPrevFn	init 0
kIndex	init 0
kIndex2	init 0
kfreq     random  0.08, 2 
;kfreq	= 0.08
ktrigger  metro   kfreq         ;metronome of triggers. One every 12.5s
if (ktrigger > 0) then
kIndex	=	kIndex + 1
endif
kNoteLen  random  5, 40
;iNoteLen = 40 
kWSize    random	sr/9, sr/2
kMod	=	kIndex % 27 

if (kMod == 0) then
kIndex2 = kIndex2 + 1
endif

kMod2	=	kIndex2 % 2

if (kMod2 == 0) then
kFn 	= 	1
elseif (kMod2 == 1) then
kFn	=	2
endif

schedkwhen ktrigger,0,0,3,0,kNoteLen,kWSize,kFn ;trigger instr. 3 for kNoteLen 

  endin

;**************************************************************************************
instr 3 ; Granulation of Audio from Buffer 
;**************************************************************************************

;define the input variables
ifn1        =         p5 
;ilen        =          nsamp(ifn1)/sr
ilen	=	nsamp(ifn1)
;iPtrStart   random     1,ilen-1
iPtrStart	=	ilen * 0.5
;iPtrTrav    random     -1,1
iPtrTrav	random	(ilen * 0.25) * -1.0, ilen * 0.25

ktimewarp   line       iPtrStart,p3,iPtrStart+iPtrTrav
kamp        linseg     0,p3/2,0.2,p3/2,0
iresample   random     -24,24.99
iresample   =          semitone(int(iresample))
ifn2        =          giWFn
ibeg        =          0
;iwsize      random     20,50000
iwsize      =          p4
irandw      =          iwsize/3
ioverlap    =         20 
itimemode   =         1 
; create a granular synthesis texture using sndwarp
aSig sndwarp  kamp,ktimewarp,iresample,ifn1,ibeg,\
                              iwsize,irandw,ioverlap,ifn2,itimemode
; envelope the signal with a lowpass filter
kcf         expseg     50,p3/2,12000,p3/2,50
aSig       moogvcf2    aSig, kcf, 0.5
aSig	= 	aSig * 0.5
            outs       aSig,aSig
  endin

</CsInstruments>
<CsScore>
f0	86400 ;keep csound running for a day

i1	0	6	1	0	
i2	7	-1	
i1	8	6	2	300

e
</CsScore>
</CsoundSynthesizer>
