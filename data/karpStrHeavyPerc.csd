<CsoundSynthesizer>
<CsOptions>
-odac
</CsOptions>
<CsInstruments>
sr = 48000
kr = 480
ksmps =  100
nchnls = 2
0dbfs = 1

seed 0	;seend the random generators from the system clock

gaReverbSend	init	0	;global reverb send variable
gaCompSend	init	0	;global compression send variable

;**************************************************************************************
instr 1; Karplus-Strong Trigger 
;**************************************************************************************

kElapsedTime	timeinsts

kMetaWindow	=	kElapsedTime % 5

if kMetaWindow == 0 then
	kTimeMod	random	1, 5
endif

kWindow		=	kElapsedTime % kTimeMod 

if kWindow == 0 then
	kfreq      random  0.08, 7
	kCutOffFreq	random	200, 1200
	kRes	random	0.1, 0.8
endif

ktrigger   metro   kfreq         ;metronome of triggers. One every 12.5s
kNoteLen   random  0.075, 0.15
kDelayTime random  0.005, 0.0075

kFeedbackAmnt = 0.9

kAtt = kNoteLen * 0.0001
kDec = kNoteLen * 0.025
kSus = 0.975
kRel = kNoteLen * 0.75

schedkwhen ktrigger,0,0,2,0,kNoteLen, kDelayTime, kFeedbackAmnt, kCutOffFreq, kRes, kAtt, kDec, kSus, kRel ;trigger instr. 2 for kNoteLen 

  endin


;***********************************************************************************************
instr 2	; 	Karplus-Strong Synth (from Joachim Heintz and Martin Neukom - 04G08_Plucked.csd : 
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
instr 3	; Reverb 
;********************************************************************************

kRoomSize	init	0.25
kHFDamping	init	0.75

aRevL, aRevR	freeverb	gaReverbSend, gaReverbSend, kRoomSize, kHFDamping

	outs	aRevL, aRevR
	clear	gaReverbSend
endin

;********************************************************************************
instr 4 ; Compressor
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

</CsInstruments>
<CsScore>
i 1	0	20
i 3	0	20
i 4	0	20
</CsScore>
</CsoundSynthesizer>
;example by joachim heintz, after martin neukom
