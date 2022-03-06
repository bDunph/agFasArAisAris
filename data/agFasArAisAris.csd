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

seed 0 ; seed the random generators from the system clock

;------------------------ global send variables ---------------------------------------------
gaSend init 0 			;send variable for Granulated Rain	 

gaReverbSend	init	0	;global reverb send variable for Karplus-Strong
gaCompSend	init	0	;global compression send variable for Karplus-Strong

;------------------- waveform and window function used for granulated rain instrument ---------------------------
giSound  	ftgen 0,0,1048576,1,"Rain_1.wav",0,0,0

; window function - used as an amplitude envelope for each grain
giWFnGranRain   ftgen 0,0,16384,9,0.5,1,0

;------------------ window function - used by sndwarp as an amplitude envelope for each grain --------------
giWFnSndWrp   	ftgen 	0, 0, 16384, 9, 0.5, 1, 0

;-------------------bartlett window function - used by grain3 as an amplitude envelope for each grain --------------
giWFn   	ftgen 	0,0,16384,20,3,1

;------------------ empty tables to write real-time audio to ------------------------
giTableLen	=	-5*sr	
giAudioBuf	ftgen	1, 0, giTableLen, 2, 0
giAudioBuf2	ftgen	2, 0, giTableLen, 2, 0

;------------------ tables used by partikkel ---------------------------------------
giSine		ftgen	0, 0, 65537, 10, 1
giCosine	ftgen	0, 0, 8193, 9, 1, 1, 90

giWave  	ftgen  	0,0,2^10,10,1,1/2,1/4,1/8,1/16,1/32,1/64
giBuzz  	ftgen 	0,0,4096,11,40,1,0.9
;giSine		ftgen 	0,0,4096,10,1
giSineWave	ftgen	0,0,16384,10,1

giFMWave  	ftgen  	0, 0, 2^10, 10, 1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64
;giCosine	ftgen	0, 0, 8193, 9, 1, 1, 90		; cosine
giDisttab	ftgen	0, 0, 32768, 7, 0, 32768, 1	; for kdistribution
giFile		ftgen	0, 0, 0, 1, "24cellRow.wav", 0, 0, 0	; soundfile for source waveform
;giFile2		ftgen	0, 0, 0, 1, "8cellRow.wav", 0, 0, 0	; soundfile for fm waveform
;giFile3		ftgen	0, 0, 0, 1, "5cellRow.wav", 0, 0, 0	; source waveform
giWin		ftgen	0, 0, 4096, 20, 6, 1		; gaussian window 
giPan		ftgen	0, 0, 32768, -21, 1		; for panning (random values between 0 and 1)
giAttack	ftgen	0, 0, 513, 5, 0.0001, 512, 1	; exponential curve 
giDecay		ftgen	0, 0, 513, 5, 1, 512, 0.0001	; exponential curve
;giGainMask	ftgen	0, 0, 5, 2, 0, 3, 1, 0.9, 0
giWavFreqStart	ftgen	0, 0, 5, 2, 0, 2, 1, 0.4, 0
giWavFreqEnd	ftgen	0, 0, 5, 2, 0, 2, 1, 0.9, 0
giFmIndex	ftgen	0, 0, 5, 2, 0, 3, 0.8, 0.4, 0 
giFmEnv		ftgen	0, 0, 129, 7, 0, 32, 1, 12, 0.7, 64, 0.7, 20, 0
giWavAmp	ftgen	0, 0, 8, 2, 0, 4, 1, 1, 1, 1, 0 

;--------------------------- table used by the kick drum synth -----------------------------------
giSquare	ftgen	0, 0, 16384, 10, 1, 0, 0.3, 0, 0.2, 0, 0.14, 0, 0.111	

;**************************************************************************************
;  instr ModalSynthTrigger, 1 	;	Triggers instr 8 which produces a 
  					;	granular swishing type texture.
;**************************************************************************************

;kFreq		chnget		"noteFreq"
;kNoteLen	chnget		"noteLength"
;kWSize		chnget		"winSize"
;kWSize = floor(kWSize)

