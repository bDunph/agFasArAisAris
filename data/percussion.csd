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

seed	0

; Generate a sawtooth wave
giSaw		ftgen	0, 0, 16384, 10, 1, 0.5, 0.3, 0.25, 0.2, 0.167, 0.14, 0.125, 0.111
giSquare	ftgen	0, 0, 16384, 10, 1, 0, 0.3, 0, 0.2, 0, 0.14, 0, 0.111	

;********************************************************************************
instr KickDrum, 1 ; Drum Synth
;********************************************************************************

iFn		init	giSquare
kAmp		init	1	
kFreq		init	30	

aOscSig		oscil	kAmp, kFreq, iFn

kBeta		init	2	
aNoiseSig	fractalnoise	kAmp, kBeta

kCutOff		init	47	
kRes		init	1.75

aFiltSig	moogvcf	aOscSig + aNoiseSig, kCutOff, kRes

kRvt		init	0.25	
aRevOut		reverb	aFiltSig, kRvt

iNoteLen	init	p3
iAtt		init	0.000001
iDec		init	0.1
iSus		init	0.01	
iRel		init	iNoteLen * 0.1

aEnv		adsr	iAtt, iDec, iSus, iRel

	outs	aRevOut * aEnv, aRevOut * aEnv

endin

</CsInstruments>
<CsScore>
i 1	0	0.5	
i 1	+	.		
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
i 1	+	.	
</CsScore>
</CsoundSynthesizer>
