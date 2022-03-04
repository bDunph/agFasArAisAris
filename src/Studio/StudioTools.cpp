#include "StudioTools.hpp"

#include <iostream>

#define PI 3.14159265359

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

CsoundSession* StudioTools::PCsoundSetup(std::string _csdName)
{
	std::string csdName = "";
	if(!_csdName.empty()) csdName = _csdName;
	m_pSession = new CsoundSession(csdName);

#ifdef _WIN32
	m_pSession->SetOption("-b -32"); 
	m_pSession->SetOption("-B 2048");
#endif
	m_pSession->StartThread();
	m_pSession->PlayScore();

	return m_pSession;
}

bool StudioTools::BSoundSourceSetup(CsoundSession* _session, int numSources)
{

	//m_pAzimuthVals = new MYFLT*[numSources];
	//m_pElevationVals = new MYFLT*[numSources];
	//m_pDistanceVals = new MYFLT*[numSources];
	
	for(int i = 0; i < numSources; i++)
	{
		std::string val1 = "azimuth" + std::to_string(i);
		const char* azimuth = val1.c_str();	
		if(_session->GetChannelPtr(m_pAzimuthVals[i], azimuth, CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) != 0)
		{
			std::cout << "GetChannelPtr could not get the azimuth" << i << " input" << std::endl;
			return false;
		}

		std::string val2 = "elevation" + std::to_string(i);
		const char* elevation = val2.c_str();
		if(_session->GetChannelPtr(m_pElevationVals[i], elevation, CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) != 0)
		{
			std::cout << "GetChannelPtr could not get the elevation" << i << " input" << std::endl;
			return false;
		}	

		std::string val3 = "distance" + std::to_string(i);
		const char* distance = val3.c_str();
		if(_session->GetChannelPtr(m_pDistanceVals[i], distance, CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) != 0)
		{
			std::cout << "GetChannelPtr could not get the distance" << i << " input" << std::endl;
			return false;
		}
	}

	return true;
}

void StudioTools::SoundSourceUpdate(std::vector<SoundSourceData>& soundSources, glm::mat4 _viewMat, glm::vec3 camWorldPos)
{
	//glm::vec3 camWorldPos = glm::vec3(_viewMat[0][3], _viewMat[1][3], _viewMat[2][3]);
	//glm::mat3 rotMat = glm::mat3(
	//		_viewMat[0][0], _viewMat[1][0], _viewMat[2][0],
	//		_viewMat[1][0], _viewMat[1][1], _viewMat[1][2],
	//		_viewMat[2][0], _viewMat[2][1], _viewMat[2][2]);

	//std::cout << "Cam World Pos: " << camWorldPos.x << ": " << camWorldPos.y << ": " << camWorldPos.z << std::endl;	

	for(int i = 0; i < soundSources.size(); i++)
	{
		//glm::mat4 soundSourceModelMatrix= glm::mat4(1.0f);

		// camera space positions
		//glm::mat4 reInvViewMat = glm::inverse(_viewMat);
		//soundSources[i].posCamSpace = _viewMat * soundSourceModelMatrix * soundSources[i].position;
		soundSources[i].posCamSpace = _viewMat * soundSources[i].position;

		// distance value camera space
		//soundSources[i].distCamSpace = sqrt(pow(soundSources[i].posCamSpace.x, 2) + pow(soundSources[i].posCamSpace.y, 2) + pow(soundSources[i].posCamSpace.z, 2));

		// distance value in world space
		float xVal = camWorldPos.x - soundSources[i].position.x; 
		float yVal = camWorldPos.y - soundSources[i].position.y;
		float zVal = camWorldPos.z - soundSources[i].position.z; 
		float distWorldSpace = sqrt(pow(xVal, 2) + pow(yVal, 2) + pow(zVal, 2));
		
		//azimuth in camera space
		float valX = soundSources[i].posCamSpace.x;// - soundSources[i].position.x;
		float valZ = soundSources[i].posCamSpace.z;// - soundSources[i].position.z;

		soundSources[i].azimuth = atan2(valX, valZ);
		//soundSources[i].azimuth = atan2(valZ, valX);
		soundSources[i].azimuth *= (180.0f/PI); 	

		//elevation in camera space
		//float oppSide = soundSources[i].posCamSpace.y;// - soundSources[i].position.y;
		//float sinVal = oppSide / soundSources[i].distCamSpace;
		//soundSources[i].elevation = asin(sinVal);
		//soundSources[i].elevation *= (180.0f/PI);		

		//elevation in world space
		float oppSide = yVal;
		float sinVal = oppSide / distWorldSpace;
		soundSources[i].elevation = asin(sinVal);
		soundSources[i].elevation *= (180.0f / PI);

		//send values to Csound pointers
		*m_pAzimuthVals[i] = (MYFLT)-soundSources[i].azimuth;
		*m_pElevationVals[i] = (MYFLT)soundSources[i].elevation;
		//*m_pDistanceVals[i] = (MYFLT)soundSources[i].distCamSpace;
		*m_pDistanceVals[i] = (MYFLT)distWorldSpace;
	}
	//std::cout << "Distance Val: " << *m_pDistanceVals[1] << std::endl;	
	soundSources.clear();

}

