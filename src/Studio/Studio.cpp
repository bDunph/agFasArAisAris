#include "Studio.hpp"

#include <iostream>
#include <random>
#include <math.h>

#ifdef __APPLE__ 
#include "GLFW/glfw3.h"
#elif _WIN32 
#include "glfw3.h"
#endif

#define PI 3.141592653589793

//*******************************************************************************************
// Setup 
//*******************************************************************************************
bool Studio::Setup(std::string csd, GLuint shaderProg)
{
	// bool to indicate first loop through update and draw functions to 
	// set initial paramaters. For reading from pboInfo in update
	m_bFirstLoop = true;
	m_fDeltaTime = 0.0f;
	m_fTargetVal = 0.0f;
	m_fPrevTargetVal = 0.0f;
	m_fCurrentVal = 0.0f;
	m_iBufSize = 20;
	for(int i = 0; i < m_iBufSize; i++)
	{
		m_dFbmAmpBuf.push_front(0.0);
		m_dFbmSpeedBuf.push_front(0.0);
		m_dAmpOutVals.push_front(0.0);
	}

	m_iSphereNum = 0;
	m_bPrevRightNNState = false;
	m_bPrevLeftNNState = false;
	m_bRightNNToggle = false;
	m_bLeftNNToggle = false;

	//m_vec3SpherePos1 = glm::vec3(7.79998f, -4.46797f, 7.72899f); 
	m_vec3SpherePos1 = glm::vec3(6.0f, -1.0f, 6.0f); 
	m_fPrevSpecVal = 0.0;

	m_pStTools = new StudioTools();

	//setup quad to use for raymarching
	m_pStTools->RaymarchQuadSetup(shaderProg);
	
	//shader uniforms
	//m_gliSineControlValLoc = glGetUniformLocation(shaderProg, "sineControlVal");
	//m_gliPitchOutLoc = glGetUniformLocation(shaderProg, "pitchOut");
	m_gliAmpOutLoc = glGetUniformLocation(shaderProg, "ampOut");
	//m_gliFreqOutLoc = glGetUniformLocation(shaderProg, "freqOut");
	m_gliSpecCentVal = glGetUniformLocation(shaderProg, "specCentVal");
	m_gliDisplayRes = glGetUniformLocation(shaderProg, "dispRes");
	m_gliTime = glGetUniformLocation(shaderProg, "time");
	m_gliFbmAmp = glGetUniformLocation(shaderProg, "fbmAmp");
	m_gliFbmSpeed = glGetUniformLocation(shaderProg, "fbmSpeed");
	m_gliFbmAmp_left = glGetUniformLocation(shaderProg, "fbmAmp_left");
	m_gliFbmSpeed_left = glGetUniformLocation(shaderProg, "fbmSpeed_left");
	m_gliSphereNum = glGetUniformLocation(shaderProg, "controlAreaSphereNum");
	m_gliControllerPos = glGetUniformLocation(shaderProg, "controllerPos");
	m_gliFractalAngle = glGetUniformLocation(shaderProg, "fractalAngle");
	m_gliSpherePos1 = glGetUniformLocation(shaderProg, "spherePos1");

	//machine learning setup
	MLRegressionSetup();

	//Right NN parameter setup
	m_pNoteFreq = std::make_unique<RegressionModel::DataInfo>();
	m_pNoteFreq->name = "noteFreq";
	m_pNoteFreq->value = 12.0;
	m_pNoteFreq->normVal = 0.0;
	m_pNoteFreq->minVal = 2.0;
	m_pNoteFreq->maxVal = 25.0;
	m_pNoteFreq->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pNoteFreq));

	m_pNoteLength = std::make_unique<RegressionModel::DataInfo>();
	m_pNoteLength->name = "noteLength";
	m_pNoteLength->value = 0.1;
	m_pNoteLength->normVal = 0.0;
	m_pNoteLength->minVal = 0.05;
	m_pNoteLength->maxVal = 0.9;
	m_pNoteLength->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pNoteLength));	

	m_pWinSize = std::make_unique<RegressionModel::DataInfo>();
	m_pWinSize->name = "winSize";
	m_pWinSize->value = 81.0;
	m_pWinSize->normVal = 0.0;
	m_pWinSize->minVal = 80.0;
	m_pWinSize->maxVal = 800.0;
	m_pWinSize->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pWinSize));

	m_pFbmAmp = std::make_unique<RegressionModel::DataInfo>();
	m_pFbmAmp->name = "fbmAmp";
	m_pFbmAmp->value = 0.5;
	m_pFbmAmp->normVal = 0.0;
	m_pFbmAmp->minVal = 0.0;
	m_pFbmAmp->maxVal = 1.0;
	m_pFbmAmp->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pFbmAmp));

	m_pFbmSpeed = std::make_unique<RegressionModel::DataInfo>();
	m_pFbmSpeed->name = "fbmSpeed";
	m_pFbmSpeed->value = 0.5;
	m_pFbmSpeed->normVal = 0.0;
	m_pFbmSpeed->minVal = 0.0;
	m_pFbmSpeed->maxVal = 3.0;
	m_pFbmSpeed->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pFbmSpeed));

	m_pFractalAngle = std::make_unique<RegressionModel::DataInfo>();
	m_pFractalAngle->name = "fractalAngle";
	m_pFractalAngle->value = 0.0;
	m_pFractalAngle->normVal = 0.0;
	m_pFractalAngle->minVal = 0.0;
	m_pFractalAngle->maxVal = 360.0 * (PI / 180.0);
	m_pFractalAngle->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pFractalAngle));

	m_pRControllerX = std::make_unique<RegressionModel::DataInfo>();
	m_pRControllerX->name = "rControllerX";
	m_pRControllerX->value = 0.0;
	m_pRControllerX->normVal = 0.0;
	m_pRControllerX->minVal = -2.0;
	m_pRControllerX->maxVal = 2.0;
	m_pRControllerX->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pRControllerX));

	m_pRControllerY = std::make_unique<RegressionModel::DataInfo>();
	m_pRControllerY->name = "rControllerY";
	m_pRControllerY->value = 0.0;
	m_pRControllerY->normVal = 0.0;
	m_pRControllerY->minVal = -2.0;
	m_pRControllerY->maxVal = 2.0;
	m_pRControllerY->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pRControllerY));

	m_pRControllerZ = std::make_unique<RegressionModel::DataInfo>();
	m_pRControllerZ->name = "rControllerZ";
	m_pRControllerZ->value = 0.0;
	m_pRControllerZ->normVal = 0.0;
	m_pRControllerZ->minVal = -2.0;
	m_pRControllerZ->maxVal = 2.0;
	m_pRControllerZ->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pRControllerZ));

	mySavedModel = "agFasModel.json";

	//Left NN parameter setup
	m_pModSamp_noteFreq = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_noteFreq->name = "modSamp_noteFreq";
	m_pModSamp_noteFreq->value = 0.8;
	m_pModSamp_noteFreq->normVal = 0.0;
	m_pModSamp_noteFreq->minVal = 0.08;
	m_pModSamp_noteFreq->maxVal = 2.0;
	m_pModSamp_noteFreq->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_noteFreq));

	m_pModSamp_noteLength = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_noteLength->name = "modSamp_noteLength";
	m_pModSamp_noteLength->value = 2.0;
	m_pModSamp_noteLength->normVal = 0.0;
	m_pModSamp_noteLength->minVal = 0.2;
	m_pModSamp_noteLength->maxVal = 5.0;
	m_pModSamp_noteLength->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_noteLength));

	m_pModSamp_winSize = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_winSize->name = "modSamp_winSize";
	m_pModSamp_winSize->value = 5000.0;
	m_pModSamp_winSize->normVal = 0.0;
	m_pModSamp_winSize->minVal = 4800.0;
	m_pModSamp_winSize->maxVal = 24000.0;
	m_pModSamp_winSize->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_winSize));

	m_pModSamp_moogCutoff = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_moogCutoff->name = "modSamp_moogCutoff";
	m_pModSamp_moogCutoff->value = 6000.0;
	m_pModSamp_moogCutoff->normVal = 0.0;
	m_pModSamp_moogCutoff->minVal = 20.0;
	m_pModSamp_moogCutoff->maxVal = 80.0;
	m_pModSamp_moogCutoff->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_moogCutoff));

	m_pModSamp_overlap = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_overlap->name = "modSamp_overlap";
	m_pModSamp_overlap->value = 20.0;
	m_pModSamp_overlap->normVal = 0.0;
	m_pModSamp_overlap->minVal = 100.0;
	m_pModSamp_overlap->maxVal = 500.0;
	m_pModSamp_overlap->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_overlap));

	m_pModSamp_amp = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_amp->name = "modSamp_amp";
	m_pModSamp_amp->value = 0.1;
	m_pModSamp_amp->normVal = 0.0;
	m_pModSamp_amp->minVal = 0.05;
	m_pModSamp_amp->maxVal = 0.2;
	m_pModSamp_amp->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_amp));

	m_pModSamp_moogRes = std::make_unique<RegressionModel::DataInfo>();
	m_pModSamp_moogRes->name = "modSamp_moogRes";
	m_pModSamp_moogRes->value = 0.2;
	m_pModSamp_moogRes->normVal = 0.0;
	m_pModSamp_moogRes->minVal = 0.05;
	m_pModSamp_moogRes->maxVal = 0.4;
	m_pModSamp_moogRes->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pModSamp_moogRes));

	m_pFbmAmp_left = std::make_unique<RegressionModel::DataInfo>();
	m_pFbmAmp_left->name = "fbmAmp_left";
	m_pFbmAmp_left->value = 0.5;
	m_pFbmAmp_left->normVal = 0.0;
	m_pFbmAmp_left->minVal = 0.0;
	m_pFbmAmp_left->maxVal = 10.0;
	m_pFbmAmp_left->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pFbmAmp_left));

	m_pFbmSpeed_left = std::make_unique<RegressionModel::DataInfo>();
	m_pFbmSpeed_left->name = "fbmSpeed_left";
	m_pFbmSpeed_left->value = 0.15;
	m_pFbmSpeed_left->normVal = 0.0;
	m_pFbmSpeed_left->minVal = 0.0;
	m_pFbmSpeed_left->maxVal = 0.5;
	m_pFbmSpeed_left->paramType = RegressionModel::OUTPUT;

	leftNN_outParamVec.push_back(std::move(m_pFbmSpeed_left));

	m_pLControllerX = std::make_unique<RegressionModel::DataInfo>();
	m_pLControllerX->name = "lControllerX";
	m_pLControllerX->value = 0.0;
	m_pLControllerX->normVal = 0.0;
	m_pLControllerX->minVal = -2.0;
	m_pLControllerX->maxVal = 2.0;
	m_pLControllerX->paramType = RegressionModel::INPUT;

	leftNN_inParamVec.push_back(std::move(m_pLControllerX));

	m_pLControllerY = std::make_unique<RegressionModel::DataInfo>();
	m_pLControllerY->name = "lControllerY";
	m_pLControllerY->value = 0.0;
	m_pLControllerY->normVal = 0.0;
	m_pLControllerY->minVal = -2.0;
	m_pLControllerY->maxVal = 2.0;
	m_pLControllerY->paramType = RegressionModel::INPUT;

	leftNN_inParamVec.push_back(std::move(m_pLControllerY));

	m_pLControllerZ = std::make_unique<RegressionModel::DataInfo>();
	m_pLControllerZ->name = "lControllerZ";
	m_pLControllerZ->value = 0.0;
	m_pLControllerZ->normVal = 0.0;
	m_pLControllerZ->minVal = -2.0;
	m_pLControllerZ->maxVal = 2.0;
	m_pLControllerZ->paramType = RegressionModel::INPUT;

	leftNN_inParamVec.push_back(std::move(m_pLControllerZ));

	mySavedModel_left = "agFasModel_left.json";

	//audio setup
	CsoundSession* csSession = m_pStTools->PCsoundSetup(csd);
	
	if(!m_pStTools->BSoundSourceSetup(csSession, NUM_SOUND_SOURCES))
	{
		std::cout << "Studio::setup sound sources not set up" << std::endl;
		return false;
	}

	//setup sends to csound
	std::vector<const char*> sendNames;

	// Pos 0
	sendNames.push_back("noteFreq");
	m_vSendVals.push_back(m_cspNoteFreq);	
	// Pos 1
	sendNames.push_back("noteLength");
	m_vSendVals.push_back(m_cspNoteLength);
	// Pos 2
	sendNames.push_back("winSize");
	m_vSendVals.push_back(m_cspWinSize);
	// Pos 3
	sendNames.push_back("modSamp_noteFreq");
	m_vSendVals.push_back(m_cspModSamp_noteFreq);
	// Pos 4
	sendNames.push_back("modSamp_noteLength");
	m_vSendVals.push_back(m_cspModSamp_noteLength);
	// Pos 5
	sendNames.push_back("modSamp_winSize");
	m_vSendVals.push_back(m_cspModSamp_winSize);
	// Pos 6
	sendNames.push_back("modSamp_moogCutoff");
	m_vSendVals.push_back(m_cspModSamp_moogCutoff);
	// Pos 7 
	sendNames.push_back("modSamp_overlap");
	m_vSendVals.push_back(m_cspModSamp_overlap);
	// Pos 8 
	sendNames.push_back("modSamp_amp");
	m_vSendVals.push_back(m_cspModSamp_amp);
	// Pos 9 
	sendNames.push_back("modSamp_moogRes");
	m_vSendVals.push_back(m_cspModSamp_moogRes);

	m_pStTools->BCsoundSend(csSession, sendNames, m_vSendVals);

	//setup returns from csound 
	std::vector<const char*> returnNames;

	returnNames.push_back("specCentOut");
	m_vReturnVals.push_back(m_pSpecCentOut);

	returnNames.push_back("ampOut");
	m_vReturnVals.push_back(m_pAmpOut);

	m_pStTools->BCsoundReturn(csSession, returnNames, m_vReturnVals);	

	return true;
}
//*******************************************************************************************


