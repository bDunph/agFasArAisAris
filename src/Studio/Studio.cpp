#include "Studio.hpp"

#include <iostream>
#include <random>

#ifdef __APPLE__ 
#include "GLFW/glfw3.h"
#elif _WIN32 
#include "glfw3.h"
#endif

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
	m_fCurrentVal = 0.0f;

	m_pStTools = new StudioTools();

	//setup quad to use for raymarching
	m_pStTools->RaymarchQuadSetup(shaderProg);
	
	//shader uniforms
	m_gliSineControlValLoc = glGetUniformLocation(shaderProg, "sineControlVal");
	m_gliPitchOutLoc = glGetUniformLocation(shaderProg, "pitchOut");
	m_gliFreqOutLoc = glGetUniformLocation(shaderProg, "freqOut");
	m_gliDisplayRes = glGetUniformLocation(shaderProg, "dispRes");
	m_gliTime = glGetUniformLocation(shaderProg, "time");
	m_gliFbmAmp = glGetUniformLocation(shaderProg, "fbmAmp");
	m_gliFbmSpeed = glGetUniformLocation(shaderProg, "fbmSpeed");
	
	//machine learning setup
	MLRegressionSetup();

	//parameter setup
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
	m_pNoteLength->maxVal = 0.2;
	m_pNoteLength->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pNoteLength));	

	m_pWinSize = std::make_unique<RegressionModel::DataInfo>();
	m_pWinSize->name = "winSize";
	m_pWinSize->value = 81.0;
	m_pWinSize->normVal = 0.0;
	m_pWinSize->minVal = 80.0;
	m_pWinSize->maxVal = 85.0;
	m_pWinSize->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pWinSize));

	m_pFbmAmp = std::make_unique<RegressionModel::DataInfo>();
	m_pFbmAmp->name = "fbmAmp";
	m_pFbmAmp->value = 0.5;
	m_pFbmAmp->normVal = 0.0;
	m_pFbmAmp->minVal = 0.0;
	m_pFbmAmp->maxVal = 5.0;
	m_pFbmAmp->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pFbmAmp));

	m_pFbmSpeed = std::make_unique<RegressionModel::DataInfo>();
	m_pFbmSpeed->name = "fbmSpeed";
	m_pFbmSpeed->value = 1.0;
	m_pFbmSpeed->normVal = 0.0;
	m_pFbmSpeed->minVal = 0.0;
	m_pFbmSpeed->maxVal = 2.0;
	m_pFbmSpeed->paramType = RegressionModel::OUTPUT;

	outParamVec.push_back(std::move(m_pFbmSpeed));

	m_pLControllerX = std::make_unique<RegressionModel::DataInfo>();
	m_pLControllerX->name = "lControllerX";
	m_pLControllerX->value = 0.0;
	m_pLControllerX->normVal = 0.0;
	m_pLControllerX->minVal = -21.0;
	m_pLControllerX->maxVal = 21.0;
	m_pLControllerX->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pLControllerX));

	m_pLControllerY = std::make_unique<RegressionModel::DataInfo>();
	m_pLControllerY->name = "lControllerY";
	m_pLControllerY->value = 0.0;
	m_pLControllerY->normVal = 0.0;
	m_pLControllerY->minVal = -21.0;
	m_pLControllerY->maxVal = 21.0;
	m_pLControllerY->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pLControllerY));

	m_pLControllerZ = std::make_unique<RegressionModel::DataInfo>();
	m_pLControllerZ->name = "lControllerZ";
	m_pLControllerZ->value = 0.0;
	m_pLControllerZ->normVal = 0.0;
	m_pLControllerZ->minVal = -21.0;
	m_pLControllerZ->maxVal = 21.0;
	m_pLControllerZ->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pLControllerZ));

	m_pRControllerX = std::make_unique<RegressionModel::DataInfo>();
	m_pRControllerX->name = "rControllerX";
	m_pRControllerX->value = 0.0;
	m_pRControllerX->normVal = 0.0;
	m_pRControllerX->minVal = -21.0;
	m_pRControllerX->maxVal = 21.0;
	m_pRControllerX->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pRControllerX));

	m_pRControllerY = std::make_unique<RegressionModel::DataInfo>();
	m_pRControllerY->name = "rControllerY";
	m_pRControllerY->value = 0.0;
	m_pRControllerY->normVal = 0.0;
	m_pRControllerY->minVal = -21.0;
	m_pRControllerY->maxVal = 21.0;
	m_pRControllerY->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pRControllerY));

	m_pRControllerZ = std::make_unique<RegressionModel::DataInfo>();
	m_pRControllerZ->name = "rControllerZ";
	m_pRControllerZ->value = 0.0;
	m_pRControllerZ->normVal = 0.0;
	m_pRControllerZ->minVal = -21.0;
	m_pRControllerZ->maxVal = 21.0;
	m_pRControllerZ->paramType = RegressionModel::INPUT;

	inParamVec.push_back(std::move(m_pRControllerZ));

	mySavedModel = "agFasModel.json";

	//audio setup
	CsoundSession* csSession = m_pStTools->PCsoundSetup(csd);
	
	if(!m_pStTools->BSoundSourceSetup(csSession, NUM_SOUND_SOURCES))
	{
		std::cout << "Studio::setup sound sources not set up" << std::endl;
		return false;
	}

	//setup sends to csound
	std::vector<const char*> sendNames;

	sendNames.push_back("noteFreq");
	m_vSendVals.push_back(m_cspNoteFreq);	

	sendNames.push_back("noteLength");
	m_vSendVals.push_back(m_cspNoteLength);

	sendNames.push_back("winSize");
	m_vSendVals.push_back(m_cspWinSize);

	m_pStTools->BCsoundSend(csSession, sendNames, m_vSendVals);

	//setup returns from csound 
	std::vector<const char*> returnNames;

	returnNames.push_back("pitchOut");
	m_vReturnVals.push_back(m_pPitchOut);

	returnNames.push_back("freqOut");
	m_vReturnVals.push_back(m_pFreqOut);

	m_pStTools->BCsoundReturn(csSession, returnNames, m_vReturnVals);	

	return true;
}
//*******************************************************************************************