void StudioTools::RaymarchQuadSetup(GLuint _shaderProg)
{
	float sceneVerts[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};
	unsigned int numSceneVerts = _countof(sceneVerts);

	unsigned int sceneIndices[] = {
		0, 1, 2,
		2, 3, 0
	};
	m_uiNumSceneIndices = _countof(sceneIndices);

	float groundRayTexCoords [] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};
	unsigned int numSceneTexCoords = _countof(groundRayTexCoords);	

	glGenVertexArrays(1, &m_gluiSceneVAO);

	glBindVertexArray(m_gluiSceneVAO);

	GLuint m_gluiSceneVBO;
	glGenBuffers(1, &m_gluiSceneVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_gluiSceneVBO);
	glBufferData(GL_ARRAY_BUFFER, numSceneVerts * sizeof(float), sceneVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint m_uiglGroundTexCoords;
	glGenBuffers(1, &m_uiglGroundTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiglGroundTexCoords);
	glBufferData(GL_ARRAY_BUFFER, numSceneTexCoords * sizeof(float), groundRayTexCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL); 

	glGenBuffers(1, &m_gluiIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gluiIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uiNumSceneIndices * sizeof(unsigned int), sceneIndices, GL_STATIC_DRAW);
	
	
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);

	m_gliMVEPMatrixLocation = glGetUniformLocation(_shaderProg, "MVEPMat");
	m_gliInverseMVEPLocation = glGetUniformLocation(_shaderProg, "InvMVEP");
}

void StudioTools::DrawStart(glm::mat4 _projMat, glm::mat4 _eyeMat, glm::mat4 _viewMat, GLuint _shaderProg, glm::vec3 translateVec)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, translateVec);

	//matrices for raymarch shaders
	glm::mat4 modelViewEyeProjectionMat = _projMat * _eyeMat * _viewMat * modelMatrix;
	glm::mat4 inverseMVEPMat = glm::inverse(modelViewEyeProjectionMat);

	glBindVertexArray(m_gluiSceneVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gluiIndexBuffer);
	glUseProgram(_shaderProg);

	glUniformMatrix4fv(m_gliMVEPMatrixLocation, 1, GL_FALSE, &modelViewEyeProjectionMat[0][0]);
	glUniformMatrix4fv(m_gliInverseMVEPLocation, 1, GL_FALSE, &inverseMVEPMat[0][0]);
}

void StudioTools::DrawEnd()
{
	glDrawElements(GL_TRIANGLES, m_uiNumSceneIndices * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool StudioTools::BCsoundSend(CsoundSession* _session, std::vector<const char*>& sendName, std::vector<MYFLT*>& sendVal)
{
	for(int i = 0; i < sendName.size(); i++)
	{
		const char* chName = sendName[i];
		std::cout << chName << std::endl;
		if(_session->GetChannelPtr(sendVal[i], chName, CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) != 0)
		{
			std::cout << "GetChannelPtr could not get the send value at position " << sendName[i] << std::endl;
			return false;
		}
	}
	
	return true;
}

bool StudioTools::BCsoundReturn(CsoundSession* _session, std::vector<const char*>& returnName, std::vector<MYFLT*>& returnVal)
{
	for(int i = 0; i < returnName.size(); i++)
	{
		const char* retName = returnName[i];
		if(_session->GetChannelPtr(returnVal[i], retName, CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL) != 0)
		{
			std::cout << "Csound return value not available at position " << returnName[i] << std::endl;
			return false;
		}
	}
	
	return true;
}

void StudioTools::Exit(){
	//stop csound
	m_pSession->StopPerformance();
	delete m_pSession;
	
	//for(int i = 0; i < m_iNumSoundSources; i++)
	//{
	//	//delete dynamic memory
	//	delete m_pAzimuthVals[i];
	//	delete m_pElevationVals[i];
	//	delete m_pDistanceVals[i];
	//}
}
