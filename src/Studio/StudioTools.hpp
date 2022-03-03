#ifndef STUDIOTOOLS_HPP
#define STUDIOTOOLS_HPP

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/gtc/quaternion.hpp>

#include "CsoundSession.hpp"

#define NUM_SOUND_SOURCES 2

class StudioTools {

public:

	struct SoundSourceData
	{
		glm::vec4 position;
		glm::vec4 posCamSpace;
		float distCamSpace;
		float azimuth;
		float elevation;
	};

	CsoundSession* PCsoundSetup(std::string _csdName);
	bool BSoundSourceSetup(CsoundSession* _session, int numSources);
	void SoundSourceUpdate(std::vector<SoundSourceData>& soundSources, glm::mat4 _viewMat);
	void RaymarchQuadSetup(GLuint _shaderProg);
	void DrawStart(glm::mat4 _projMat, glm::mat4 _eyeMat, glm::mat4 _viewMat, GLuint _shaderProg, glm::vec3 translateVec);
	void DrawEnd();
	bool BCsoundSend(CsoundSession* _session, std::vector<const char*>& sendName, std::vector<MYFLT*>& sendVal);
	bool BCsoundReturn(CsoundSession* _session, std::vector<const char*>& returnName, std::vector<MYFLT*>& returnVal);
	void Exit();

private:

	CsoundSession* m_pSession;

	//MYFLT** m_pAzimuthVals;
	//std::vector<MYFLT*> m_vecAzimuthVals;
	MYFLT* m_pAzimuthVals[NUM_SOUND_SOURCES];
	//MYFLT** m_pelevationvals;
	MYFLT* m_pElevationVals[NUM_SOUND_SOURCES];
	//MYFLT** m_pDistanceVals;
	MYFLT* m_pDistanceVals[NUM_SOUND_SOURCES];

	unsigned int m_uiNumSceneIndices;
	GLuint m_gluiSceneVAO;
	GLuint m_gluiIndexBuffer;

	GLint m_gliMVEPMatrixLocation;
	GLint m_gliInverseMVEPLocation;

	int m_iNumSoundSources;
};
#endif