//*******************************************************************************************
// Update 
//*******************************************************************************************
void Studio::Update(glm::mat4 viewMat, MachineLearning& machineLearning, glm::vec3 controllerWorldPos_0, glm::vec3 controllerWorldPos_1, glm::quat controllerQuat_0, glm::quat controllerQuat_1, PBOInfo& pboInfo, glm::vec2 displayRes, glm::vec3 translationVec){

	//std::cout << "TRANSLATION VEC: " << translationVec.x << ": " << translationVec.y << ": " << translationVec.z << std::endl;
	// For return values from shader.
	// vec4 for each fragment is returned in the order RGBA. 
	// You have to wait until the 2nd frame to read from the buffer. 
	// At the moment, only the RGB channels are useable when the A channel is set to max.
	// The values returned are unpredictable if A channel is anything other than max.
	//if(!m_bFirstLoop)
	//std::cout << (double)pboInfo.pboPtr[0] << ":" << (double)pboInfo.pboPtr[1] << ":" << (double)pboInfo.pboPtr[2] << ":" << (double)pboInfo.pboPtr[3] << std::endl;

	// spectral pitch data processing
	//m_fCurrentFrame = glfwGetTime();
	//m_fDeltaTime = m_fCurrentFrame - m_fLastFrame;	
	//m_fDeltaTime *= 1000.0f;
	//if(*m_vReturnVals[1] >= 0.0) m_fTargetVal = *m_vReturnVals[1];	
	////if(m_fTargetVal > m_fCurrentVal)
	//if(m_fTargetVal > m_fPrevTargetVal)
	//{
	//	//m_fCurrentVal += m_fDeltaTime;
	//	m_fCurrentVal += 0.2f;
	////} else if(m_fTargetVal <= m_fCurrentVal)
	//} else if(m_fTargetVal <= m_fPrevTargetVal)
	//{
	//	//m_fCurrentVal -= m_fDeltaTime;
	//	m_fCurrentVal -= 0.2f;
	////} else if(m_fTargetVal == m_fCurrentVal)
	//} else if(m_fTargetVal == m_fPrevTargetVal)
	//{
	//	m_fCurrentVal = m_fTargetVal;
	//}
	//if(m_fCurrentVal <= 0.0f) m_fCurrentVal = 0.0f;
	//m_fPitch = m_fCurrentVal;
	//m_fAmpOut = m_fCurrentVal;
	
	//m_fAmpOut = *m_vReturnVals[1] * 1000.0f;
	//m_fPrevTargetVal = m_fTargetVal;
	m_dAmpOutVals.push_front(*m_vReturnVals[1] * 5.0f);
	m_dAmpOutVals.pop_back();
	float ampOutSum = 0.0f;
	for(int i = 0; i < m_iBufSize; i++)
	{
		ampOutSum += m_dAmpOutVals[i];
	}
	m_fAmpOut = ampOutSum / m_iBufSize;

	//std::cout << "AmpOut : " << m_fAmpOut << std::endl;
	
	// granulated rain sound source at origin
	StudioTools::SoundSourceData soundSource1;
	glm::vec4 sourcePosWorldSpace = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	soundSource1.position = sourcePosWorldSpace;

	// clickPopStatic sound source mapped to moving sphere
	
	//float currentSpecVal = *m_vReturnVals[0];
	//if(currentSpecVal > 0.0 && currentSpecVal != m_fPrevSpecVal)
	//{
	//	m_fSpecCentVal = currentSpecVal;
	//}
	//m_fPrevSpecVal = currentSpecVal;
	//m_fSpecCentVal = *m_vReturnVals[0];
	

	//std::cout << "SpecCentVal: " << *m_vReturnVals[0] << std::endl;

	rotY = glm::mat3(
			cos(glfwGetTime() * 0.08f), 	0, 	sin(glfwGetTime() * 0.08f),
			0,				1,	0,
			-sin(glfwGetTime() * 0.08f),	0,	cos(glfwGetTime() * 0.08f));
	m_vec3RotatedSpherePos1 = rotY * m_vec3SpherePos1;

	StudioTools::SoundSourceData soundSource2;
	glm::vec4 sourcePosWorldSpace2 = glm::vec4(m_vec3RotatedSpherePos1.x, m_vec3RotatedSpherePos1.y, m_vec3RotatedSpherePos1.z, 1.0f); 
	soundSource2.position = sourcePosWorldSpace2;
	
	std::vector<StudioTools::SoundSourceData> soundSources;
	soundSources.push_back(soundSource1);
	soundSources.push_back(soundSource2);

	m_pStTools->SoundSourceUpdate(soundSources, viewMat, translationVec);

	//example control signal - sine function
	//sent to shader and csound
	//m_fSineControlVal = sin(glfwGetTime() * 0.33f);
	//*m_vSendVals[0] = (MYFLT)m_fSineControlVal;

	m_fTime = glfwGetTime();
	m_vDisplayRes = displayRes;

	//run machine learning
	//MLAudioParameter paramData;
	//paramData.distributionLow = 400.0f;
	//paramData.distributionHigh = 1000.0f;
	//paramData.sendVecPosition = 1;
	//std::vector<MLAudioParameter> paramVec;
	//paramVec.push_back(paramData);
	//MLRegressionUpdate(machineLearning, pboInfo, paramVec);	
	
	//********************* Activate NN Left and Right **************************
	bool currentRightNNState = machineLearning.bActivateNNRight;
	bool currentLeftNNState = machineLearning.bActivateNNLeft;

	if(currentRightNNState == true && currentRightNNState != m_bPrevRightNNState) m_bRightNNToggle = !m_bRightNNToggle;

	if(currentLeftNNState == true && currentLeftNNState != m_bPrevLeftNNState) m_bLeftNNToggle = !m_bLeftNNToggle;

	//if(m_bRightNNToggle) std::cout << "Right Hand NN Is Active" << std::endl;

	//if(m_bLeftNNToggle) std::cout << "Left Hand NN Is Active" << std::endl;

	m_bPrevRightNNState = currentRightNNState;
	m_bPrevLeftNNState = currentLeftNNState;

	//********** Right Hand Neural Network ********************
	if(m_bRightNNToggle){
		m_bCurrentRandomState = machineLearning.bRandomParams;
		if(machineLearning.bRandomParams != m_bPrevRandomState && machineLearning.bRandomParams == true){

			regMod.randomiseData(outParamVec);
			std::cout << "RANDOMISED WIN SIZE: " << outParamVec[2]->value << std::endl;
				
		}
		m_bPrevRandomState = m_bCurrentRandomState;


		if(machineLearning.bRecord){

			inParamVec[0]->value = controllerWorldPos_1.x;
			inParamVec[1]->value = controllerWorldPos_1.y;
			inParamVec[2]->value = controllerWorldPos_1.z;
			//inParamVec[3]->value = controllerWorldPos_0.x;
			//inParamVec[4]->value = controllerWorldPos_0.y;
			//inParamVec[5]->value = controllerWorldPos_0.z;
			
			//inParamVec[0]->value = 2.43;
			//inParamVec[1]->value = 5.32;
			//inParamVec[2]->value = 4.53;
			//inParamVec[3]->value = 3.21;
			//inParamVec[4]->value = 4.23;
			//inParamVec[5]->value = 4.56;

			regMod.normaliseData(inParamVec);
			regMod.normaliseData(outParamVec);
			regMod.collectData(inParamVec, outParamVec);
			std::cout << "RECORDING" << std::endl;
		}
		machineLearning.bRecord = false;

		m_bCurrentTrainState = machineLearning.bTrainModel;
		if(machineLearning.bTrainModel != m_bPrevTrainState && machineLearning.bTrainModel == true){
			m_bModelTrained = regMod.trainModel();
			std::cout << "MODEL TRAINED: " << m_bModelTrained << std::endl;
		}
		m_bPrevTrainState = m_bCurrentTrainState;

		m_bCurrentRunState = machineLearning.bRunModel;
		if(m_bCurrentRunState && m_bModelTrained)
		{
			//std::cout << "MODEL RUNNING" << std::endl;
			inParamVec[0]->value = controllerWorldPos_1.x;
			inParamVec[1]->value = controllerWorldPos_1.y;
			inParamVec[2]->value = controllerWorldPos_1.z;
			//inParamVec[3]->value = controllerWorldPos_0.x;
			//inParamVec[4]->value = controllerWorldPos_0.y;
			//inParamVec[5]->value = controllerWorldPos_0.z;

			//inParamVec[0]->value = 5.32;
			//inParamVec[1]->value = 1.23;
			//inParamVec[2]->value = 2.53;
			//inParamVec[3]->value = 5.23;
			//inParamVec[4]->value = 3.26;
			//inParamVec[5]->value = 3.05;

			regMod.normaliseData(inParamVec);

			regMod.run(inParamVec, outParamVec);

			regMod.remapData(outParamVec);

			*m_vSendVals[0] = (MYFLT)outParamVec[0]->value;		
			*m_vSendVals[1] = (MYFLT)outParamVec[1]->value;
			*m_vSendVals[2] = (MYFLT)outParamVec[2]->value;
			//m_dFbmAmp = outParamVec[3]->value;
			//m_dFbmSpeed = outParamVec[4]->value;

			//create a running average value for the FBM to counteract jittery motion when model
			//is running
			m_dFbmAmpBuf.push_front(outParamVec[3]->value);
			m_dFbmSpeedBuf.push_front(outParamVec[4]->value);
			m_dFbmAmpBuf.pop_back();
			m_dFbmSpeedBuf.pop_back();
			double ampSum = 0.0;
			double speedSum = 0.0;
			for(int i = 0; i < m_iBufSize; i++)
			{
				ampSum += m_dFbmAmpBuf[i];
				speedSum += m_dFbmSpeedBuf[i];
			}
			m_dFbmAmp = ampSum / m_iBufSize;
			m_dFbmSpeed = speedSum / m_iBufSize;
			m_dFractalAngle = outParamVec[5]->value;

		} 
		else if(!m_bCurrentRunState && m_bModelTrained)
		{
			std::cout << "Model Stopped" << std::endl;
			*m_vSendVals[0] = (MYFLT)outParamVec[0]->value;		
			*m_vSendVals[1] = (MYFLT)outParamVec[1]->value;
			*m_vSendVals[2] = (MYFLT)outParamVec[2]->value;
			m_dFbmAmp = outParamVec[3]->value;
			m_dFbmSpeed = outParamVec[4]->value;
			m_dFractalAngle = outParamVec[5]->value;

		} else if(!m_bCurrentRunState && !m_bModelTrained)
		{
			*m_vSendVals[0] = (MYFLT)outParamVec[0]->value;		
			*m_vSendVals[1] = (MYFLT)outParamVec[1]->value;
			*m_vSendVals[2] = (MYFLT)outParamVec[2]->value;
			m_dFbmAmp = outParamVec[3]->value;
			m_dFbmSpeed = outParamVec[4]->value;
			m_dFractalAngle = outParamVec[5]->value;
		}

		//std::cout << m_dFbmAmp << "	:	" << m_dFbmSpeed << std::endl;
		
		// save model
		m_bCurrentSaveState = m_bPrevSaveState;
		if(machineLearning.bSaveModel!= m_bCurrentSaveState && machineLearning.bSaveModel == true)
		{
			regMod.saveModel(mySavedModel);
		}
		m_bPrevSaveState = machineLearning.bSaveModel;

		// load model
		m_bCurrentLoadState = m_bPrevLoadState;
		if(machineLearning.bLoadModel != m_bCurrentLoadState && machineLearning.bLoadModel == true)
		{
			m_bModelTrained = regMod.loadModel(mySavedModel);	
		}
		m_bPrevLoadState = machineLearning.bLoadModel;
	}

	if(m_bLeftNNToggle){

		m_bCurrentRandomState = machineLearning.bRandomParams;
		if(machineLearning.bRandomParams != m_bPrevRandomState && machineLearning.bRandomParams == true){

			regModLeft.randomiseData(leftNN_outParamVec);
			std::cout << "RANDOMISED LEFT NN PARAMS" << std::endl;
				
		}
		m_bPrevRandomState = m_bCurrentRandomState;


		if(machineLearning.bRecord){

			leftNN_inParamVec[0]->value = controllerWorldPos_0.x;
			leftNN_inParamVec[1]->value = controllerWorldPos_0.y;
			leftNN_inParamVec[2]->value = controllerWorldPos_0.z;
			
			regModLeft.normaliseData(leftNN_inParamVec);
			regModLeft.normaliseData(leftNN_outParamVec);
			regModLeft.collectData(leftNN_inParamVec, leftNN_outParamVec);
			std::cout << "RECORDING LEFT NN" << std::endl;
		}
		machineLearning.bRecord = false;

		m_bCurrentTrainState = machineLearning.bTrainModel;
		if(machineLearning.bTrainModel != m_bPrevTrainState && machineLearning.bTrainModel == true){
			m_bLeftNN_modelTrained = regModLeft.trainModel();
			std::cout << "LEFT NN MODEL TRAINED: " << m_bLeftNN_modelTrained << std::endl;
		}
		m_bPrevTrainState = m_bCurrentTrainState;

		m_bCurrentRunState = machineLearning.bRunModel;
		if(m_bCurrentRunState && m_bLeftNN_modelTrained)
		{
			std::cout << "LEFT MODEL RUNNING" << std::endl;
			leftNN_inParamVec[0]->value = controllerWorldPos_0.x;
			leftNN_inParamVec[1]->value = controllerWorldPos_0.y;
			leftNN_inParamVec[2]->value = controllerWorldPos_0.z;

			regModLeft.normaliseData(leftNN_inParamVec);

			regModLeft.run(leftNN_inParamVec, leftNN_outParamVec);

			regModLeft.remapData(leftNN_outParamVec);

			*m_vSendVals[3] = (MYFLT)leftNN_outParamVec[0]->value;		
			*m_vSendVals[4] = (MYFLT)leftNN_outParamVec[1]->value;
			*m_vSendVals[5] = (MYFLT)leftNN_outParamVec[2]->value;
			*m_vSendVals[6] = (MYFLT)leftNN_outParamVec[3]->value;
			*m_vSendVals[7] = (MYFLT)leftNN_outParamVec[4]->value;
			*m_vSendVals[8] = (MYFLT)leftNN_outParamVec[5]->value;
			*m_vSendVals[9] = (MYFLT)leftNN_outParamVec[6]->value;
			m_dFbmAmp_left = leftNN_outParamVec[7]->value;
			m_dFbmSpeed_left = leftNN_outParamVec[8]->value;


		} 
		else if(!m_bCurrentRunState && m_bLeftNN_modelTrained)
		{
			std::cout << "Left Model Stopped" << std::endl;
			*m_vSendVals[3] = (MYFLT)leftNN_outParamVec[0]->value;		
			*m_vSendVals[4] = (MYFLT)leftNN_outParamVec[1]->value;
			*m_vSendVals[5] = (MYFLT)leftNN_outParamVec[2]->value;
			*m_vSendVals[6] = (MYFLT)leftNN_outParamVec[3]->value;
			*m_vSendVals[7] = (MYFLT)leftNN_outParamVec[4]->value;
			*m_vSendVals[8] = (MYFLT)leftNN_outParamVec[5]->value;
			*m_vSendVals[9] = (MYFLT)leftNN_outParamVec[6]->value;
			m_dFbmAmp_left = leftNN_outParamVec[7]->value;
			m_dFbmSpeed_left = leftNN_outParamVec[8]->value;

		} else if(!m_bCurrentRunState && !m_bLeftNN_modelTrained)
		{
			*m_vSendVals[3] = (MYFLT)leftNN_outParamVec[0]->value;		
			*m_vSendVals[4] = (MYFLT)leftNN_outParamVec[1]->value;
			*m_vSendVals[5] = (MYFLT)leftNN_outParamVec[2]->value;
			*m_vSendVals[6] = (MYFLT)leftNN_outParamVec[3]->value;
			*m_vSendVals[7] = (MYFLT)leftNN_outParamVec[4]->value;
			*m_vSendVals[8] = (MYFLT)leftNN_outParamVec[5]->value;
			m_dFbmAmp_left = leftNN_outParamVec[7]->value;
			m_dFbmSpeed_left = leftNN_outParamVec[8]->value;

		}
		
		// save model
		m_bCurrentSaveState = m_bPrevSaveState;
		if(machineLearning.bSaveModel!= m_bCurrentSaveState && machineLearning.bSaveModel == true)
		{
			regModLeft.saveModel(mySavedModel_left);
		}
		m_bPrevSaveState = machineLearning.bSaveModel;

		// load model
		m_bCurrentLoadState = m_bPrevLoadState;
		if(machineLearning.bLoadModel != m_bCurrentLoadState && machineLearning.bLoadModel == true)
		{
			m_bLeftNN_modelTrained = regModLeft.loadModel(mySavedModel_left);	
		}
		m_bPrevLoadState = machineLearning.bLoadModel;

	}

}
//*********************************************************************************************