;kFreq     	random  	2, 	25 
;kMetVal		metro   	0.2,	0.00000001		
;kTrigVal	samphold	kFreq,	kMetVal	

;kTrigger	metro		kTrigVal
;kTrigger	metro		kFreq	

;kNoteLen	random		0.05,		0.2
;kWSize		random		80,		85

;schedkwhen kTrigger,0,0,2,0,kNoteLen,kWSize ;trigger instr. 2 for 40s

;  endin


;**************************************************************************************
instr ModalSynth, 2 ; Modal Synth 
;**************************************************************************************

; get control value from application
;kSineControlVal	chnget	"sineControlVal"

iamp    init ampdbfs(-12)

;kFreqScale chnget "randFreq" ; random frequency scale value sent from application
;kWgbowAmpVal chnget "randAmp"
;kWgbowPressureVal chnget "randPressure"
;kWgbowPosVal chnget "randPos"
;kGaussRange	chnget	"gaussRange"
;kModeFreq1	chnget 	"modeFreq1"
;kModeFreq2	chnget	"modeFreq2"
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

;gaOut1 = (aexc + ares) * kSineControlVal 
gaOut1 = (aexc * 0.001) + (ares * 0.166)
;	outs	gaOut1,	gaOut1

;--------------------- write output to function table--------------------------------
aIndx	phasor 1/5
tablew	gaOut1, aIndx, p4, 1, 0, 1 

;--------------------- send route mean square value out------------------------------
;kRms	rms	gaOut1
;	chnset	kRms,	"rmsOut"

endin

;**************************************************************************************
instr ModalSamplerTrigger, 3	;	Triggers instr 3 which samples the modal synth 
				;	and sndwarps it	
;**************************************************************************************

kFreq		chnget		"modSamp_noteFreq"
kNoteLen	chnget		"modSamp_noteLength"
kWSize		chnget		"modSamp_winSize"
kWSize = floor(kWSize)
kOverlap	chnget		"modSamp_overlap"
kAmp		chnget		"modSamp_amp"

kFn	init 1
;kPrevFn	init 0
;kIndex	init 0
;kIndex2	init 0

;kfreq     random  0.08, 2 
;kMetVal		metro   	1,	0.00000001		
;kTrigVal	samphold	kFreq,	kMetVal	

;kTrigger	metro		kTrigVal
kTrigger	metro		kFreq	

;if (ktrigger > 0) then
;kIndex	=	kIndex + 1
;endif
;kNoteLen  random  5, 40
;iNoteLen = 40 
;kWSize    random	sr/9, sr/2
;kMod	=	kIndex % 27 

;if (kMod == 0) then
;kIndex2 = kIndex2 + 1
;endif

;kMod2	=	kIndex2 % 2

;if (kMod2 == 0) then
;kFn 	= 	1
;elseif (kMod2 == 1) then
;kFn	=	2
;endif

schedkwhen kTrigger, 0, 0, 4, 0, kNoteLen, kWSize, kFn, kOverlap, kAmp 
  endin

;**************************************************************************************
instr ModalSampler, 4	;	Reads audio recorded from Modal Synth and uses 
			;	sndwarp to stretch and granulate it
;**************************************************************************************

kMoogCutoff	chnget	"modSamp_moogCutoff"
kMoogRes	chnget	"modSamp_moogRes"

;define the input variables
ifn1        =         p5 
;ilen        =          nsamp(ifn1)/sr
ilen	=	nsamp(ifn1)
;iPtrStart   random     1,ilen-1
iPtrStart	=	ilen * 0.5
;iPtrTrav    random     -1,1
iPtrTrav	random	(ilen * 0.25) * -1.0, ilen * 0.25

ktimewarp   line       iPtrStart,p3,iPtrStart+iPtrTrav
kamp        linseg     0,p3/2,p7,p3/2,0
iresample   random     -24,24.99
iresample   =          semitone(int(iresample))
ifn2        =          giWFnSndWrp
ibeg        =          0
;iwsize      random     20,50000
iwsize      =          p4
irandw      =          iwsize/3
ioverlap    =         p6 
itimemode   =         1 
; create a granular synthesis texture using sndwarp
aSig sndwarp  kamp,ktimewarp,iresample,ifn1,ibeg,\
                              iwsize,irandw,ioverlap,ifn2,itimemode
