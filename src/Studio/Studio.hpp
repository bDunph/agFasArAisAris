#ifndef STUDIO_HPP
#define STUDIO_HPP

//#define NUM_HRTF_VALS 3
#define NUM_SOUND_SOURCES 5 

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <deque>

#include "RapidLib/regression.h"
#include "StudioTools.hpp"
#include "RegressionModel.hpp"

class Studio {

public:
	
	// struct to hold PBO info
	struct PBOInfo
	{
		unsigned char* pboPtr;
		uint32_t pboSize;		
	};

	//bools to control machine learning
	struct MachineLearning{
		bool bRecord;
		bool bRandomParams;
		bool bTrainModel;
		bool bRunModel;
		bool bHaltModel;
		bool bSaveModel;
		bool bLoadModel;
		bool bDevMode;
		bool bActivateNNRight;
		bool bActivateNNLeft;
	};

	struct MLAudioParameter 
	{
		float distributionLow;
		float distributionHigh;
		int sendVecPosition;
	};

	bool Setup(std::string csd, GLuint shaderProg);
	void Update(glm::mat4 viewMat, MachineLearning& machineLearning, glm::vec3 controllerWorldPos_0, glm::vec3 controllerWorldPos_1, glm::quat controllerQuat_0, glm::quat controllerQuat_1, PBOInfo& pboInfo, glm::vec2 displayRes, glm::vec3 translationVec);
	void Draw(glm::mat4 projMat, glm::mat4 viewMat, glm::mat4 eyeMat, GLuint mengerProg, glm::vec3 translateVec);
	bool BCsoundReturn(CsoundSession* _session, std::vector<const char*>& returnName, std::vector<MYFLT*>& returnVal);
	void MLRegressionSetup();
	void MLRegressionUpdate(MachineLearning& machineLearning, PBOInfo& pboInfo, std::vector<MLAudioParameter>& params);
	void Exit();

private:

	StudioTools* m_pStTools;

	glm::vec4 cameraPos;
	
	MYFLT* m_pPitchOut;
	MYFLT* m_pFreqOut;
	MYFLT* m_pAmpOut;
	MYFLT* m_cspSineControlVal;
	MYFLT* m_cspRandVal;

	GLint m_gliSineControlValLoc;
	GLint m_gliPitchOutLoc;
	GLint m_gliFreqOutLoc;
	GLint m_gliAmpOutLoc;
	GLint m_gliDisplayRes;
	GLint m_gliTime;
	GLint m_gliOffset;
	float m_fOffset;
	GLint m_gliScale;
	float m_fScale;
	GLint m_gliIterations;
	float m_fIterations;
	GLint m_gliFbmAmp;
	double m_dFbmAmp;
	GLint m_gliFbmSpeed;
	double m_dFbmSpeed;
	GLint m_gliFbmAmp_left;
	double m_dFbmAmp_left;
	GLint m_gliFbmSpeed_left;
	double m_dFbmSpeed_left;
	glm::vec2 m_vDisplayRes;
	float m_fTime;
	GLint m_gliSphereNum;
	int m_iSphereNum;
	GLint m_gliControllerPos;
	glm::vec3 m_vec3ControllerPos;
	GLint m_gliControllerPos1;
	glm::vec3 m_vec3ControllerPos1;
	GLint m_gliFractalAngle;
	double m_dFractalAngle;
	GLint m_gliRed;
	GLint m_gliGreen;
	GLint m_gliBlue;
	float m_fRed;
	float m_fGreen;
	float m_fBlue;

	std::deque<double> m_dFbmAmpBuf;
	std::deque<double> m_dFbmSpeedBuf;
	int m_iBufSize;
	
	//control variables
	bool m_bFirstLoop; 
	float m_fSineControlVal;
	float m_fPitch;
	float m_fDeltaTime;
	float m_fLastFrame;
	float m_fCurrentFrame;
	float m_fTargetVal;
	float m_fCurrentVal;

	std::vector<MYFLT*> m_vSendVals;
	std::vector<const char*> m_vReturnNames;
	std::vector<MYFLT*> m_vReturnVals;
	std::vector<const char*> m_vMLParamSendNames;
	std::vector<MYFLT*> m_vMLParamSendVals;

	//machine learning controls
	bool m_bPrevSaveState;
	bool m_bCurrentSaveState;
	bool m_bPrevRandomState;
	bool m_bCurrentRandomState;
	bool m_bPrevTrainState;
	bool m_bCurrentTrainState;
	bool m_bPrevHaltState;
	bool m_bPrevRunState;
	bool m_bCurrentRunState;
	bool m_bPrevLoadState;
	bool m_bCurrentLoadState;
	bool m_bCurrentMsgState;
	bool m_bMsg;
	bool m_bRunMsg;
	bool m_bCurrentRunMsgState;
	float sizeVal;
	bool m_bModelTrained;
	bool m_bLeftNN_modelTrained;
	bool m_bPrevControlAreaMarkerState;
	bool m_bPrevRightNNState;
	bool m_bPrevLeftNNState;
	bool m_bRightNNToggle;
	bool m_bLeftNNToggle;

	//machine learning 
	regression staticRegression;
	trainingExample trainingData;
	std::vector<trainingExample> trainingSet;
	std::vector<double> inputData;
	std::vector<double> outputData;	
	
