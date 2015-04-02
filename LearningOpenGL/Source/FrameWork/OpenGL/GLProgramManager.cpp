#include "GLProgramManager.h"

void CGLProgramManager::Add( const std::string& sProgramName, const std::string& sVertexFile, const std::string& sFragmentFile )
{
	m_vGLPrograms[sProgramName] = SShaderInfo(sVertexFile, sFragmentFile);
}

GLuint CGLProgramManager::CreateProgramByName( const std::string& sProgramName )
{
	GLuint theProgram = 0;

	auto it = m_vGLPrograms.find(sProgramName);
	if ( it != m_vGLPrograms.end() )
	{
		std::vector<GLuint> shaderList;
		shaderList.push_back(OpenGLFramework::LoadShader(GL_VERTEX_SHADER, it->second.sVertex));
		shaderList.push_back(OpenGLFramework::LoadShader(GL_FRAGMENT_SHADER, it->second.sFragment));
		theProgram = OpenGLFramework::CreateProgram(shaderList);
		std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
	}

	return theProgram;
}

CGLProgramManager* CGLProgramManager::GetInstance()
{
	if ( !s_pInstance )
	{
		s_pInstance = new CGLProgramManager;
	}

	return s_pInstance;
}

CGLProgramManager* CGLProgramManager::s_pInstance = nullptr;