//*******************************************************************************************
// Update 
//*******************************************************************************************
void Studio::Update(glm::mat4 viewMat, MachineLearning& machineLearning, glm::vec3 controllerWorldPos_0, glm::vec3 controllerWorldPos_1, glm::quat controllerQuat_0, glm::quat controllerQuat_1, PBOInfo& pboInfo, glm::vec2 displayRes){

	// For return values from shader.
	// vec4 for each fragment is returned in the order RGBA. 
	// You have to wait until the 2nd frame to read from the buffer. 
	// At the moment, only the RGB channels are useable when the A channel is set to max.
	// The values returned are unpredictable if A channel is anything other than max.
	//if(!m_bFirstLoop)
	//std::cout << (double)pboInfo.pboPtr[0] << ":" << (double)pboInfo.pboPtr[1] << ":" << (double)pboInfo.pboPtr[2] << ":" << (double)pboInfo.pboPtr[3] << std::endl;

	// spectral pitch data processing
	m_fCurrentFrame = glfwGetTime();
	m_fDeltaTime = m_fCurrentFrame - m_fLastFrame;	
	m_fDeltaTime *= 1000.0f;
	if(*m_vReturnVals[0] > 0) m_fTargetVal = *m_vReturnVals[0];	
	if(m_fTargetVal > m_fCurrentVal)
	{
		m_fCurrentVal += m_fDeltaTime;
	} else if(m_fTargetVal <= m_fCurrentVal)
	{
		m_fCurrentVal -= m_fDeltaTime;
	} else if(m_fTargetVal == m_fCurrentVal)
	{
		m_fCurrentVal = m_fTargetVal;
	}
	if(m_fCurrentVal < 0.0f) m_fCurrentVal = 0.0f;
	m_fPitch = m_fCurrentVal;
	
	// example sound source at origin
	StudioTools::SoundSourceData soundSource1;
	glm::vec4 sourcePosWorldSpace = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	soundSource1.position = sourcePosWorldSpace;
	std::vector<StudioTools::SoundSourceData> soundSources;
	soundSources.push_back(soundSource1);

	m_pStTools->SoundSourceUpdate(soundSources, viewMat);

	//example control signal - sine function
	//sent to shader and csound
	//m_fSineControlVal = sin(glfwGetTime() * 0.33f);
	//*m_vSendVals[0] = (MYFLT)m_fSineControlVal;

	m_fTime = glfwGetTime();
	m_vDisplayRes = displayRes;

	//run machine learning
	MLAudioParameter paramData;
	paramData.distributionLow = 400.0f;
	paramData.distributionHigh = 1000.0f;
	paramData.sendVecPosition = 1;
	std::vector<MLAudioParameter> paramVec;
	paramVec.push_back(paramData);
	//MLRegressionUpdate(machineLearning, pboInfo, paramVec);	

	//********** RegressionModel Class********************
	bool currentRandomState = m_bPrevRandomState;
	if(machineLearning.bRandomParams != currentRandomState && machineLearning.bRandomParams == true){

		regMod.randomiseData(outParamVec);
		std::cout << "RANDOMISED WIN SIZE: " << outParamVec[2]->value << std::endl;
			
	}
	m_bPrevRandomState = machineLearning.bRandomParams;


	if(machineLearning.bRecord){

		inParamVec[0]->value = controllerWorldPos_0.x;
		inParamVec[1]->value = controllerWorldPos_0.y;
		inParamVec[2]->value = controllerWorldPos_0.z;
		inParamVec[3]->value = controllerWorldPos_1.x;
		inParamVec[4]->value = controllerWorldPos_1.y;
		inParamVec[5]->value = controllerWorldPos_1.z;
		//inParamVec[0]->value = 2.43;
		//inParamVec[1]->value = 5.32;
		//inParamVec[2]->value = 4.53;
		//inParamVec[3]->value = 3.21;
		//inParamVec[4]->value = 4.23;
		//inParamVec[5]->value = 4.56;

		regMod.normaliseData(inParamVec);
		regMod.normaliseData(outParamVec);
		regMod.collectData(inParamVec, outParamVec);
	}
	machineLearning.bRecord = false;

	bool currentTrainState = m_bPrevTrainState;
	if(machineLearning.bTrainModel != currentTrainState && machineLearning.bTrainModel == true){
		m_bModelTrained = regMod.trainModel();
		std::cout << "MODEL TRAINED: " << m_bModelTrained << std::endl;
	}
	m_bPrevTrainState = machineLearning.bTrainModel;

	bool currentRunState = machineLearning.bRunModel;
	if(machineLearning.bRunModel && m_bModelTrained)
	{
		inParamVec[0]->value = controllerWorldPos_0.x;
		inParamVec[1]->value = controllerWorldPos_0.y;
		inParamVec[2]->value = controllerWorldPos_0.z;
		inParamVec[3]->value = controllerWorldPos_1.x;
		inParamVec[4]->value = controllerWorldPos_1.y;
		inParamVec[5]->value = controllerWorldPos_1.z;

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
		m_dFbmAmp = outParamVec[3]->value;
		m_dFbmSpeed = outParamVec[4]->value;
	} 
	else if(!machineLearning.bRunModel && currentRunState != m_bPrevRunState)
	{
		std::cout << "Model Stopped" << std::endl;
	}
	m_bPrevRunState = currentRunState;
	
	// save model
	bool currentSaveState = m_bPrevSaveState;
	if(machineLearning.bSaveModel!= currentSaveState && machineLearning.bSaveModel == true)
	{
		regMod.saveModel(mySavedModel);
	}
	m_bPrevSaveState = machineLearning.bSaveModel;

	// load model
	bool currentLoadState = m_bPrevLoadState;
	if(machineLearning.bLoadModel != currentLoadState && machineLearning.bLoadModel == true)
	{
		m_bModelTrained = regMod.loadModel(mySavedModel);	
	}
	m_bPrevLoadState = machineLearning.bLoadModel;

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
	//glUniform1f(m_gliFreqOutLoc, *m_vReturnVals[1]);
	glUniform2f(m_gliDisplayRes, m_vDisplayRes.x, m_vDisplayRes.y);
	glUniform1f(m_gliTime, m_fTime);
	glUniform1f(m_gliFbmAmp, m_dFbmAmp);
	glUniform1f(m_gliFbmSpeed, m_dFbmSpeed);

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
	m_bPrevRandomState = false;
	m_bPrevTrainState = false;
	m_bPrevHaltState = false;
	m_bPrevLoadState = false;
	m_bMsg = true;
	m_bCurrentMsgState = false;
	m_bRunMsg = true;
	m_bCurrentRunMsgState = false;
	sizeVal = 0.0f;
	m_bModelTrained = false;
}

