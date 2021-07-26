#pragma once
#include "Shader.h"
#include "Camera.h"
#include <vector>
#include <set>
#include <iterator>
#include "stb_image.h"

//Множество анимаций
class C_Animation
{
	struct Frame_Bones {
		unsigned int Id = 0;
		Vec3 Position;
		Quaternion Rotation;
	};
	std::string Animation_Name;

	std::vector< std::vector<Frame_Bones>>Frame;
	float Duration;
	float Rate;
public:
	C_Animation(std::string& name, std::ifstream& fobj);

	const unsigned int& Get_Bone_Id(const unsigned int& index, const unsigned int& time) const;
	//Проверяет и выдает новое время анимации изменяя на дельту
	void Get_New_Time(float& time, const float& delta) const;
	//Возвращает частату анимации
	const float& Get_Rate() const;
	//Возвращает интерполированный вектор позиции
	const Vec3 Get_Position(const unsigned int& index, const unsigned int& time1, const unsigned int& time2, const float& anim_delta) const;
	//Возвращает сферически интерполированный кватернион поворота
	const Quaternion Get_Rotation(const unsigned int& index, const unsigned int& time1, const unsigned int& time2, const float& anim_delta) const;
	//Возвращает матрицу трансформации
	const Matrix Get_Transform(const unsigned int& index, const unsigned int& frame1, const unsigned int& frame2, const float& anim_delta) const;
	bool operator<(const C_Animation& temp) const;
};
extern std::set<C_Animation> Animations;


//Множество скелетов
class C_Skelet
{
	struct S_Bone
	{
		unsigned int Id = 0;
		unsigned int Parent_Id = 0;
		Matrix Base_Transform;
		Matrix Bindpose;

		Matrix Local_Transform;
	};
	std::string Skelet_Name;
	std::vector<S_Bone> Bones;
	Matrix Global_Transform;
public:
	C_Skelet(std::string& name, std::ifstream& fobj);
	const Matrix& Get_Global() const;
	const Matrix& Get_Parent_Local(const unsigned int& id) const;
	const Matrix& Get_Base_Transform(const unsigned int& id) const;
	const Matrix& Get_Bind_Pose(const unsigned int& id) const;
	const unsigned int Get_Size() const;
	bool operator<(const C_Skelet& temp) const;
};
extern std::set<C_Skelet> Skelets;



//Множество мешей
class C_Mesh
{
	struct S_AABB
	{
		unsigned int Id = 0;
		Vec3 Min;
		Vec3 Max;
	};
	std::vector<S_AABB> AABB;
public:
	std::string Mesh_Name;
	unsigned int Texture = -1;
	unsigned int VAO = -1;
	unsigned int VBO = -1;
	unsigned int EBO = -1;
	unsigned int Index_Count = -1;

	C_Mesh(const std::string& name);
	~C_Mesh();
	const unsigned int& Get_Texture() const;
	const unsigned int& Get_VAO() const;
	const unsigned int& Get_Index_Count() const;
	Vec2 Get_AABB()const;
	bool operator<(const C_Mesh& temp) const;
private:
	void Load_Mesh();
	void Load_Texture(const unsigned int& count);
};
extern std::set<C_Mesh> Meshes;



//Стандартный меш
class C_Static_Mesh
{
protected:
	std::set<C_Mesh>::iterator It_Mesh = Meshes.end();
	unsigned int Model_loc = -1;
	Matrix Model;
	unsigned int Shader_id = -1;
public:
	C_Static_Mesh(const std::string& name);
	virtual void Set_Shader(const Shader& shader);
	virtual void Draw();
};


//Меш с анимацией
class C_Dynamic_Mesh :public C_Static_Mesh
{
protected:
	std::set<C_Skelet>::iterator It_Skelet = Skelets.end();
	std::set<C_Animation>::iterator It_Animation = Animations.end();
	unsigned int Selected_Animation = 0;
	float Animation_Time = 0.0f;
	bool New_Animation = true;
	unsigned int Animation_loc = -1;
public:
	C_Dynamic_Mesh(const std::string& name);

	void Set_Shader(const Shader& shader) override;
	void Set_Animation(const unsigned int& anim);
	void Play_Animation(const float& delta);
};



//Класс мира(терреин)
class World : public C_Static_Mesh
{
public:
	World() :C_Static_Mesh("Land") {}
};



//Класс солнца
class Sunshine : public C_Static_Mesh
{
	float alf = 0.0f;
	Vec2 Pos_world;
	unsigned int Sun_Buffer = -1;
public:
	Sunshine() :C_Static_Mesh("Sun")
	{
		glCreateBuffers(1, &Sun_Buffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, Sun_Buffer);
		glNamedBufferData(Sun_Buffer, sizeof(Pos_world), NULL, GL_DYNAMIC_DRAW);
	}

	void Draw() override;
	void Sun_Move();
};



//Класс неписей
class NPC : public C_Dynamic_Mesh
{
	Quaternion Orientation;
	Vec3 Position;
	float Radius;
	float Height;

public:
	NPC() :C_Dynamic_Mesh("Cube")
	{
		Vec2 temp = It_Mesh->Get_AABB();
		Height = temp.x;
		Radius = temp.y;
	}

	void Set_Position(Vec3& temp)
	{
		Orientation.Rotate_Vec3(temp);
		Position += temp;
	}
	void Set_Position(Vec3& temp, const NPC& mesh)
	{
		Orientation.Rotate_Vec3(temp);
		Position += temp;

		if (Collision(mesh))
		{
			Position -= temp;
		}
	}
	void Set_RotateY(const float& angel)
	{
		Quaternion tempY(0.0f, angel, 0.0f);
		Orientation *= tempY;
	}
	void Set_Move()
	{
		Model = Matrix(Orientation, Position);
		glProgramUniformMatrix4fv(Shader_id, Model_loc, 1, GL_FALSE, Model.Get_link());
	}
	void Draw() override;
	bool Collision(const NPC& npc);
};