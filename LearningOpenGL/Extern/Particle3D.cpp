#include "Particle3D.h"
#include "JsonParse.h"
#include "GameScene.h"
#include "Level.h"

extern GameScene *game_scene;

void Particle3DSystem::SetEnable(bool ret)
{
	m_enable = ret;
}
bool Particle3DSystem::GetEnable()
{
	return m_enable;
}
Particle3DSystem::Particle3DSystem()
{
	m_pos = Vec3::ZERO;
	m_rotate = Quaternion::identity();
	m_rootScale = Vec3::ONE;

	is_pause = false;
	
	m_enable = true;

	m_layer_parent = NULL;

	SetTexturePath("vfx/");
}
void Particle3DSystem::Init(const char *jsonfile,const char *texPath)
{
	is_pause = false;

	m_enable = true;

	jsonParse *parse = new jsonParse();
	parse->Load(jsonfile);

	////////////////////////////////
	rapidjson::Value &vv = parse->GetDocObject();
	if(vv.HasMember("emitter_list"))
	{
		vv = vv["emitter_list"];

		int tCount = vv.Size();
		for(int i = 0; i < tCount;i++)
		{
			Emitter3D *emitter = new Emitter3D(this);

			rapidjson::Value &v = vv[(rapidjson::SizeType)i];

			//v = v[(rapidjson::SizeType)i];
			std::string emitter_name = v["emitter_name"].GetString();

			Vec2 data;
			data.x = v["init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			data.y = v["init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->m_init_angle_rand = data;

			emitter->m_init_angle = v["init_angle"][(rapidjson::SizeType)0]["Constant"].GetDouble();
			//----
			emitter->m_interval_time = v["interval_time"].GetDouble();
			emitter->m_delay_time = v["delay_time"].GetDouble();
			emitter->m_during_time = v["during_time"].GetDouble();
			emitter->m_loop = v["loop"].GetDouble();

			emitter->m_rotate_Z_init_angle = v["rotate_Z_init_angle"][(rapidjson::SizeType)0]["Constant"].GetDouble();
			emitter->m_rotate_Z_init_angle_rand.x = v["rotate_Z_init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();;
			emitter->m_rotate_Z_init_angle_rand.y = v["rotate_Z_init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();;

			emitter->m_rotate_Z_angle_speed_rand.x = v["rotate_Z_angle_speed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			emitter->m_rotate_Z_angle_speed_rand.y = v["rotate_Z_angle_speed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->m_rotate_Z_angle_speed = v["rotate_Z_angle_speed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			emitter->m_is_local = std::string(v["localcoordinate"].GetString()) == "world" ? false : true;

			emitter->max_particle = v["max_particle"].GetInt();

			std::string t_str = v["emittertype"].GetString();
			if(t_str == "camera")
				emitter->m_emitter_type = kFollowCamera;
			if(t_str == "free")
				emitter->m_emitter_type = kFree;
			if(t_str == "followtarget")
				emitter->m_emitter_type = kFollowTarget;
			if(t_str == "emission")
				emitter->m_emitter_type = kEmission;

			t_str = std::string(v["blendmode"].GetString());

			emitter->particle_blend_mode = t_str == "additive" ? 1 : 0;

			emitter->follow_pos.x = v["followTargetPos"][(rapidjson::SizeType)0].GetDouble();
			emitter->follow_pos.y = v["followTargetPos"][(rapidjson::SizeType)1].GetDouble();
			emitter->follow_pos.z = v["followTargetPos"][(rapidjson::SizeType)2].GetDouble();

			emitter->m_graiy_dir.x = v["GravtySpeed"][(rapidjson::SizeType)0].GetDouble();
			emitter->m_graiy_dir.y = v["GravtySpeed"][(rapidjson::SizeType)1].GetDouble();
			emitter->m_graiy_dir.z = v["GravtySpeed"][(rapidjson::SizeType)2].GetDouble();

			emitter->m_gravity_power = v["GravtyPower"].GetDouble();

			emitter->m_emissionHangle = Vec2::ZERO;
			if(v.HasMember("EmissionHAngle"))
			{
				emitter->m_emissionHangle.x = v["EmissionHAngle"][(rapidjson::SizeType)0].GetDouble();
				emitter->m_emissionHangle.y = v["EmissionHAngle"][(rapidjson::SizeType)1].GetDouble();
			}

			emitter->m_emissionVangle = Vec2::ZERO;
			if(v.HasMember("EmissionVAngle"))
			{
				emitter->m_emissionVangle.x = v["EmissionVAngle"][(rapidjson::SizeType)0].GetDouble();
				emitter->m_emissionVangle.y = v["EmissionVAngle"][(rapidjson::SizeType)1].GetDouble();
			}


			emitter->m_wind_dir = Vec3::ZERO;

			Vec2 t_data;
			t_data.x = v["AccSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["AccSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->AccSpeed_rand = t_data;
			emitter->AccSpeed = v["AccSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["InitSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["InitSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->InitSpeed_rand = t_data;
			emitter->InitSpeed = v["InitSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["ResiSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["ResiSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->ResiSpeed_rand = t_data;
			emitter->ResiSpeed = v["ResiSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["particle_init_Zangle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["particle_init_Zangle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->particle_init_zangle_rand = t_data;
			emitter->particle_init_zangle = v["particle_init_Zangle"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["particle_init_ZangleSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["particle_init_ZangleSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->particle_zangle_speed_rand = t_data;
			emitter->particle_zangle_speed = v["particle_init_ZangleSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			emitter->particle_texture_name = v["particleTextureName"].GetString();
			emitter->particle_texture_name = tex_name_path.c_str() + emitter->particle_texture_name;

			emitter->particle_self_rotate.x = v["particle_self_rotate"][(rapidjson::SizeType)0].GetDouble();
			emitter->particle_self_rotate.y = v["particle_self_rotate"][(rapidjson::SizeType)1].GetDouble();
			emitter->particle_self_rotate.z = v["particle_self_rotate"][(rapidjson::SizeType)2].GetDouble();

			emitter->particle_texture_info.x = v["particleTextureInfo"][(rapidjson::SizeType)0].GetDouble();
			emitter->particle_texture_info.y = v["particleTextureInfo"][(rapidjson::SizeType)1].GetDouble();

			emitter->layerid = v["LayerId"].GetInt();

			emitter->particle_during_time_rand = Vec2::ZERO;
			if(v.HasMember("particle_during_time"))
			{
				emitter->particle_during_time_rand.x = v["particle_during_time"][(rapidjson::SizeType)0].GetDouble();
				emitter->particle_during_time_rand.y = v["particle_during_time"][(rapidjson::SizeType)1].GetDouble();
			}

			emitter->particle_texture_speed_rand = Vec2::ZERO;
			if(v.HasMember("particleTextureSpeed"))
			{
				emitter->particle_texture_speed_rand.x = v["particleTextureSpeed"][(rapidjson::SizeType)0].GetDouble();
				emitter->particle_texture_speed_rand.y = v["particleTextureSpeed"][(rapidjson::SizeType)1].GetDouble();
			}

			///-----------------------
			int loops = v["particle_size_curve"][(rapidjson::SizeType)0]["loop"].GetInt();

			emitter->m_particle_size_control.SetLoop(loops);

			std::string curve_type = v["particle_size_curve"][(rapidjson::SizeType)0]["type"].GetString();
			if(curve_type == "liner")
			{
				rapidjson::Value &vv = v["particle_size_curve"][(rapidjson::SizeType)0]["curve_info"];

				for(int i = 0 ; i < (int)vv.Size(); i++)
				{
					Vec3 s_v;
					s_v.x = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)0].GetDouble();
					s_v.y = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)1].GetDouble();
					s_v.z = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)2].GetDouble();

					Vec3 e_v;
					e_v.x = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)0].GetDouble();
					e_v.y = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)1].GetDouble();
					e_v.z = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)2].GetDouble();

					float t_s = vv[(rapidjson::SizeType)i]["start_time"].GetDouble();
					float t_d = vv[(rapidjson::SizeType)i]["during_time"].GetDouble();

					emitter->m_particle_size_control.AddPoint(s_v,e_v,t_s,t_d);
				}	
			}
			///-----------------------
			loops = v["particle_color_curve"][(rapidjson::SizeType)0]["loop"].GetInt();

			emitter->m_particle_color_control.SetLoop(loops);

			curve_type = v["particle_color_curve"][(rapidjson::SizeType)0]["type"].GetString();
			if(curve_type == "liner")
			{
				rapidjson::Value &vv = v["particle_color_curve"][(rapidjson::SizeType)0]["curve_info"];

				for(int i = 0 ; i < (int)vv.Size(); i++)
				{
					Vec4 s_v;
					s_v.x = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)0].GetDouble();
					s_v.y = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)1].GetDouble();
					s_v.z = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)2].GetDouble();
					s_v.w = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)3].GetDouble();

					s_v = s_v / 255.0f;

					Vec4 e_v;
					e_v.x = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)0].GetDouble();
					e_v.y = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)1].GetDouble();
					e_v.z = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)2].GetDouble();
					e_v.w = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)3].GetDouble();

					e_v = e_v / 255.0f;

					float t_s = vv[(rapidjson::SizeType)i]["start_time"].GetDouble();
					float t_d = vv[(rapidjson::SizeType)i]["during_time"].GetDouble();

					emitter->m_particle_color_control.AddPoint(s_v,e_v,t_s,t_d);
				}	
			}

			emitter->init();
			AddEmitter3D(emitter);
		}
	}
	////////////////////////////////

	delete parse;
}
Particle3DSystem::Particle3DSystem(const char *jsonfile,const char *texPath)
{
	m_file_parse = jsonfile;

	m_pos = Vec3::ZERO;
	m_rotate = Quaternion::identity();
	m_rootScale = Vec3::ONE;

	SetTexturePath(texPath);

	m_layer_parent = NULL;

	Init(jsonfile,texPath);
	/*jsonParse *parse = new jsonParse();
	parse->Load(jsonfile);

	////////////////////////////////
	rapidjson::Value &vv = parse->GetDocObject();
	if(vv.HasMember("emitter_list"))
	{
		vv = vv["emitter_list"];

		int tCount = vv.Size();
		for(int i = 0; i < tCount;i++)
		{
			Emitter3D *emitter = new Emitter3D(this);

			rapidjson::Value &v = vv[(rapidjson::SizeType)i];

			//v = v[(rapidjson::SizeType)i];
			std::string emitter_name = v["emitter_name"].GetString();

			Vec2 data;
			data.x = v["init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			data.y = v["init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->m_init_angle_rand = data;

			emitter->m_init_angle = v["init_angle"][(rapidjson::SizeType)0]["Constant"].GetDouble();
			//----
			emitter->m_interval_time = v["interval_time"].GetDouble();
			emitter->m_delay_time = v["delay_time"].GetDouble();
			emitter->m_during_time = v["during_time"].GetDouble();
			emitter->m_loop = v["loop"].GetDouble();

			emitter->m_rotate_Z_init_angle = v["rotate_Z_init_angle"][(rapidjson::SizeType)0]["Constant"].GetDouble();
			emitter->m_rotate_Z_init_angle_rand.x = v["rotate_Z_init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();;
			emitter->m_rotate_Z_init_angle_rand.y = v["rotate_Z_init_angle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();;

			emitter->m_rotate_Z_angle_speed_rand.x = v["rotate_Z_angle_speed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			emitter->m_rotate_Z_angle_speed_rand.y = v["rotate_Z_angle_speed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->m_rotate_Z_angle_speed = v["rotate_Z_angle_speed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			emitter->m_is_local = std::string(v["localcoordinate"].GetString()) == "world" ? false : true;
			
			emitter->max_particle = v["max_particle"].GetInt();

			std::string t_str = v["emittertype"].GetString();
			if(t_str == "camera")
				emitter->m_emitter_type = kFollowCamera;
			if(t_str == "free")
				emitter->m_emitter_type = kFree;
			if(t_str == "followtarget")
				emitter->m_emitter_type = kFollowTarget;
			if(t_str == "emission")
				emitter->m_emitter_type = kEmission;

			t_str = std::string(v["blendmode"].GetString());

			emitter->particle_blend_mode = t_str == "additive" ? 1 : 0;

			emitter->follow_pos.x = v["followTargetPos"][(rapidjson::SizeType)0].GetDouble();
			emitter->follow_pos.y = v["followTargetPos"][(rapidjson::SizeType)1].GetDouble();
			emitter->follow_pos.z = v["followTargetPos"][(rapidjson::SizeType)2].GetDouble();
			 
			emitter->m_graiy_dir.x = v["GravtySpeed"][(rapidjson::SizeType)0].GetDouble();
			emitter->m_graiy_dir.y = v["GravtySpeed"][(rapidjson::SizeType)1].GetDouble();
			emitter->m_graiy_dir.z = v["GravtySpeed"][(rapidjson::SizeType)2].GetDouble();

			emitter->m_gravity_power = v["GravtyPower"].GetDouble();

			emitter->m_emissionHangle = Vec2::ZERO;
			if(v.HasMember("EmissionHAngle"))
			{
				emitter->m_emissionHangle.x = v["EmissionHAngle"][(rapidjson::SizeType)0].GetDouble();
				emitter->m_emissionHangle.y = v["EmissionHAngle"][(rapidjson::SizeType)1].GetDouble();
			}

			emitter->m_emissionVangle = Vec2::ZERO;
			if(v.HasMember("EmissionVAngle"))
			{
				emitter->m_emissionVangle.x = v["EmissionVAngle"][(rapidjson::SizeType)0].GetDouble();
				emitter->m_emissionVangle.y = v["EmissionVAngle"][(rapidjson::SizeType)1].GetDouble();
			}
			

			emitter->m_wind_dir = Vec3::ZERO;

			Vec2 t_data;
			t_data.x = v["AccSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["AccSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->AccSpeed_rand = t_data;
			emitter->AccSpeed = v["AccSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["InitSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["InitSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->InitSpeed_rand = t_data;
			emitter->InitSpeed = v["InitSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["ResiSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["ResiSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->ResiSpeed_rand = t_data;
			emitter->ResiSpeed = v["ResiSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["particle_init_Zangle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["particle_init_Zangle"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->particle_init_zangle_rand = t_data;
			emitter->particle_init_zangle = v["particle_init_Zangle"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			t_data.x = v["particle_init_ZangleSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)0].GetDouble();
			t_data.y = v["particle_init_ZangleSpeed"][(rapidjson::SizeType)0]["Random"][(rapidjson::SizeType)1].GetDouble();

			emitter->particle_zangle_speed_rand = t_data;
			emitter->particle_zangle_speed = v["particle_init_ZangleSpeed"][(rapidjson::SizeType)0]["Constant"].GetDouble();

			emitter->particle_texture_name = v["particleTextureName"].GetString();
			emitter->particle_texture_name = tex_name_path.c_str() + emitter->particle_texture_name;

			emitter->particle_self_rotate.x = v["particle_self_rotate"][(rapidjson::SizeType)0].GetDouble();
			emitter->particle_self_rotate.y = v["particle_self_rotate"][(rapidjson::SizeType)1].GetDouble();
			emitter->particle_self_rotate.z = v["particle_self_rotate"][(rapidjson::SizeType)2].GetDouble();

			emitter->particle_texture_info.x = v["particleTextureInfo"][(rapidjson::SizeType)0].GetDouble();
			emitter->particle_texture_info.y = v["particleTextureInfo"][(rapidjson::SizeType)1].GetDouble();

			emitter->layerid = v["LayerId"].GetInt();

			emitter->particle_during_time_rand = Vec2::ZERO;
			if(v.HasMember("particle_during_time"))
			{
				emitter->particle_during_time_rand.x = v["particle_during_time"][(rapidjson::SizeType)0].GetDouble();
				emitter->particle_during_time_rand.y = v["particle_during_time"][(rapidjson::SizeType)1].GetDouble();
			}

			emitter->particle_texture_speed_rand = Vec2::ZERO;
			if(v.HasMember("particleTextureSpeed"))
			{
				emitter->particle_texture_speed_rand.x = v["particleTextureSpeed"][(rapidjson::SizeType)0].GetDouble();
				emitter->particle_texture_speed_rand.y = v["particleTextureSpeed"][(rapidjson::SizeType)1].GetDouble();
			}
			
			///-----------------------
			int loops = v["particle_size_curve"][(rapidjson::SizeType)0]["loop"].GetInt();

			emitter->m_particle_size_control.SetLoop(loops);

			std::string curve_type = v["particle_size_curve"][(rapidjson::SizeType)0]["type"].GetString();
			if(curve_type == "liner")
			{
				rapidjson::Value &vv = v["particle_size_curve"][(rapidjson::SizeType)0]["curve_info"];

				for(int i = 0 ; i < (int)vv.Size(); i++)
				{
					Vec3 s_v;
					s_v.x = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)0].GetDouble();
					s_v.y = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)1].GetDouble();
					s_v.z = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)2].GetDouble();

					Vec3 e_v;
					e_v.x = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)0].GetDouble();
					e_v.y = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)1].GetDouble();
					e_v.z = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)2].GetDouble();

					float t_s = vv[(rapidjson::SizeType)i]["start_time"].GetDouble();
					float t_d = vv[(rapidjson::SizeType)i]["during_time"].GetDouble();

					emitter->m_particle_size_control.AddPoint(s_v,e_v,t_s,t_d);
				}	
			}
			///-----------------------
			loops = v["particle_color_curve"][(rapidjson::SizeType)0]["loop"].GetInt();

			emitter->m_particle_color_control.SetLoop(loops);

			curve_type = v["particle_color_curve"][(rapidjson::SizeType)0]["type"].GetString();
			if(curve_type == "liner")
			{
				rapidjson::Value &vv = v["particle_color_curve"][(rapidjson::SizeType)0]["curve_info"];

				for(int i = 0 ; i < (int)vv.Size(); i++)
				{
					Vec4 s_v;
					s_v.x = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)0].GetDouble();
					s_v.y = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)1].GetDouble();
					s_v.z = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)2].GetDouble();
					s_v.w = vv[(rapidjson::SizeType)i]["start_value"][(rapidjson::SizeType)3].GetDouble();

					s_v = s_v / 255.0f;

					Vec4 e_v;
					e_v.x = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)0].GetDouble();
					e_v.y = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)1].GetDouble();
					e_v.z = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)2].GetDouble();
					e_v.w = vv[(rapidjson::SizeType)i]["end_value"][(rapidjson::SizeType)3].GetDouble();

					e_v = e_v / 255.0f;

					float t_s = vv[(rapidjson::SizeType)i]["start_time"].GetDouble();
					float t_d = vv[(rapidjson::SizeType)i]["during_time"].GetDouble();

					emitter->m_particle_color_control.AddPoint(s_v,e_v,t_s,t_d);
				}	
			}

			emitter->init();
			AddEmitter3D(emitter);
		}
	}
	////////////////////////////////

	delete parse;*/
}
void Particle3DSystem::Pause()
{
	is_pause = true;

	SetEnable(false);
}
void Particle3DSystem::Resume()
{
	is_pause = false;

	SetEnable(true);
}
void Particle3DSystem::Reset()
{
	Clear();
	Init(m_file_parse.c_str(),tex_name_path.c_str());
}
void Particle3DSystem::AddEmitter3D(Emitter3D *emitter)
{
	m_emitter_list.push_back(emitter);
}
void Particle3DSystem::update(float dt)
{
	if(is_pause)
		return;

	std::vector<Emitter3D *>::iterator iter = m_emitter_list.begin();
	for(;iter != m_emitter_list.end();)
	{
		Emitter3D *item = *iter;

		item->update(dt);
		iter++;
	}
}
int Particle3DSystem::GetParticleInstanceTotalCount()
{
	int tEmitterCount = m_emitter_list.size();
	int tParticleInstanceCount = 0;

	for(int i = 0 ; i < (int)tEmitterCount; i++)
	{
		tParticleInstanceCount += m_emitter_list[i]->m_particle_instance_list.size();
	}
	return tParticleInstanceCount;
}
void Particle3DSystem::SetPosition(const Vec3 &pos)
{
	m_pos = pos;
}
Vec3 Particle3DSystem::GetPosition()
{
	return m_pos;
}
void Particle3DSystem::SetScale(const Vec3 &scale)
{
	m_rootScale = scale;
}
Vec3 Particle3DSystem::GetScale()
{
	return m_rootScale;
}
void Particle3DSystem::SetRotation(const Quaternion &quat)
{
	m_rotate = quat;
}
Quaternion Particle3DSystem::GetRotation()
{
	return m_rotate;
}
void Particle3DSystem::Clear()
{
	int tEmitterCount = m_emitter_list.size();
	int tParticleInstanceCount = 0;

	for(int i = 0 ; i < (int)tEmitterCount; i++)
	{
		tParticleInstanceCount = (int)m_emitter_list[i]->m_particle_instance_list.size();
		for(int j = 0 ; j < (int)tParticleInstanceCount; j++)
		{
			ParticleInstance3D *t_particle = m_emitter_list[i]->m_particle_instance_list[j];

			static_cast<ParticleLayer *>(m_layer_parent)->RemoveParticle(t_particle);
			delete t_particle;
		}
		m_emitter_list[i]->m_particle_instance_list.clear();
		delete m_emitter_list[i];
	}
	m_emitter_list.clear();
}
Emitter3D::Emitter3D(Particle3DSystem *particleSystem)
{
	m_parent = particleSystem;
	m_loop = 1;

	m_init_angle_rand = Vec2::ZERO;
	m_init_angle = 0.0f;

	m_rotate_Z_init_angle = 0.0f;
	m_rotate_Z_angle_speed = 0.0f;

	AccSpeed = 0.0f;
	AccSpeed_rand = Vec2::ZERO;

	InitSpeed = 0.0f;
	InitSpeed_rand = Vec2::ZERO;

	ResiSpeed = 0.0f;
	ResiSpeed_rand = Vec2::ZERO;

	particle_init_zangle = 0.0f;
	particle_init_zangle_rand = Vec2::ZERO;

	particle_zangle_speed = 0.0f;
	particle_zangle_speed_rand = Vec2::ZERO;

	m_interval_time = 0.0f;
	m_during_time = 0.0f;
	m_time = 0.0f;
}
void Emitter3D::init()
{
	if(m_rotate_Z_init_angle_rand == Vec2::ZERO)
	{
		angle_xy.y = m_rotate_Z_init_angle;
	}
	else
	{
		angle_xy.y = m_rotate_Z_init_angle_rand.x + CCRANDOM_0_1() * (m_rotate_Z_init_angle_rand.y - m_rotate_Z_init_angle_rand.x);
	}
	
	m_during_total_time = m_during_time;
}
void Emitter3D::update(float dt)
{
	////////////////////////
	if(m_delay_time > 0.0f)
	{
		m_delay_time -= dt;
	}
	else
	{
		if(m_loop > 0 || m_loop < 0 || m_during_time > 0.0f)
		{
			if(m_during_time <= 0.0f)
			{
				if(m_loop < 0)
				{
					m_during_time = m_during_total_time;
				}
				if(m_loop > 0)
				{
					m_loop--;
					m_during_time = m_during_total_time;
				}
			}
			m_time += dt;

			if((m_time > m_interval_time || 
				fabs(m_time - m_interval_time) <= 0.00005f) && (int)m_particle_instance_list.size() < max_particle)
			{
				m_time = 0.0f;

				/////////////////////////////add particle instance

				if(m_init_angle_rand == Vec2::ZERO)
				{
					angle_xy.x = m_init_angle;
				}
				else
				{
					angle_xy.x = m_init_angle_rand.x + CCRANDOM_0_1() * (m_init_angle_rand.y - m_init_angle_rand.x);
				}

				ParticleInstance3D *t_particle = new ParticleInstance3D(this);

				t_particle->layerId = layerid;

				static_cast<ParticleLayer *>(m_parent->m_layer_parent)->AddParticle(t_particle);
				t_particle->buildVertex();

				t_particle->tex = Director::getInstance()->getTextureCache()->addImage(particle_texture_name.c_str());
				t_particle->m_during_time = particle_during_time_rand.x + CCRANDOM_0_1() * (particle_during_time_rand.y - particle_during_time_rand.x);
				t_particle->m_texture_info = particle_texture_info;
				t_particle->m_texture_speed = particle_texture_speed_rand.x + CCRANDOM_0_1() * (particle_texture_speed_rand.y - particle_texture_speed_rand.x);

				t_particle->m_size_control = m_particle_size_control;
				t_particle->m_size_control.Init();

				t_particle->m_color_control = m_particle_color_control;
				t_particle->m_color_control.Init();

				Mat4 m_root_matrix;
				Mat4::createRotation(m_parent->GetRotation(),&m_root_matrix);
				m_root_matrix.m[12] = m_parent->GetPosition().x;
				m_root_matrix.m[13] = m_parent->GetPosition().y;
				m_root_matrix.m[14] = m_parent->GetPosition().z;

				t_particle->m_parent_matrix = m_root_matrix;

				t_particle->blend_mode = particle_blend_mode;

				if(AccSpeed_rand == Vec2::ZERO)
				{
					t_particle->acce_speed = AccSpeed;
				}
				else
				{
					t_particle->acce_speed = AccSpeed_rand.x + CCRANDOM_0_1() * (AccSpeed_rand.y - AccSpeed_rand.x);
				}
				if(InitSpeed_rand == Vec2::ZERO)
				{
					t_particle->init_speed = InitSpeed;
				}
				else
				{
					t_particle->init_speed = InitSpeed_rand.x + CCRANDOM_0_1() * (InitSpeed_rand.y - InitSpeed_rand.x);
				}
				if(ResiSpeed_rand == Vec2::ZERO)
				{
					t_particle->resi_speed = ResiSpeed;
				}
				else
				{
					t_particle->resi_speed = ResiSpeed_rand.x + CCRANDOM_0_1() * (ResiSpeed_rand.y - ResiSpeed_rand.x);
				}
				
				t_particle->m_self_rotate = particle_self_rotate;

				if(particle_init_zangle_rand == Vec2::ZERO)
				{
					t_particle->m_z_angle = particle_init_zangle;
				}
				else
				{
					t_particle->m_z_angle = particle_init_zangle_rand.x + CCRANDOM_0_1() * (particle_init_zangle_rand.y - particle_init_zangle_rand.x);
				}
				if(particle_zangle_speed_rand == Vec2::ZERO)
				{
					t_particle->m_z_angle_speed = particle_zangle_speed;
				}
				else
				{
					t_particle->m_z_angle_speed = particle_zangle_speed_rand.x + CCRANDOM_0_1() * (particle_zangle_speed_rand.y - particle_zangle_speed_rand.x);
				}

				Mat4 m1,m2;
				m1.rotate(Vec3(1,0,0),CC_DEGREES_TO_RADIANS(angle_xy.x),&m1);
				m2.rotate(Vec3(0,1,0),CC_DEGREES_TO_RADIANS(angle_xy.y),&m2);
				m1 = m2 * m1;
				Vec3 tDir(0,0,-1);
				m1.transformVector(tDir,&tDir);
				tDir.normalize();

				if(m_emitter_type == kEmission)
				{
					float emission_h_angle = m_emissionHangle.x + CCRANDOM_0_1() * (m_emissionHangle.y - m_emissionHangle.x);
					float emission_v_angle = m_emissionVangle.x + CCRANDOM_0_1() * (m_emissionVangle.y - m_emissionVangle.x);

					Mat4 mm1,mm2;
					mm1.rotate(Vec3(1,0,0),CC_DEGREES_TO_RADIANS(emission_v_angle),&mm1);
					mm2.rotate(Vec3(0,1,0),CC_DEGREES_TO_RADIANS(emission_h_angle),&mm2);

					mm1 = mm1 * mm2;
					mm1.transformVector(tDir,&tDir);
					tDir.normalize();
				}

				/*Mat4 m_root_rotate;
				Mat4::createRotation(m_parent->GetRotation(),&m_root_rotate);

				m_root_rotate.transformVector(tDir,&tDir);
				tDir.normalize();*/

				t_particle->m_move_dir = tDir;
				t_particle->m_graiy_dir = m_graiy_dir;
				t_particle->m_wind_dir = m_wind_dir;
				
				m_particle_instance_list.push_back(t_particle);
				/////////////////////////////
			}

			m_during_time -= dt;
		}

		float tAngleSpeed = 0.0f;

		if(m_rotate_Z_angle_speed_rand == Vec2::ZERO)
		{
			tAngleSpeed = m_rotate_Z_angle_speed;
		}
		else
		{
			tAngleSpeed = m_rotate_Z_angle_speed_rand.x + CCRANDOM_0_1() * (m_rotate_Z_angle_speed_rand.y - m_rotate_Z_angle_speed_rand.x);
		}
		if(fabs(tAngleSpeed) <= 0.00005f)
		{
			if(m_rotate_Z_init_angle_rand == Vec2::ZERO)
			{
				angle_xy.y = m_rotate_Z_init_angle;
			}
			else
			{
				angle_xy.y = m_rotate_Z_init_angle_rand.x + CCRANDOM_0_1() * (m_rotate_Z_init_angle_rand.y - m_rotate_Z_init_angle_rand.x);
			}
		}
		angle_xy.y += tAngleSpeed * dt;
	}
	////////////////////////
	std::vector<ParticleInstance3D *>::iterator iter = m_particle_instance_list.begin();
	for(;iter != m_particle_instance_list.end();)
	{
		ParticleInstance3D *item = *iter;
		if(!item->IsLife())
		{
			static_cast<ParticleLayer *>(m_parent->m_layer_parent)->RemoveParticle(item);

			delete item;
			iter = m_particle_instance_list.erase(iter);
			if(iter == m_particle_instance_list.end())
				break;
			continue;
		}

		item->update(dt);
		iter++;
	}
}
ParticleInstance3D::ParticleInstance3D(Emitter3D *emitter)
{
	m_parent_pos = Vec3::ZERO;

	blend_mode = 0;
	particle_offset_rotate = 0.0f;
	layerId = 0;
	m_z_angle = 0.0f;
	m_z_angle_speed = 0.0f;

	m_graity_acc_speed = 0.0f;
	m_size = Vec3(1,1,1);
	m_color = Vec4(1,1,1,1);
	m_time = 0.0f;
	_glProgramState = nullptr;
	m_parent = emitter;

	m_is_life = true;

	m_frame_id = 0;

	_Vertex item;
	//--------left top
	item.pos = Vec3(-1.0f,0.0f,1.0f);
	item.color = Color4B::WHITE;
	item.uv = Vec2(0,0);

	_vertexs.push_back(item);
	//--------right top
	item.pos = Vec3(1.0f,0.0f,1.0f);
	item.color = Color4B::WHITE;
	item.uv = Vec2(1,0);

	_vertexs.push_back(item);

	//--------left bottom
	item.pos = Vec3(-1.0f,0.0f,-1.0f);
	item.color = Color4B::WHITE;
	item.uv = Vec2(0,1);

	_vertexs.push_back(item);

	//--------right bottom
	item.pos = Vec3(1.0f,0.0f,-1.0f);
	item.color = Color4B::WHITE;
	item.uv = Vec2(1,1);

	_vertexs.push_back(item);

	//
	_indices.push_back(0);
	_indices.push_back(1);
	_indices.push_back(3);

	_indices.push_back(0);
	_indices.push_back(3);
	_indices.push_back(2);
}
ParticleInstance3D::~ParticleInstance3D()
{
	clearVertex();
}
void ParticleInstance3D::SetPosition(const Vec3 &pos)
{
	m_pos = pos;
}
Vec3 ParticleInstance3D::GetPosition()
{
	return m_pos;
}
void ParticleInstance3D::SetSize(const Vec3 &size)
{
	m_size = size;
}
Vec3 ParticleInstance3D::GetSize()
{
	return m_size;
}
void ParticleInstance3D::update(float dt)
{
	if(!m_parent)
		return;
	m_time += dt;
	if(m_during_time > 0.0f && m_time >= m_during_time)
	{
		m_is_life = false;
		return;
	}
	////////////
	m_size_control.Update(dt);
	m_color_control.Update(dt);

	if(m_size_control.IsRun())
	{
		m_size = m_size_control.GetValue();
	}
	if(m_color_control.IsRun())
	{
		m_color = m_color_control.GetValue();
	}
	////////////
	Mat4 m_rotate,m_scale,m_move;

	float tSpeed = init_speed + acce_speed - resi_speed;
	if(tSpeed < 0.0f)
		tSpeed = 0.0f;

	m_move_dir.normalize();
	m_graity_acc_speed += m_parent->m_gravity_power * dt;

	Mat4 m1,m2;

	m1.rotate(Vec3(1,0,0),CC_DEGREES_TO_RADIANS(m_parent->angle_xy.x),&m1);
	m2.rotate(Vec3(0,1,0),CC_DEGREES_TO_RADIANS(m_parent->angle_xy.y),&m2);

	////emitter self rotate
	m_rotate = m2 * m1;

	if(m_parent->m_emitter_type == EmitterType::kEmission)
	{
		m_pos += m_move_dir * tSpeed * dt;
	}
	else
	{
		m_pos += m_move_dir * tSpeed * dt;
		m_pos += m_graiy_dir * m_graity_acc_speed;
	}

	if(m_parent->m_is_local)
	{
		Mat4::createRotation(m_parent->m_parent->GetRotation(),&m_parent_matrix);
		m_parent_matrix.m[12] = m_parent->m_parent->GetPosition().x;
		m_parent_matrix.m[13] = m_parent->m_parent->GetPosition().y;
		m_parent_matrix.m[14] = m_parent->m_parent->GetPosition().z;
	}

	m_move.translate(m_pos);

	Mat4 self_Zrotate,self_rotate;

	Mat4 _x,_y,_z;
	_x.rotateX(CC_DEGREES_TO_RADIANS(m_self_rotate.x),&_x);
	_y.rotateY(CC_DEGREES_TO_RADIANS(m_self_rotate.y),&_y);
	_z.rotateZ(CC_DEGREES_TO_RADIANS(m_self_rotate.z),&_z);

	////self free angle
	self_rotate = _z * _y * _x;

	////self z rotate angle
	m_z_angle += m_z_angle_speed * dt;

	Vec3 m_z_dir;
	self_rotate.getUpVector(&m_z_dir);

	self_Zrotate.rotate(m_z_dir,CC_DEGREES_TO_RADIANS(m_z_angle),&self_Zrotate);

	Mat4 root_scale_matrix;
	root_scale_matrix.scale(m_parent->m_parent->GetScale());

	if(m_parent->m_emitter_type == EmitterType::kFree)
	{
		m_scale.createScale(m_size.x,m_size.y,m_size.z,&m_scale);
		
		m_model_transform = m_parent_matrix * m_move * m_rotate * self_Zrotate * self_rotate * m_scale;
	}
	else
	{
		if(m_parent->m_emitter_type == EmitterType::kFollowCamera)
		{
			m_scale.createScale(m_size.x,m_size.y,m_size.z,&m_scale);
			Mat4 m_camera_rotate_transform = m_parent->m_parent->m_view_transform;

			m_camera_rotate_transform.m[12] = m_camera_rotate_transform.m[13] = m_camera_rotate_transform.m[14] = 0.0f;
			m_model_transform = m_parent_matrix * m_move * m_camera_rotate_transform * self_Zrotate * self_rotate * m_scale;
		}
		else
		{
			Vec3 tDir;
			Vec3 tUp(0,0,1);

			//if(m_parent->m_is_local == false)
				m_rotate.getForwardVector(&tUp);

			Vec3::cross(Vec3(0,0,1),m_move_dir.getNormalized(),&tDir);
			tDir.normalize();

			float t_angle = Vec3::angle(Vec3(0,0,1),m_move_dir.getNormalized());
			Quaternion dir_quat;
			Mat4 dir_m;
			Quaternion::createFromAxisAngle(tDir,t_angle,&dir_quat);
			dir_quat.normalize();
			Mat4::createRotation(dir_quat,&dir_m);

			m_scale.createScale(m_size.x,m_size.y,m_size.z,&m_scale);
			m_model_transform = m_parent_matrix * m_move * dir_m * m_rotate * self_Zrotate * self_rotate * m_scale;
		}
	}

	m_model_transform = m_model_transform * root_scale_matrix;

	if(!m_texture_info.isZero())
	{
		m_frame_id += m_texture_speed * dt;

		if((int)m_frame_id >= m_texture_info.x * m_texture_info.y)
		{
			m_frame_id = 0.0f;
		}
	}

	m_vp_transform = m_parent->m_parent->m_vp_transform;
}
void ParticleInstance3D::draw()
{
	Mat4 mat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, m_vp_transform);

	if(_glProgramState)
	{
		_glProgramState->apply(m_model_transform);
	}
	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	if(blend_mode == 0)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
	}

	 GL::bindTexture2D(tex->getName());

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

	GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

	if(!m_texture_info.isZero())
	{
		Vec2 t_anim_uv = Vec2((int)m_frame_id % (int)m_texture_info.y,(int)m_frame_id / (int)m_texture_info.x);
		_glProgramState->getGLProgram()->setUniformLocationWith4f(u_texture_info,t_anim_uv.x,t_anim_uv.y,m_texture_info.x,m_texture_info.y);
	}
	else
		_glProgramState->getGLProgram()->setUniformLocationWith4f(u_texture_info,0,0,1,1);

	_glProgramState->getGLProgram()->setUniformLocationWith4f(u_add_color,m_color.x,m_color.y,m_color.z,m_color.w);

	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (GLvoid*)0);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(_Vertex), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(_Vertex), (GLvoid*)(sizeof(float) * 3 + 4));

	glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, mat);

	glDepthMask(GL_TRUE);
}
void ParticleInstance3D::buildVertex()
{
	GLProgram *pProgram = GLProgramCache::getInstance()->getGLProgram("particle_shader");
	GameScene *tScene = game_scene;

	_glProgramState = tScene->_glProgramState;

	u_texture_info = glGetUniformLocation(pProgram->getProgram(),"v_texAnimInfo");
	u_add_color = glGetUniformLocation(pProgram->getProgram(),"v_addcolor");

	glGenBuffers(1,&_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,_vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER,
		_vertexs.size() * sizeof(_vertexs[0]),
		&_vertexs[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,0);

	glGenBuffers(1,&_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_indexBuffer);

	unsigned int indexSize = 2;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indexSize * _indices.size(),&_indices[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	_primitiveType = Mesh::PrimitiveType::TRIANGLES;
	_indexFormat = Mesh::IndexFormat::INDEX16;
}
void ParticleInstance3D::resetVertex()
{
	clearVertex();
	buildVertex();
}
void ParticleInstance3D::clearVertex()
{
	if(glIsBuffer(_vertexBuffer))
	{
		glDeleteBuffers(1, &_vertexBuffer);
		_vertexBuffer = 0;
	}

	if(glIsBuffer(_indexBuffer))
	{
		glDeleteBuffers(1, &_indexBuffer);
		_indexBuffer = 0;
	}
	_primitiveType = Mesh::PrimitiveType::TRIANGLES;
	_indexFormat = Mesh::IndexFormat::INDEX16;
}

static bool compareRenderLayer(ParticleInstance3D* a, ParticleInstance3D* b)
{
	return a->layerId > b->layerId;
}
void ParticleLayer::AddParticle(ParticleInstance3D *particle)
{
	m_particle_list.push_back(particle);

	std::sort(std::begin(m_particle_list),std::end(m_particle_list), compareRenderLayer);
}
void ParticleLayer::RemoveParticle(ParticleInstance3D *particle)
{
	std::vector<ParticleInstance3D *>::iterator iter = m_particle_list.begin();
	for(;iter != m_particle_list.end();iter++)
	{
		if((*iter) == particle)
		{
			m_particle_list.erase(iter);
			break;
		}
	}
}
void ParticleLayer::update(float dt)
{
	int tCount = (int)m_particle_list.size();
	for(int i = 0 ; i < tCount; i++)
	{
		if(m_particle_list[i]->m_parent->m_parent->IsPause() == false)
			m_particle_list[i]->update(dt);
	}
}
void ParticleLayer::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	int tCount = (int)m_particle_list.size();
	for(int i = tCount - 1 ; i >= 0; i--)
	{
		if(m_particle_list[i]->m_parent->m_parent->GetEnable())
			m_particle_list[i]->draw();
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
}