	RegressionModel regMod;
	RegressionModel regModLeft;
	
	//Right NN parameter setup
	std::unique_ptr<RegressionModel::DataInfo> m_pNoteFreq;
	std::unique_ptr<RegressionModel::DataInfo> m_pNoteLength;
	std::unique_ptr<RegressionModel::DataInfo> m_pWinSize;
	std::unique_ptr<RegressionModel::DataInfo> m_pFbmAmp;
	std::unique_ptr<RegressionModel::DataInfo> m_pFbmSpeed;
	std::unique_ptr<RegressionModel::DataInfo> m_pFractalAngle;
	std::unique_ptr<RegressionModel::DataInfo> m_pIterations;
	std::unique_ptr<RegressionModel::DataInfo> m_pScale;
	std::unique_ptr<RegressionModel::DataInfo> m_pOffset;
	std::unique_ptr<RegressionModel::DataInfo> m_pReverbFeedback;
	std::unique_ptr<RegressionModel::DataInfo> m_pReverbCutoff;
	std::unique_ptr<RegressionModel::DataInfo> m_pResampleVal;
	std::unique_ptr<RegressionModel::DataInfo> m_pGranRainMaxInsts;
	std::vector<std::unique_ptr<RegressionModel::DataInfo>> outParamVec;
	
	std::unique_ptr<RegressionModel::DataInfo> m_pRControllerX;
	std::unique_ptr<RegressionModel::DataInfo> m_pRControllerY;
	std::unique_ptr<RegressionModel::DataInfo> m_pRControllerZ;
	std::vector<std::unique_ptr<RegressionModel::DataInfo>> inParamVec;
	
	//Left NN parameter setup
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_noteFreq;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_noteLength;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_winSize;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_moogCutoff;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_overlap;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_amp;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_moogRes;
	std::unique_ptr<RegressionModel::DataInfo> m_pFbmSpeed_left;
	std::unique_ptr<RegressionModel::DataInfo> m_pFbmAmp_left;
	std::unique_ptr<RegressionModel::DataInfo> m_pRed;
	std::unique_ptr<RegressionModel::DataInfo> m_pGreen;
	std::unique_ptr<RegressionModel::DataInfo> m_pBlue;
	std::unique_ptr<RegressionModel::DataInfo> m_pModSamp_maxInsts;
	std::vector<std::unique_ptr<RegressionModel::DataInfo>> leftNN_outParamVec;

	std::unique_ptr<RegressionModel::DataInfo> m_pLControllerX;
	std::unique_ptr<RegressionModel::DataInfo> m_pLControllerY;
	std::unique_ptr<RegressionModel::DataInfo> m_pLControllerZ;
	std::vector<std::unique_ptr<RegressionModel::DataInfo>> leftNN_inParamVec;

	MYFLT* m_cspNoteFreq;
	MYFLT* m_cspNoteLength;
	MYFLT* m_cspWinSize;
	MYFLT* m_cspModSamp_noteFreq;
	MYFLT* m_cspModSamp_noteLength;
	MYFLT* m_cspModSamp_winSize;
	MYFLT* m_cspModSamp_moogCutoff;
	MYFLT* m_cspModSamp_overlap;
	MYFLT* m_cspModSamp_amp;
	MYFLT* m_cspModSamp_moogRes;
	MYFLT* m_cspModSamp_maxInsts;
	MYFLT* m_cspReverbFeedback;
	MYFLT* m_cspReverbCutoff;
	MYFLT* m_cspResampleVal;
	MYFLT* m_cspGranRain_maxInsts;

	std::string mySavedModel;
	std::string mySavedModel_left;

	//Sound sources
	GLint m_gliSpherePos1;
	GLint m_gliSpherePos2;
	GLint m_gliSpherePos3;
	glm::vec3 m_vec3SpherePos1;
	glm::vec3 m_vec3SpherePos2;
	glm::vec3 m_vec3SpherePos3;
	glm::vec3 m_vec3RotatedSpherePos1;
	glm::vec3 m_vec3RotatedSpherePos2;
	glm::vec3 m_vec3RotatedSpherePos3;
	glm::mat3 rotY;
	glm::mat3 rotYAntiClock;
	GLint m_gliSpecCentVal;
	float m_fSpecCentVal;
	MYFLT* m_pSpecCentOut;
	float m_fPrevSpecVal;
	float m_fPrevTargetVal;
	float m_fAmpOut;
	std::deque<float> m_dAmpOutVals;
	GLint m_gliModSamp_amp;
	float m_fModSamp_amp;
	MYFLT* m_cspModSamp_specAmp;
	MYFLT* m_cspModSamp_specFreq;
	float m_fModSamp_specFreq;
	MYFLT* m_cspModSamp_rmsOut;
	float m_fModSamp_rmsOut;
	GLint m_gliModSamp_rmsOut;
	std::deque<float> m_dRmsVals;
	MYFLT* m_cspGranRain_specAmpOut;
	float m_fGranRainSpecAmpOut;
	std::deque<float> m_dGranRainAmpVals;
	GLint m_gliGranRainAmp;

	glm::vec3 m_vec3SpherePos4;
	GLint m_gliSpherePos4;
	glm::vec3 m_vec3RotatedSpherePos4;
};
#endif