; envelope the signal with a lowpass filter
;kcf         expseg     50,p3/2,12000,p3/2,50
;aSig       moogvcf2    aSig, kMoogCutoff, 0.5
aSig       moogvcf2    aSig, kMoogCutoff, kMoogRes 
aSig	= 	aSig * 0.05

;--------------------- send route mean square value out------------------------------
kRms	rms	aSig	
	chnset	kRms,	"modSamp_rmsOut"

;gaModalSamplerOut = aSig
            outs       aSig,aSig

  endin

;**************************************************************************************
instr ClickPopStaticTrigger, 5	;	Triggers instr 5 which creates a click and pop
				;	texture using partikkel
;**************************************************************************************

;kFileSpeed	chnget	"fileSpeed"
kFileSpeed = 5.0
;kGrainDurFactor chnget	"grainSize"
kGrainDurFactor = 90.0

kGaussVal 	gauss 	6.0
kGaussVal2	gauss	100

seed 0
kRand random 2.2, 10.8
kRand2 random 3, 20  

kMetVal		metro   	0.5,	0.00000001		
kTrigVal	samphold	kRand,	kMetVal	
kTrigger 	metro		kTrigVal 

kMinTim		= 0 
kMaxNum 	= 1
kInsNum 	= 6 
kWhen 		= 2
gkDur 		= kRand2 
kSpeed 		= kFileSpeed + kGaussVal
kGrainFreq 	= p4 + kGaussVal
kGrainDurFactor = kGrainDurFactor + kGaussVal2
kCentCalc 	= kGrainFreq + kGaussVal
kPosRand 	= 100 + kGaussVal
kCentRand 	= kCentCalc + kGaussVal 
kPanCalc 	= 1
kDist 		= 0.7 

schedkwhen kTrigger, kMinTim, kMaxNum, kInsNum, kWhen, gkDur, kSpeed, kGrainFreq, kGrainDurFactor, kCentCalc, kPosRand, kCentRand, kPanCalc, kDist 

endin

; **********************************************************************************************
; uses the files "24cellRow.wav" "8cellRow.wav" & "5cellRow.wav" 
; original partikkel example by Joachim Heintz and Oeyvind Brandtsegg 2008
; **********************************************************************************************
instr ClickPopStatic, 6	;	Synth that creates a textures that clicks, pops and sounds 
			;	like static. Uses partikkel.
; **********************************************************************************************

/*score parameters*/
ispeed			= p4		; 1 = original speed 
igrainfreq		= p5		; grain rate
igraindurfactor 	= p6		; grain size in ms
icent 	 		= p7		; transposition in cent
iposrand		= p8		; time position randomness (offset) of the pointer in ms
icentrand		= p9		; transposition randomness in cents
ipan			= p10		; panning narrow (0) to wide (1)
idist			= p11		; grain distribution (0=periodic, 1=scattered)

kduration = (0.5/igrainfreq)*igraindurfactor

/*get length of source wave file, needed for both transposition and time pointer*/
ifilen			tableng	giFile
ifildur			= ifilen / sr

/*sync input (disabled)*/
async			= 0		

/*grain envelope*/
kenv2amt		= 0.5		; use only secondary envelope
ienv2tab 		= giWin		; grain (secondary) envelope
ienv_attack		= giAttack 		; default attack envelope 
ienv_decay		= giDecay 		; default decay envelope 
ksustain_amount		= 0.8		; sustain for fraction of grain length
ka_d_ratio		= 0.75 		; no meaning in this case (use only secondary envelope, ienv2tab)

/*amplitude*/
kamp			= ampdbfs(0)	; grain amplitude
igainmasks		= -1		; gain masking

/*transposition*/
kcentrand		rand icentrand	; random transposition
iorig			= 1 / ifildur	; original pitch
kwavfreq		= iorig; * cent(icent + kcentrand)

