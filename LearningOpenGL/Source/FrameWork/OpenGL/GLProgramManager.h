#pragma once

#include "../Utility.h"
#include "GLFrameWork.h"

class CGLProgramManager
{
	struct SShaderInfo
	{
		SShaderInfo()
		{
		}

		SShaderInfo(const std::string& sVertexFile, const std::string& sFragmentFile)
		{
			sVertex = sVertexFile;
			sFragment = sFragmentFile;
		}

		std::string sVertex;
		std::string sFragment;
	};

public:
	static CGLProgramManager* GetInstance();

	void Add(const std::string& sProgramName, const std::string& sVertexFile, const std::string& sFragmentFile);
	GLuint CreateProgramByName(const std::string& sProgramName);

private:
	std::map<std::string, SShaderInfo> m_vGLPrograms;

	static CGLProgramManager* s_pInstance;
};