void Studio::MLRegressionUpdate(MachineLearning& machineLearning, PBOInfo& pboInfo, std::vector<MLAudioParameter>& params)
{

	bool currentRandomState = m_bPrevRandomState;

	// randomise parameters
	if(machineLearning.bRandomParams != currentRandomState && machineLearning.bRandomParams == true)
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
	bool currentTrainState = m_bPrevTrainState;
	if(machineLearning.bTrainModel != currentTrainState && machineLearning.bTrainModel == true && trainingSet.size() > 0)
	{

		staticRegression.train(trainingSet);
		m_bModelTrained = true;
		std::cout << "Model Trained" << std::endl;
	}	
	else if(machineLearning.bTrainModel != currentTrainState && machineLearning.bTrainModel == true && trainingSet.size() == 0)
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
	std::string mySavedModel = "mySavedModel_cyclicalEx.json";
	bool currentSaveState = m_bPrevSaveState;
	if(machineLearning.bSaveModel!= currentSaveState && machineLearning.bSaveModel == true)
	{
		staticRegression.writeJSON(mySavedModel);
		std::cout << "Saving Training Data" << std::endl;
	}
	m_bPrevSaveState = machineLearning.bSaveModel;

	// load model
	bool currentLoadState = m_bPrevLoadState;
	if(machineLearning.bLoadModel != currentLoadState && machineLearning.bLoadModel == true)
	{
		staticRegression.reset();
		staticRegression.readJSON(mySavedModel);	
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
