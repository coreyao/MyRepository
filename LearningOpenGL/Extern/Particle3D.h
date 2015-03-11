#ifndef __Particle3D__
#define __Particle3D__

#include "cocos2d.h"
#include<map>
#include "Utils.h"

USING_NS_CC;

class Emitter3D;
class ParticleInstance3D;

template<class type>
class LinerType
{
public:
	struct NodeItem
	{
		type start_value;
		type end_value;
		float start_time;
		float during_time;
	};

	LinerType()
	{
		m_loops = -1;

		m_is_run = false;
		m_time = 0.0f;
	}
	void AddPoint(type start,type end,float start_time,float during_time)
	{
		NodeItem item;
		item.start_value = start;
		item.end_value = end;

		item.during_time = during_time;
		item.start_time = start_time;

		node_list.push_back(item);

		if(node_list.size() > 0)
		{
			m_cur_value = node_list[0].start_value;
		}

		m_during_time = start_time + during_time;
	}
	void Init()
	{
		m_is_run = true;
	}
	void SetLoop(int is_loop)
	{
		m_loops = is_loop;
	}
	bool IsRun(){return m_is_run;}
	void Update(float dt)
	{
		if(m_is_run == false)
			return;

		m_time += dt;
		if(m_time >= m_during_time)
		{
			m_cur_value = node_list[node_list.size() - 1].end_value;

			if(m_loops < 0)
			{
				m_time = 0.0f;
			}
			else if(m_loops > 0)
			{
				m_time = 0.0f;
				m_loops--;
			}
			else
				return;
		}
		else
		{
			float start_time = 0.0f;
			float end_time = 0.0f;

			type start_value;
			type end_value;

			bool is_find = false;

			int tCount = (int)node_list.size();
			for(int i = 0 ; i < tCount; i++)
			{
				if(m_time >= node_list[i].start_time && 
					m_time <= node_list[i].start_time + node_list[i].during_time)
				{
					end_time = node_list[i].start_time + node_list[i].during_time;
					end_value = node_list[i].end_value;

					start_time = node_list[i].start_time;
					start_value = node_list[i].start_value;

					is_find = true;

					break;
				}
			}

			if(is_find)
			{
				float t_ratio = (float)(m_time - start_time) / (float)(end_time - start_time);

				if(t_ratio < 0.0f)
					t_ratio = 0.0f;
				if(t_ratio > 1.0f)
					t_ratio = 1.0f;

				m_cur_value = Lerp(start_value,end_value,t_ratio);
			}			
		}
	}
	type GetValue()
	{
		return m_cur_value;
	}

private:
	int m_loops;
	std::vector<NodeItem> node_list;
	
	float m_time;
	type m_cur_value;
	bool m_is_run;
	float m_during_time;
};
enum EmitterType
{
	kFollowCamera,
	kFollowTarget,
	kFree,
	kEmission,
};
class Particle3DSystem
{
public:
	Particle3DSystem();
	Particle3DSystem(const char *jsonfile,const char *texPath = "vfx/");

	void Init(const char *jsonfile,const char *texPath);

	void AddEmitter3D(Emitter3D *);

	void update(float dt);
	void SetPosition(const Vec3 &);
	Vec3 GetPosition();

	void Pause();
	void Resume();
	void Reset();

	void SetEnable(bool ret);
	bool GetEnable();

	bool IsPause(){return is_pause;}

	int GetParticleInstanceTotalCount();
	void Clear();

	void SetTexturePath(std::string texPath){tex_name_path = texPath;}

	void SetRotation(const Quaternion &);
	Quaternion GetRotation();

	void SetScale(const Vec3 &);
	Vec3 GetScale();

	std::vector<Emitter3D *> m_emitter_list;

	Mat4 m_vp_transform;
	Mat4 m_view_transform;

	Node *m_layer_parent;

	GLProgramState *_glProgramState;

	std::string tex_name_path;

	std::string m_file_parse;
private:
	bool is_pause;
	Vec3 m_pos;
	Quaternion m_rotate;
	Vec3 m_rootScale;
	bool m_enable;
};
class Emitter3D
{
public:
	Emitter3D(Particle3DSystem *);