//*********************************************************************************************
// Draw 
//*********************************************************************************************
void Studio::Draw(glm::mat4 projMat, glm::mat4 viewMat, glm::mat4 eyeMat, GLuint shaderProg, glm::vec3 translateVec)
{
	m_pStTools->DrawStart(projMat, eyeMat, viewMat, shaderProg, translateVec);
	
	//glUniform1f(m_gliSineControlValLoc, m_fSineControlVal);
	//glUniform1f(m_gliPitchOutLoc, m_fPitch);
	glUniform1f(m_gliAmpOutLoc, m_fAmpOut);
	//glUniform1f(m_gliFreqOutLoc, *m_vReturnVals[1]);
	glUniform2f(m_gliDisplayRes, m_vDisplayRes.x, m_vDisplayRes.y);
	glUniform1f(m_gliTime, m_fTime);
	glUniform1f(m_gliFbmAmp, m_dFbmAmp);
	glUniform1f(m_gliFbmSpeed, m_dFbmSpeed);
	glUniform1f(m_gliFbmAmp_left, m_dFbmAmp_left);
	glUniform1f(m_gliFbmSpeed_left, m_dFbmSpeed_left);
	glUniform1i(m_gliSphereNum, m_iSphereNum);
	glUniform3f(m_gliControllerPos, m_vec3ControllerPos.x, m_vec3ControllerPos.y, m_vec3ControllerPos.z);
	glUniform1f(m_gliFractalAngle, m_dFractalAngle);
	glUniform3f(m_gliSpherePos1, m_vec3RotatedSpherePos1.x, m_vec3RotatedSpherePos1.y, m_vec3RotatedSpherePos1.z);
	glUniform1f(m_gliSpecCentVal, m_fSpecCentVal);

	m_pStTools->DrawEnd();

	// update first loop switch
	m_bFirstLoop = false;
	// set end of frame timestamp
	m_fLastFrame = m_fCurrentFrame;
}
//*********************************************************************************************