/*other pitch related (disabled)*/
ksweepshape		= 0;0.8		; no frequency sweep
iwavfreqstarttab 	= -1;giWavFreqStart		; frequency sweep start
iwavfreqendtab		= -1;giWavFreqEnd		; frequency sweep end

;aEnv	linseg	0, p3 * 0.2, 1, p3 * 0.1, 0.8, p3 * 0.5, 0.8, p3 * 0.2, 0
;kCps	linseg	200, p3 * 0.8, 500,  p3 * 0.2, 100
;aSig	oscili aEnv, kCps, giFMWave	

awavfm			= 0;aSig 
ifmamptab		= -1;giFmIndex		; FM scaling 
kfmenv			= -1;giFmEnv		; FM envelope 

/*trainlet related (disabled)*/
icosine			= giCosine	; cosine ftable
kTrainCps		= igrainfreq; set trainlet cps equal to grain rate for single-cycle trainlet in each grain
knumpartials		= 1		; number of partials in trainlet
kchroma			= 1		; balance of partials in trainlet

/*panning, using channel masks*/
imid			= .5; center
ileftmost		= imid - ipan/2
irightmost		= imid + ipan/2
;giPanthis		ftgen	0, 0, 32768, -24, giPan, ileftmost, irightmost	; rescales giPan according to ipan
			;tableiw		0, 0, giPanthis				; change index 0 ...
			;tableiw		32766, 1, giPanthis			; ... and 1 for ichannelmasks
ichannelmasks		= -1 ;giPanthis		; ftable for panning

/*random gain masking*/
krandommask		= 0.2	

/*source waveforms*/
kwaveform1		= giFile; source waveforms
kwaveform2		= giFile
kwaveform3		= giFile
kwaveform4		= giFile
iwaveamptab		= -1; mix of source waveforms and trainlets

/*time pointer*/
afilposphas		phasor ispeed / ifildur

/*generate random deviation of the time pointer*/
iposrandsec		= iposrand / 1000	; ms -> sec
iposrand		= iposrandsec / ifildur	; phase values (0-1)
krndpos			linrand	 iposrand	; random offset in phase values
;kGaussVal		gauss	20.0

/*add random deviation to the time pointer*/
asamplepos1		= afilposphas + krndpos; resulting phase values (0-1)
asamplepos2		= asamplepos1 + krndpos; + kGaussVal
asamplepos3		= asamplepos2 + krndpos; + kGaussVal	
asamplepos4		= asamplepos1 + krndpos; + kGaussVal	

/*original key for each source waveform*/
kwavekey1		= 1
kwavekey2		= 0.5 
kwavekey3		= 1.32 
kwavekey4		= 0.66 

/* maximum number of grains per k-period*/
imax_grains		= 3000		

aOut		partikkel igrainfreq, idist, giDisttab, async, kenv2amt, ienv2tab, \
		ienv_attack, ienv_decay, ksustain_amount, ka_d_ratio, kduration, kamp, igainmasks, \
		kwavfreq, ksweepshape, iwavfreqstarttab, iwavfreqendtab, awavfm, \
		ifmamptab, kfmenv, icosine, kTrainCps, knumpartials, \
		kchroma, ichannelmasks, krandommask, kwaveform1, kwaveform2, kwaveform3, kwaveform4, \
		iwaveamptab, asamplepos1, asamplepos2, asamplepos3, asamplepos4, \
		kwavekey1, kwavekey2, kwavekey3, kwavekey4, imax_grains

aOutEnv	linseg	0, p3 * 0.05, 1, p3 * 0.05, 0.85, p3 * 0.8, 0.85, p3 * 0.1, 0

gaParticleOut = aOut * aOutEnv
	;outs	gaParticleOut, gaParticleOut

endin

;**************************************************************************************
instr DenseGrain, 7	;	Synth that produces a dense grain cloud using grain3 
;**************************************************************************************

;kCps	chnget	"grainFreq"
;kPhs	chnget	"grainPhase"
;kFmd	chnget	"randFreq"
;kPmd	chnget	"randPhase"
;kGDur	chnget	"grainDur"
;kDens	chnget	"grainDensity"
;kFrPow	chnget	"grainFreqVariationDistrib"
;kPrPow	chnget	"grainPhaseVariationDistrib"
;kFn	chnget	"grainWaveform"