	void init();

	void update(float dt);

	float m_init_angle;
	Vec2 m_init_angle_rand;

	float m_interval_time;
	float m_delay_time;
	float m_during_time;
	float m_during_total_time;
	float m_time;
	int m_loop;

	Vec2 angle_xy;

	float m_rotate_Z_init_angle;
	Vec2  m_rotate_Z_init_angle_rand;

	float m_rotate_Z_angle_speed;
	Vec2 m_rotate_Z_angle_speed_rand;

	bool m_is_local;
	int layerid;

	EmitterType m_emitter_type;

	Particle3DSystem *m_parent;

	Vec3 follow_pos;

	Vec3 m_move_dir;
	Vec3 m_graiy_dir;
	Vec3 m_wind_dir;
	float m_gravity_power;

	Vec2 m_emissionHangle;
	Vec2 m_emissionVangle;

	float AccSpeed;
	Vec2 AccSpeed_rand;

	float InitSpeed;
	Vec2 InitSpeed_rand;

	float ResiSpeed;
	Vec2 ResiSpeed_rand;

	float particle_init_zangle;
	Vec2 particle_init_zangle_rand;

	float particle_zangle_speed;
	Vec2 particle_zangle_speed_rand;

	std::string particle_texture_name;
	Vec2 particle_texture_info;

	int particle_blend_mode;

	Vec3 particle_self_rotate;

	Vec2 particle_during_time_rand;

	Vec2 particle_texture_speed_rand;

	LinerType<Vec3> m_particle_size_control;
	LinerType<Vec4> m_particle_color_control;

	int max_particle;

	std::vector<ParticleInstance3D *> m_particle_instance_list;
};
class ParticleInstance3D
{
public:
	ParticleInstance3D(Emitter3D *);
	~ParticleInstance3D();

	void update(float dt);
	void draw();

	void buildVertex();
	void resetVertex();
	void clearVertex();

	void SetPosition(const Vec3 &pos);
	Vec3 GetPosition();

	void SetSize(const Vec3 &size);
	Vec3 GetSize();

	bool IsLife(){return m_is_life;}
public:
	Mat4 m_parent_matrix;
	Vec3 m_size;
	Vec4 m_color;

	Mat4 m_model_transform;
	Mat4 m_vp_transform;

	int layerId;

	Emitter3D *m_parent;
	Vec3 m_pos;
	Vec3 m_move_dir;
	Vec3 m_graiy_dir;
	float m_graity_acc_speed;
	Vec3 m_wind_dir;
	Vec3 m_self_rotate;
	float m_z_angle;
	float m_z_angle_speed;

	float acce_speed;
	float init_speed;
	float resi_speed;

	Vec3 m_parent_pos;

	float particle_offset_rotate;

	LinerType<Vec3> m_size_control;
	LinerType<Vec4> m_color_control;

	int blend_mode;//0 blend 1 additive
	
	bool m_is_dirty;
	bool m_is_life;

	float m_frame_id;
	Vec2 m_texture_info;
	float m_texture_speed;
	float m_time;
	float m_during_time;
	Texture2D *tex;

	////////////////////////
	Mesh::PrimitiveType _primitiveType;
	Mesh::IndexFormat _indexFormat;
	GLuint _vertexBuffer;
	GLuint _indexBuffer;

	struct _Vertex
	{
		Vec3 pos;
		Color4B color;
		Vec2 uv;
	};
	std::vector<_Vertex> _vertexs;
	std::vector<unsigned short> _indices;

	GLProgramState *_glProgramState;

	GLuint u_texture_info;
	GLuint u_add_color;
};

class ParticleLayer : public Node
{
public:
	void AddParticle(ParticleInstance3D *);
	void RemoveParticle(ParticleInstance3D *);

	void SetEnable(bool ret);
	bool GetEnable();

	void update(float);
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
public:
	Mat4 m_vp_transform;
	std::vector<ParticleInstance3D *>m_particle_list;
};

#endif