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

		s_pInstance->Add("SkinMesh", SHADER_FILE_DIR + "SkinMesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
		s_pInstance->Add("StaticMesh", SHADER_FILE_DIR + "Mesh_Vertex_Shader.vert", SHADER_FILE_DIR + "Mesh_Fragment_Shader.frag");
		s_pInstance->Add("Particle", SHADER_FILE_DIR + "Particle_Vertex_Shader.vert", SHADER_FILE_DIR + "Particle_Fragment_Shader.frag");
		s_pInstance->Add("Label", SHADER_FILE_DIR + "Label_Vertex_Shader.vert", SHADER_FILE_DIR + "Label_Fragment_Shader.frag");
		s_pInstance->Add("Primitive", SHADER_FILE_DIR + "Primitive_Vertex_Shader.vert", SHADER_FILE_DIR + "Primitive_Fragment_Shader.frag");
		s_pInstance->Add("Terrain", SHADER_FILE_DIR + "Terrain_Vertex_Shader.vert", SHADER_FILE_DIR + "Terrain_Fragment_Shader.frag");
		s_pInstance->Add("SkyBox", SHADER_FILE_DIR + "SkyBox_Vertex_Shader.vert", SHADER_FILE_DIR + "SkyBox_Fragment_Shader.frag");
		s_pInstance->Add("ShadowMap", SHADER_FILE_DIR + "shadow_map.vert", SHADER_FILE_DIR + "shadow_map.frag");
	}

	return s_pInstance;
}

CGLProgramManager* CGLProgramManager::s_pInstance = nullptr;