;kGDur = 0.01 + kGDur ; initialisation to avoid perf error 0.0
;kDens = 1 + kDens

; get control value from application
;kSineControlVal	chnget	"sineControlVal"
;kCps = kCps * kSineControlVal + 20

  kCps    =       100
  kPhs    =       0
  ;kFmd    transeg 0,1,0,0, 10,4,15, 10,-4,0
  kFmd	= 3
  ;kPmd    transeg 0,1,0,0, 10,4,1,  10,-4,0
  kPmd	= 7
  kGDur   =       0.08
  kDens   =       200
  iMaxOvr =       1000
  kFn     =       giWave
  kFrPow = 0
  kPrPow = 0
  ;print info. to the terminal
          ;printks "Random Phase:%5.2F%TPitch Random:%5.2F%n",1,kPmd,kFmd
	;printks "Grain Density:%f%n", 1, kDens
  aOut8    grain3  kCps, kPhs, kFmd, kPmd, kGDur, kDens, iMaxOvr, kFn, giWFn, kFrPow, kPrPow

aOutEnv	linseg	0, p3 * 0.05, 1, 0.05, 0.95, 0.8, 0.95, 0.1, 0

gaOut8 = aOut8 * 0.2; * aOutEnv

	outs	gaOut8, gaOut8
endin

;**************************************************************************************
  instr GranulatedRainTrigger, 8 	;	Triggers instr 8 which produces a 
  					;	granular swishing type texture.
;**************************************************************************************

kFreq		chnget		"noteFreq"
kNoteLen	chnget		"noteLength"
kWSize		chnget		"winSize"
kWSize = floor(kWSize)
kResample	chnget		"resampleVal"

;kFreq     	random  	2, 	25 
kMetVal		metro   	0.2,	0.00000001		
kTrigVal	samphold	kFreq,	kMetVal	

;kTrigger	metro		kTrigVal
kTrigger	metro		kFreq	

;kNoteLen	random		0.05,		0.2
;kWSize		random		80,		85

schedkwhen kTrigger,0,0,9,0,kNoteLen,kWSize,kResample ;trigger instr. 2 for 40s

  endin

;**************************************************************************************
  instr GranulatedRain, 9 	;	Synth that produces a granular swishing type texture using
  				;	sndwarpst.

  				;	adapted by Bryan Dunphy, from 
  				;	http://floss.booktype.pro/csound/g-granular-synthesis/, 
				;	example 05G02_selfmade_grain.csd written by Iain McCurdy
;**************************************************************************************

;define the input variables
ifn1        =          	giSound
ilen        =          	nsamp(ifn1)/sr
iPtrStart   random     	1,ilen-1
iPtrTrav    random     	-1,1
ktimewarp   line       	iPtrStart,p3,iPtrStart+iPtrTrav
kamp        linseg     	0,p3/2,0.5,p3/2,0
;iresample   random     	-24,24.99
;iresample   =          	semitone(int(iresample))
iresample   =          	semitone(int(p5))
ifn2        =          	giWFnGranRain
ibeg        =          	0
;iwsize     random     	20,50000
iwsize      =          	p4
irandw      =          	iwsize/3
ioverlap    =		8 
itimemode   =          	1

; create a stereo granular synthesis texture using sndwarp
aSig sndwarp  kamp,ktimewarp,iresample,ifn1,ibeg,\
                              iwsize,irandw,ioverlap,ifn2,itimemode

; envelope the signal with a lowpass filter
kcf         expseg     50,p3/2,8000,p3/2,50
gaGranulatedRainDrySig moogvcf2    aSig, kcf, 0.5

; add a little of our audio signals to the global send variables -
; - these will be sent to the reverb instrument (2)
gaSend     =          gaSend + gaGranulatedRainDrySig

            ;outs       gaGranulatedRainDrySig,gaGranulatedRainDrySig
  endin

;**************************************************************************************
  instr GranularRainReverb, 10 ;	Reverb for GranulatedRain 