//*********************************************************************************************
// Machine Learning 
//*********************************************************************************************
void Studio::MLRegressionSetup()
{
	m_bPrevSaveState = false;
	m_bCurrentSaveState = false;
	m_bPrevRandomState = false;
	m_bCurrentRandomState = false;
	m_bPrevTrainState = false;
	m_bCurrentTrainState = false;
	m_bPrevRunState = false;
	m_bCurrentRunState = false;
	m_bPrevHaltState = false;
	m_bPrevLoadState = false;
	m_bCurrentLoadState = false;
	m_bMsg = true;
	m_bCurrentMsgState = false;
	m_bRunMsg = true;
	m_bCurrentRunMsgState = false;
	sizeVal = 0.0f;
	m_bModelTrained = false;
	m_bLeftNN_modelTrained = false;
	m_bPrevControlAreaMarkerState = false;
}

void Studio::MLRegressionUpdate(MachineLearning& machineLearning, PBOInfo& pboInfo, std::vector<MLAudioParameter>& params)
{

	m_bCurrentRandomState = m_bPrevRandomState;

	// randomise parameters
	if(machineLearning.bRandomParams != m_bCurrentRandomState && machineLearning.bRandomParams == true)
	{
		//random device
		std::random_device rd;

		//random audio params
		for(int i = 0; i < params.size(); i++)
		{

			std::uniform_real_distribution<float> distribution(params[i].distributionLow, params[i].distributionHigh);
			std::default_random_engine generator(rd());
			float val = distribution(generator);
			*m_vSendVals[params[i].sendVecPosition] = (MYFLT)val;		
		}
			
	}
	m_bPrevRandomState = machineLearning.bRandomParams;

	// record training examples
	if(machineLearning.bRecord)
	{
		//shader values provide input to neural network
		for(int i = 0; i < pboInfo.pboSize; i+=pboInfo.pboSize * 0.01)
		{
			inputData.push_back((double)pboInfo.pboPtr[i]);
		}

		//neural network outputs to audio engine 
		for(int i = 0; i < params.size(); i++)
		{
			outputData.push_back((double)*m_vSendVals[params[i].sendVecPosition]);
		}

		trainingData.input = inputData;
		trainingData.output = outputData;
		trainingSet.push_back(trainingData);

		std::cout << "Recording Data" << std::endl;
		inputData.clear();
		outputData.clear();
	}
	machineLearning.bRecord = false;

	// train model
	m_bCurrentTrainState = m_bPrevTrainState;
	if(machineLearning.bTrainModel != m_bCurrentTrainState && machineLearning.bTrainModel == true && trainingSet.size() > 0)
	{

		staticRegression.train(trainingSet);
		m_bModelTrained = true;
		std::cout << "Model Trained" << std::endl;
	}	
	else if(machineLearning.bTrainModel != m_bCurrentTrainState && machineLearning.bTrainModel == true && trainingSet.size() == 0)
	{
		std::cout << "Can't train model. No training data." << std::endl;
	}

	m_bPrevTrainState = machineLearning.bTrainModel;

	//run and halt model
	if(machineLearning.bDevMode)
	{
		bool currentHaltState = m_bPrevHaltState;
		if(machineLearning.bRunModel && !machineLearning.bHaltModel && m_bModelTrained)
		{
			std::vector<double> modelOut;
			std::vector<double> modelIn;

			for(int i = 0; i < pboInfo.pboSize; i+=pboInfo.pboSize * 0.01)
			{
				modelIn.push_back((double)pboInfo.pboPtr[i]); 
			}
			
			modelOut = staticRegression.run(modelIn);
			
			for(int i = 0; i < modelOut.size(); i++)
			{
				if(modelOut[i] > params[i].distributionHigh) modelOut[i] = params[i].distributionHigh;
				if(modelOut[i] < params[i].distributionLow) modelOut[i] = params[i].distributionLow;
				*m_vSendVals[params[i].sendVecPosition] = (MYFLT)modelOut[i];
			}
			
			std::cout << "Model Running" << std::endl;
			modelIn.clear();
			modelOut.clear();
		} 
		else if(!machineLearning.bRunModel && machineLearning.bHaltModel != currentHaltState)
		{

			std::cout << "Model Stopped" << std::endl;
		}
		m_bPrevHaltState = machineLearning.bHaltModel;
	} else 
	{
		if(machineLearning.bRunModel && m_bModelTrained)
		{
			std::vector<double> modelOut;
			std::vector<double> modelIn;

			for(int i = 0; i < pboInfo.pboSize; i+=pboInfo.pboSize * 0.01)
			{
				modelIn.push_back((double)pboInfo.pboPtr[i]); 
			}

			modelOut = staticRegression.run(modelIn);
			
			for(int i = 0; i < modelOut.size(); i++)
			{
				if(modelOut[i] > params[i].distributionHigh) modelOut[i] = params[i].distributionHigh;;
				if(modelOut[i] < params[i].distributionLow) modelOut[i] = params[i].distributionLow;
				*m_vSendVals[params[i].sendVecPosition] = (MYFLT)modelOut[i];
			}

			bool prevRunMsgState = m_bCurrentRunMsgState;
			if(m_bRunMsg != prevRunMsgState && m_bRunMsg == true)
			{
				std::cout << "Model Running" << std::endl;
				m_bRunMsg = !m_bRunMsg;
			}
			m_bCurrentRunMsgState = m_bRunMsg;

			modelIn.clear();
			modelOut.clear();
			m_bMsg = true;
		} 
		else if(!machineLearning.bRunModel)
		{
			bool prevMsgState = m_bCurrentMsgState;
			if(m_bMsg != prevMsgState && m_bMsg == true)
			{
				std::cout << "Model Stopped" << std::endl;
				m_bMsg = !m_bMsg;
			}
			m_bCurrentMsgState = m_bMsg;
			m_bRunMsg = true;
		}
	}
		
	// save model
	std::string mySavedModel_old = "mySavedModel_cyclicalEx.json";
	m_bCurrentSaveState = m_bPrevSaveState;
	if(machineLearning.bSaveModel!= m_bCurrentSaveState && machineLearning.bSaveModel == true)
	{
		staticRegression.writeJSON(mySavedModel_old);
		std::cout << "Saving Training Data" << std::endl;
	}
	m_bPrevSaveState = machineLearning.bSaveModel;

	// load model
	m_bCurrentLoadState = m_bPrevLoadState;
	if(machineLearning.bLoadModel != m_bCurrentLoadState && machineLearning.bLoadModel == true)
	{
		staticRegression.reset();
		staticRegression.readJSON(mySavedModel_old);	
		m_bModelTrained = true;
		std::cout << "Loading Data and Training Model" << std::endl;
	}
	m_bPrevLoadState = machineLearning.bLoadModel;
}
//*********************************************************************************************


//*********************************************************************************************
// Clean up
//*********************************************************************************************
void Studio::Exit(){

	//delete StudioTools pointer
	m_pStTools->Exit();
	delete m_pStTools;

	//close GL context and any other GL resources
	glfwTerminate();
}
