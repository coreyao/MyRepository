#pragma once

#include "DataTypes.h"

#ifndef SHADER_FILE_DIR
#define SHADER_FILE_DIR std::string("../Resource/Shaders/")
#endif

#ifndef IMAGE_FILE_DIR
#define IMAGE_FILE_DIR std::string("../Resource/Images/")
#endif

#ifndef MESH_FILE_DIR
#define MESH_FILE_DIR std::string("../Resource/Mesh/")
#endif

class CBaseMesh
{
public:
	virtual void Render() = 0;

	Vec2 m_worldPos;
	Vec3 m_rotation;
	Vec3 m_scale;

protected:
	SMeshData m_data;
};