;**************************************************************************************

kRevFeedback	chnget "reverbFeedback"
kRevCutoff	chnget "reverbCutoff"

;aRvbL,aRvbR reverbsc   gaSend,gaSend,0.6,4000
aRvbL,aRvbR reverbsc   gaSend,gaSend,kRevFeedback,kRevCutoff

gaGranularRainReverbOut = aRvbL

            ;outs       aRvbL,aRvbR

;clear variables to prevent out of control accumulation
            clear      gaSend

  endin

;**************************************************************************************
instr KarplusStrongTrigger, 11	;	Triggers instr 11 which is a bask Karplus-Strong synth. 
;**************************************************************************************

;kElapsedTime	timeinsts

;kMetaWindow	=	kElapsedTime % 5

;if kMetaWindow == 0 then
;	kTimeMod	random	1, 5
;endif

;kWindow		=	kElapsedTime % kTimeMod 

;if kWindow == 0 then
;	kfreq      random  0.08, 7
;	kCutOffFreq	random	200, 1200
;	kRes	random	0.1, 0.8
;endif

kFreq     	random  	0.08, 	7 
kMetVal		metro   	0.2,	0.00000001		
kTrigVal	samphold	kFreq,	kMetVal	

ktrigger   metro   kTrigVal	

kNoteLen   random  0.075, 0.15

kCutOffFreq	random	200, 1200
kRes	random	0.1, 0.8
kDelayTime random  0.005, 0.0075

kFeedbackAmnt = 0.9

kAtt = kNoteLen * 0.0001
kDec = kNoteLen * 0.025
kSus = 0.975
kRel = kNoteLen * 0.75

schedkwhen ktrigger,0,0,12,0,kNoteLen, kDelayTime, kFeedbackAmnt, kCutOffFreq, kRes, kAtt, kDec, kSus, kRel ;trigger instr. 2 for kNoteLen 

  endin

