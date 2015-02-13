#include "OpenGLFrameWork.h"
#include <fstream>
#include <sstream>

using namespace std;

#define LOCAL_FILE_DIR "./Resource"

namespace OpenGLFramework
{
	GLuint LoadShader( GLenum eShaderType, const std::string &strShaderFilename )
	{
		std::string strFilename = FindFileOrThrow(strShaderFilename);
		std::ifstream shaderFile(strFilename.c_str());
		std::stringstream shaderData;
		shaderData << shaderFile.rdbuf();
		shaderFile.close();

		try
		{
			return 0/*glutil::CompileShader(eShaderType, shaderData.str())*/;
		}
		catch(std::exception &e)
		{
			fprintf(stderr, "%s\n", e.what());
			throw;
		}
	}

	std::string FindFileOrThrow( const std::string &strBasename )
	{
		std::string strFilename = LOCAL_FILE_DIR + strBasename;
		std::ifstream testFile(strFilename.c_str());
		if( testFile.is_open() )
		{
			testFile.close();
			return strFilename;
		}

		throw std::runtime_error("Could not find the file " + strBasename);
	}
}