;***********************************************************************************************
instr KarplusStrong, 12	;	Karplus-Strong Synth (from Joachim Heintz and Martin Neukom - 04G08_Plucked.csd : 
			;	http://floss.booktype.pro/csound/g-physical-modelling/)
;***********************************************************************************************
;delay time
iDelTm    =        p4 
;fill the delay line with either -1 or 1 randomly
kDur      timeinsts
 if kDur < iDelTm then
aFill     rand      1, 2, 1, 1 ;values 0-2
aFill     =         floor(aFill)*2 - 1 ;just -1 or +1
          else
aFill     =         0
 endif
;delay and feedback
aUlt      init      0 ;last sample in the delay line
aUlt1     init      0 ;delayed by one sample
aMean     =         (aUlt+aUlt1)/2 ;mean of these two
aUlt      delay     aFill+aMean, iDelTm
aUlt1     delay1    aUlt

aDelOut init 0
kFeedback = p5

aDelOut	delay	aUlt + (aDelOut * kFeedback), 0.02 

kCofFreq = p6
kResAmnt = p7

aFiltOut	moogvcf	aDelOut, kCofFreq, kResAmnt 

iAtt = p8
iDec = p9
iSus = p10
iRel = p11

aEnv	adsr	iAtt, iDec, iSus, iRel

aLimOut	limit	aFiltOut * aEnv, -0.8, 0.8 

        ;  outs      aLimOut, aLimOut 

;iRevSendAmnt	=	0.8
gaCompSend	=	gaCompSend + aLimOut

endin

;********************************************************************************
instr KarplusStrongCompressor, 13 ; Karplus-Strong Compressor
;********************************************************************************

kThresh	=	-90
kLoKnee	=	-52
kHiKnee	=	-30
kRatio	=	5
kAtt	=	0.01
kRel	=	0.1
iLook	=	0.05

aCompSig	compress2	gaCompSend, gaCompSend, kThresh, kLoKnee, kHiKnee, kRatio, kAtt, kRel, iLook

	outs	aCompSig, aCompSig
	clear	gaCompSend

iRevSendAmnt	=	0.8
gaReverbSend	=	gaReverbSend + (aCompSig * iRevSendAmnt)

endin

;********************************************************************************
instr KarplusStrongReverb, 14	; Karplus-Strong Reverb 
;********************************************************************************

kRoomSize	init	0.25
kHFDamping	init	0.75

aRevL, aRevR	freeverb	gaReverbSend, gaReverbSend, kRoomSize, kHFDamping

	outs	aRevL, aRevR
	clear	gaReverbSend
endin

;**************************************************************************************
  instr KickDrumTrigger, 15 	;	Triggers instr 15 which produces a 
  				;	synthesised kick drum sound.
;**************************************************************************************

kFreq     	random  	2, 	25 
kMetVal		metro   	0.2,	0.00000001		
kTrigVal	samphold	kFreq,	kMetVal	

kTrigger	metro		kTrigVal

schedkwhen kTrigger,0,0,16,0,0.5

  endin

;********************************************************************************
;instr KickDrum, 16 ; Drum Synth
;********************************************************************************

;iFn		init	giSquare
;kAmp		init	1	
;kFreq		init	30	
;
;aOscSig		oscil	kAmp, kFreq, iFn
;
;kBeta		init	2	
;aNoiseSig	fractalnoise	kAmp, kBeta
;
;kCutOff		init	47	
;kRes		init	1.75
;
;aFiltSig	moogvcf	aOscSig + aNoiseSig, kCutOff, kRes
;
;kRvt		init	0.25	
;aRevOut		reverb	aFiltSig, kRvt
;
;iNoteLen	init	p3
;iAtt		init	0.000001
;iDec		init	0.1
;iSus		init	0.01	
;iRel		init	iNoteLen * 0.1
;
;aEnv		adsr	iAtt, iDec, iSus, iRel
;
;	outs	aRevOut * aEnv, aRevOut * aEnv
;
;endin


;**************************************************************************************
instr SpectralAnalysis, 17	; Spectral Analysis 
;**************************************************************************************

ifftsize = 1024 
ioverlap = ifftsize / 4
iwinsize = ifftsize * 2
iwinshape = 0

aSig  = gaParticleOut 
;aSig1 = gaModalSamplerOut

; route output from instruments above to pvsanal
fsig	pvsanal	aSig,	ifftsize,	ioverlap,	iwinsize,	iwinshape
;fsig1	pvsanal	aSig1,	ifftsize,	ioverlap,	iwinsize,	iwinshape

kcent	pvscent	fsig
	chnset	kcent,	"specCentOut"

kFreq,	kAmp	pvspitch	fsig,	0.01
	chnset	kFreq,	"freqOut"
	chnset	kAmp,	"ampOut"

;kFreq1,	kAmp1	pvspitch	fsig1,	0.01
;	chnset	kFreq1,	"modSamp_specFreq"
;	chnset	kAmp1,	"modSamp_ampOut"

; get info from pvsanal and print
;ioverlap,	inbins,	iwindowsize,	iformat	pvsinfo	fsig
;print	ioverlap,	inbins,	iwindowsize,	iformat		

; create tables to write frequency data
;iFreqTable	ftgen	0,	0,	inbins,	2,	0
;iAmpTable	ftgen	0,	0,	inbins,	2,	0
;
;; write frequency data to function table
;kFlag	pvsftw	fsig,	iAmpTable,	iFreqTable	
;
; if kFlag == 0 goto contin 
;
;;************** Frequency Processing *****************
;
;; modify frequency data from fsig with mandelbulb escape values from application
;kCount = 0
;
;loop:
;
;	; read amplitude data from iAmpTable
;	kAmp	tablekt	kCount,	iAmpTable
;
;	; send val out to application
;	S_ChannelName	sprintfk	"fftAmpBin%d",	kCount
;	chnset	kAmp,	S_ChannelName
;	
;	loop_lt	kCount,	1,	inbins,	loop
;
;contin:

endin

;**************************************************************************************
instr SoundLocaliser, 18	; Sound Localisation using hrtf2
;**************************************************************************************
kPortTime linseg 0.0, 0.001, 0.05 

iNumAudioSources init	2 

kAzimuths[] 	init 	iNumAudioSources
kElevations[] 	init	iNumAudioSources
kDistances[]	init	iNumAudioSources

kAzimuths[0]	chnget	"azimuth0"
kAzimuths[1]	chnget	"azimuth1"
kElevations[0]	chnget	"elevation0"
kElevations[1]	chnget	"elevation1"
kDistances[0]	chnget	"distance0"
kDistances[1]	chnget	"distance1"

;iCount init 0

;channelLoop:

	;S_azimuth sprintfk "azimuth%d", iCount 
	;kAzimuths[iCount] 	chnget "S_azimuth"
	;prints "%s\n", S_azimuth

	;S_elevation sprintfk "elevation%d", iCount 
	;;kElevations[kInd] 	chnget S_elevation 

	;S_distance sprintfk "distance%d", iCount 
	;;kDistances[kInd]	chnget S_distance 

	;loop_lt	iCount, 1, iNumAudioSources, channelLoop
	
aInstSigs[]	init	iNumAudioSources
aInstSigs[0] =	(gaGranulatedRainDrySig * 0.4) + (gaGranularRainReverbOut * 0.6) 
aInstSigs[1] =	gaParticleOut * 5.5

aLeftSigs[]	init	iNumAudioSources
aRightSigs[]	init	iNumAudioSources
kDistVals[]	init	iNumAudioSources

kDistVals[0] portk kDistances[0], kPortTime	;to filter out audio artifacts due to the distance changing too quickly
kDistVals[1] portk kDistances[1], kPortTime	;to filter out audio artifacts due to the distance changing too quickly
	
aLeftSigs[0], aRightSigs[0]  hrtfmove2	aInstSigs[0], kAzimuths[0], kElevations[0], "hrtf-48000-left.dat", "hrtf-48000-right.dat", 4, 9.0, 48000
kDistSquared0	pow	kDistVals[0], 2
aLeftSigs[0] = aLeftSigs[0] / kDistSquared0 ;(kDistVals[0] + 0.00001)
aRightSigs[0] = aRightSigs[0] / kDistSquared0 ;(kDistVals[0] + 0.00001)

aLeftSigs[1], aRightSigs[1]  hrtfmove2	aInstSigs[1], kAzimuths[1], kElevations[1], "hrtf-48000-left.dat", "hrtf-48000-right.dat", 4, 9.0, 48000
kDistSquared1	pow	kDistVals[1], 2
aLeftSigs[1] = aLeftSigs[1] / kDistSquared1 
aRightSigs[1] = aRightSigs[1] / kDistSquared1 ;(kDistVals[1] + 1);0.00001)

aL init 0
aR init 0

aL sum	aLeftSigs[0], aLeftSigs[1]
aR sum	aRightSigs[0], aRightSigs[1]

;aL = aL / iNumAudioSources
;aR = aR / iNumAudioSources

outs	aL,	aR
endin

</CsInstruments>
<CsScore>

;********************************************************************
; f tables
;********************************************************************
;p1	p2	p3	p4	p5	p6	p7	p8	p9	p10	p11	p12	p13	p14	p15	p16	p17	p18	p19	p20	p21	p22	p23	p24	p25

f0	86400 ;keep csound running for a day

;f1	0	1025	8	0	2	1	3	0	4	1	6	0	10	1	12	0	16	1	32	0	1	0	939	0

;********************************************************************
; score events
;********************************************************************

i "ModalSynth"			0	6	1	0

i "ModalSamplerTrigger"		7	-1

;i1	8	6	2	300

;i4	0	-1	8	

;i6.01	1	-1	0
;i6.02	2	-1	50.0	

i "GranulatedRainTrigger"	2	-1

i "GranularRainReverb"		2	-1

i "ClickPopStaticTrigger"	2	-1	20	

i "SpectralAnalysis"		2	-1

i "SoundLocaliser"		2	-1

;i "KarplusStrongTrigger"	2	-1
;i "KarplusStrongCompressor"	2	-1
;i "KarplusStrongReverb"		2	-1

;i "KickDrumTrigger"	0	-1
e
</CsScore>
</CsoundSynthesizer>
