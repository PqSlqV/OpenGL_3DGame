#include "Object.h"

C_Animation::C_Animation(std::string& name, std::ifstream& fobj)
{
	Animation_Name = name;
	Duration = 7.5f;

	int frsize;
	fobj.read((char*)&frsize, sizeof(int));
	Frame.resize(frsize);
	for (int i = 0; i < frsize; i++)
	{
		int fbone;
		fobj.read((char*)&fbone, sizeof(int));
		Frame[i].resize(fbone);
		int siof = sizeof(Frame_Bones);
		for (int j = 0; j < fbone; j++)
		{
			fobj.read((char*)&Frame[i][j], siof);
		}
	}
	Rate = (Frame.size() - 1) / Duration;
}

const unsigned int& C_Animation::Get_Bone_Id(const unsigned int& index, const unsigned int& time) const
{
	return Frame[time][index].Id;
}

const float& C_Animation::Get_Rate() const
{
	return Rate;
}

void C_Animation::Get_New_Time(float& time, const float& delta) const
{
	time += delta;
	if (time > Duration)
	{
		int over = int(time / Duration);
		time -= Duration * over;
	}
}

const Vec3 C_Animation::Get_Position(const unsigned int& index, const unsigned int& time1, const unsigned int& time2, const float& anim_delta) const
{
	return Vec3(Frame[time1][index].Position, Frame[time2][index].Position, anim_delta);
}

const Quaternion C_Animation::Get_Rotation(const unsigned int& index, const unsigned int& time1, const unsigned int& time2, const float& anim_delta) const
{

	return Quaternion(Frame[time1][index].Rotation, Frame[time2][index].Rotation, anim_delta);
}

const Matrix C_Animation::Get_Transform(const unsigned int& index, const unsigned int& frame1, const unsigned int& frame2, const float& anim_delta) const
{
	return Matrix(Get_Rotation(index, frame1, frame2, anim_delta), Get_Position(index, frame1, frame2, anim_delta));
}

bool C_Animation::operator<(const C_Animation& temp) const
{
	if (Animation_Name < temp.Animation_Name)
		return true;
	else
		return false;
}


// -------------------------------

C_Skelet::C_Skelet(std::string& name,std::ifstream& fobj)
{
	Skelet_Name = name;
	int size;
	fobj.read((char*)&size, sizeof(int));
	Bones.resize(size);

	int siof = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(Matrix) + sizeof(Matrix);
	for (int i = 0; i < size; i++)
	{
		fobj.read((char*)&Bones[i], siof);
	}

	fobj.read((char*)&Global_Transform, sizeof(Matrix));
}

const Matrix& C_Skelet::Get_Global() const
{
	return Global_Transform;
}

const Matrix& C_Skelet::Get_Parent_Local(const unsigned int& id) const
{
	return Bones[Bones[id].Parent_Id].Local_Transform;
}

const Matrix& C_Skelet::Get_Base_Transform(const unsigned int& id) const
{
	return Bones[id].Base_Transform;
}

const Matrix& C_Skelet::Get_Bind_Pose(const unsigned int& id) const
{
	return Bones[id].Bindpose;
}

const unsigned int C_Skelet::Get_Size() const
{
	return unsigned int(Bones.size());
}

bool C_Skelet::operator<(const C_Skelet& temp) const
{
	if (Skelet_Name < temp.Skelet_Name)
		return true;
	else
		return false;
}

// -------------------------------

C_Mesh::C_Mesh(const std::string& name)
{
	Mesh_Name = name;
	Load_Mesh();
}

C_Mesh::~C_Mesh()
{
	/*if (VAO != -1)
		glDeleteVertexArrays(1, &VAO);
	if (VBO != -1)
		glDeleteBuffers(1, &VBO);
	if (EBO != -1)
		glDeleteBuffers(1, &EBO);*/
}

const unsigned int& C_Mesh::Get_Texture() const
{
	return Texture;
}

const unsigned int& C_Mesh::Get_VAO() const
{
	return VAO;
}

const unsigned int& C_Mesh::Get_Index_Count() const
{
	return Index_Count;
}

Vec2 C_Mesh::Get_AABB() const
{
	float Height = AABB[0].Max.y - AABB[0].Min.y;
	float Radius;
	fabs(AABB[0].Max.x) > fabs(AABB[0].Max.z) ? Radius = fabs(AABB[0].Max.x) : Radius = fabs(AABB[0].Max.z);
	return Vec2(Height, Radius);
}

bool C_Mesh::operator<(const C_Mesh& temp) const
{
	if (Mesh_Name < temp.Mesh_Name)
		return true;
	else
		return false;
}

void C_Mesh::Load_Mesh()
{
	std::string path("Resources/Objects/" + Mesh_Name + ".mesh");
	std::ifstream fobj;
	fobj.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fobj.open(path, std::ios::binary);
	}
	catch (std::ifstream::failure&)
	{
		std::cout << "ERROR::OBJECT::LOAD_OBJECT::" << Mesh_Name << "::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		exit(-1);
	}
	struct Info {
		unsigned int Index_count = 0;
		unsigned int Texture_count = 0;
		bool textures = false;
		bool normal = false;
		bool bones = false;
		bool Animation = false;
		bool AABB = false;
	}info;

	fobj.read((char*)&info, sizeof(Info));

	//Vertex
	std::vector<unsigned int> Index;
	Index.resize(info.Index_count);
	Index_Count = info.Index_count;
	glCreateBuffers(1, &VBO);
	glCreateVertexArrays(1, &VAO);
	if (!info.textures && !info.normal && !info.bones)
	{
		std::vector<Vec3> Mesh;
		Mesh.resize(info.Index_count);
		int siof = sizeof(Vec3);
		for (unsigned int i = 0; i < info.Index_count; i++)
		{
			fobj.read((char*)&Mesh[i], siof);
			Index[i] = i;
		}
		glNamedBufferStorage(VBO, sizeof(Vec3) * Mesh.size(), &Mesh[0], GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vec3));
	}
	else if (!info.normal && !info.bones)
	{
		struct Vertex {
			Vec3 Position;
			Vec2 TexCoords;
		};
		std::vector<Vertex> Mesh;
		int siof = sizeof(Vertex);
		Mesh.resize(info.Index_count);
		for (unsigned int i = 0; i < info.Index_count; i++)
		{
			fobj.read((char*)&Mesh[i], siof);
			Index[i] = i;
		}
		glNamedBufferStorage(VBO, sizeof(Vertex) * Mesh.size(), &Mesh[0], GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	}
	else if (!info.bones)
	{
		struct Vertex {
			Vec3 Position;
			Vec2 TexCoords;
			Vec3 Normal;
			Vec3 Tangent;
			Vec3 Bitangent;
		};
		std::vector<Vertex> Mesh;
		Mesh.resize(info.Index_count);
		int siof = sizeof(Vertex);
		for (unsigned int i = 0; i < info.Index_count; i++)
		{
			fobj.read((char*)&Mesh[i], siof);
			Index[i] = i;
		}
		glNamedBufferStorage(VBO, sizeof(Vertex) * Mesh.size(), &Mesh[0], GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	}
	else
	{
		struct Vertex {
			Vec3 Position;
			Vec2 TexCoords;
			Vec3 Normal;
			Vec3 Tangent;
			Vec3 Bitangent;
			uVec4 Index;
			Vec4 Weight;
		};
		std::vector<Vertex> Mesh;
		Mesh.resize(info.Index_count);
		int siof = sizeof(Vertex);
		for (unsigned int i = 0; i < info.Index_count; i++)
		{
			fobj.read((char*)&Mesh[i], siof);
			Index[i] = i;
		}
		Skelets.insert(C_Skelet(Mesh_Name,fobj));

		glNamedBufferStorage(VBO, sizeof(Vertex) * Mesh.size(), &Mesh[0], GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	}
	if (info.Animation)
	{
		int size;
		fobj.read((char*)&size, sizeof(int));
		for (int i = 0; i < size; i++)
		{
			Animations.insert(C_Animation(Mesh_Name,fobj));
		}
	}
	if (info.AABB)
	{
		int size;
		fobj.read((char*)&size, sizeof(int));
		AABB.resize(size);
		int siof = sizeof(S_AABB);
		for (int i = 0; i < size; i++)
		{
			fobj.read((char*)&AABB[i], siof);
		}
	}
	fobj.close();
	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, sizeof(unsigned int) * Index.size(), &Index[0], GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(VAO, EBO);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);

	if (info.textures)
	{
		glEnableVertexArrayAttrib(VAO, 1);
		glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec3));
		glVertexArrayAttribBinding(VAO, 1, 0);
	}
	if (info.textures && info.normal)
	{
		glEnableVertexArrayAttrib(VAO, 2);
		glEnableVertexArrayAttrib(VAO, 3);
		glEnableVertexArrayAttrib(VAO, 4);
		glVertexArrayAttribFormat(VAO, 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3) + sizeof(Vec2));
		glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3) + sizeof(Vec2));
		glVertexArrayAttribFormat(VAO, 4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vec3) + sizeof(Vec2));
		glVertexArrayAttribBinding(VAO, 2, 0);
		glVertexArrayAttribBinding(VAO, 3, 0);
		glVertexArrayAttribBinding(VAO, 4, 0);
	}
	if (info.textures && info.normal && info.bones)
	{
		glEnableVertexArrayAttrib(VAO, 5);
		glEnableVertexArrayAttrib(VAO, 6);
		glVertexArrayAttribIFormat(VAO, 5, 4, GL_UNSIGNED_INT, 4 * sizeof(Vec3) + sizeof(Vec2));
		glVertexArrayAttribFormat(VAO, 6, 4, GL_FLOAT, GL_FALSE, sizeof(uVec4) + 4 * sizeof(Vec3) + sizeof(Vec2));
		glVertexArrayAttribBinding(VAO, 5, 0);
		glVertexArrayAttribBinding(VAO, 6, 0);
	}

	if (info.Texture_count != 0)
		Load_Texture(int(info.Texture_count));
}

void C_Mesh::Load_Texture(const unsigned int& count)
{
	stbi_set_flip_vertically_on_load(0);
	if (count == 1)
	{
		std::string path("Resources/Textures/" + Mesh_Name + ".png");
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &Texture);
			if (nrChannels == 3)
			{
				glTextureStorage2D(Texture, 1, GL_RGB8, width, height);
				glTextureSubImage2D(Texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 4)
			{
				glTextureStorage2D(Texture, 1, GL_RGBA8, width, height);
				glTextureSubImage2D(Texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
				std::cout << "ERROR::LOADIMAGE::" << path << "::CHANNELS" << std::endl;
		}
		else
			std::cout << "ERROR::LOADIMAGE::" << path << std::endl;
		stbi_image_free(data);

		glGenerateTextureMipmap(Texture);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		for (unsigned int i = 0; i < count; i++)
		{
			int width, height, nrChannels;
			std::string path("Resources/Textures/" + Mesh_Name + "_" + std::to_string(i) + ".png");
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				if (i == 0)
				{
					glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &Texture);
					glTextureStorage3D(Texture, 1, GL_RGBA8, width, height, count);
				}
				if (nrChannels == 3)
					glTextureSubImage3D(Texture, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
				else if (nrChannels == 4)
					glTextureSubImage3D(Texture, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
				else
					std::cout << "ERROR::LOADIMAGE::" << path << "::CHANNELS" << std::endl;
			}
			else
			{
				std::cout << "ERROR::LOADIMAGE::" << path << std::endl;
			}
			stbi_image_free(data);
		}
		glGenerateTextureMipmap(Texture);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

// -------------------------------

C_Static_Mesh::C_Static_Mesh(const std::string& name)
{
	It_Mesh = Meshes.find(name);
	if (It_Mesh == Meshes.end())
		Meshes.insert(name);
	It_Mesh = Meshes.find(name);
	if (It_Mesh == Meshes.end())
	{
		std::cout << "ERROR::DYNAMIC_MESH::NOT_FIND::SKELET_or_ANIMATION";
		exit(-1);
	}
}

void C_Static_Mesh::Set_Shader(const Shader& shader)
{
	Shader_id = shader.Get_Id();
	Model_loc = glGetUniformLocation(Shader_id, "Model");
	glProgramUniformMatrix4fv(Shader_id, Model_loc, 1, GL_FALSE, Model.Get_link());
	glProgramUniform1i(Shader_id, glGetUniformLocation(Shader_id, "Texture"), 0);
}

void C_Static_Mesh::Draw()
{
	glBindTextureUnit(0, (*It_Mesh).Texture);
	glBindVertexArray((*It_Mesh).VAO);
	glDrawElements(GL_TRIANGLES, (*It_Mesh).Index_Count, GL_UNSIGNED_INT, 0);
}

// -------------------------------

C_Dynamic_Mesh::C_Dynamic_Mesh(const std::string& name) :C_Static_Mesh(name)
{
	It_Skelet = Skelets.begin();
	It_Animation = Animations.begin();
	if (It_Skelet == Skelets.end() || It_Animation == Animations.end())
	{
		std::cout << "ERROR::DYNAMIC_MESH::NOT_FIND::SKELET_or_ANIMATION";
		exit(-1);
	}
}

void C_Dynamic_Mesh::Set_Shader(const Shader& shader)
{
	Shader_id = shader.Get_Id();
	Model_loc = glGetUniformLocation(Shader_id, "Model");
	Animation_loc = glGetUniformLocation(Shader_id, "Animation");
	glProgramUniform1i(Shader_id, glGetUniformLocation(Shader_id, "Texture"), 0);
}

void C_Dynamic_Mesh::Set_Animation(const unsigned int& anim)
{
	//if (anim > Animation.size())
	//{
	//	std::cout << "Animation > animation.size" << std::endl;
	//	return;
	//}
	Selected_Animation = anim;
	Animation_Time = 0.0f;
	New_Animation = true;
}

void C_Dynamic_Mesh::Play_Animation(const float& delta)
{
	if (!New_Animation)
		It_Animation->Get_New_Time(Animation_Time, delta);
	else
		New_Animation = false;

	std::vector<Matrix> Final;
	Final.resize(It_Skelet->Get_Size());
	std::vector<Matrix> Local;
	Final.resize(It_Skelet->Get_Size());
	int j = 0;

	float anim_delta = It_Animation->Get_Rate() * Animation_Time;
	unsigned int frame1 = unsigned int(anim_delta);
	anim_delta -= frame1;
	unsigned int frame2 = { frame1 + 1 };

	for (unsigned int i = 0; i < It_Skelet->Get_Size(); i++)
	{
		if (i != 0)
		{
			Matrix parent_transform;
			parent_transform = It_Skelet->Get_Parent_Local(i);

			if (i == It_Animation->Get_Bone_Id(j, frame1))
			{
				Local[i] = parent_transform * It_Animation->Get_Transform(j, frame1, frame2, anim_delta);
				j++;
			}
			else
			{
				Local[i] = parent_transform * It_Skelet->Get_Base_Transform(i);
			}
		}
		else
		{
			Local[i] = It_Skelet->Get_Base_Transform(i);
		}
		Final[i] = It_Skelet->Get_Global() * Local[i] * It_Skelet->Get_Bind_Pose(i);
	}

	glProgramUniformMatrix4fv(Shader_id, Animation_loc, int(Final.size()), GL_FALSE, Final[0].Get_link());
}

void Sunshine::Draw()
{
	Sun_Move();
	glBindTextureUnit(0, (*It_Mesh).Texture);
	glEnable(GL_BLEND);
	glBindVertexArray((*It_Mesh).VAO);
	glDrawElements(GL_TRIANGLE_FAN, (*It_Mesh).Index_Count, GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);
}

void Sunshine::Sun_Move()
{
	Pos_world.x = 1000.0f * (Model.matrix[12] = -5.0f * (Model.matrix[5] = Model.matrix[0] = cos(alf * RAD))); //x
	Pos_world.y = 1000.0f * (Model.matrix[13] = 5.0f * (Model.matrix[1] = Model.matrix[4] = sin(alf * RAD))); //y
	glProgramUniformMatrix4fv(Shader_id, Model_loc, 1, GL_FALSE, Model.Get_link());
	glNamedBufferSubData(Sun_Buffer, 0, sizeof(Pos_world), &Pos_world);

	alf += 0.3f;
	if (alf > 359.0f)
		alf = 0.0f;
}

void NPC::Draw()
{
	Set_Move();
	glBindTextureUnit(0, (*It_Mesh).Texture);
	glBindVertexArray((*It_Mesh).VAO);
	glDrawElements(GL_TRIANGLES, (*It_Mesh).Index_Count, GL_UNSIGNED_INT, 0);
}

bool NPC::Collision(const NPC& npc)
{
	if (Position.y<npc.Position.y + npc.Height && Position.y + Height > npc.Position.y)
	{
		float centr = (Position.x - npc.Position.x) * (Position.x - npc.Position.x) + (Position.z - npc.Position.z) * (Position.z - npc.Position.z);
		float rad1 = (Radius - npc.Radius) * (Radius - npc.Radius);
		float rad2 = (Radius + npc.Radius) * (Radius + npc.Radius);
		if (centr <= rad2 && centr >= rad1)
			return true;
	}
	return false;
}

/*glm::mat4 Global(1.0f);
	glm_conv(Global, global_trans);
	std::vector<glm::mat4> fina;
	fina.resize(Bones.size());
	int j = 0;
	for (int i = 0; i < Bones.size(); i++)
	{
		glm::mat4 parent_transform(1.0f);

		if (i != 0)
		{
			parent_transform = Transf[Bones[i].Parent_Id];
		}
		if(i == Animation[0].Frame[time][j].Id)
		{
			glm::mat4 Translete(1.0f);
			Translete = translate(Translete, move);
			glm::mat4 Rotate(1.0f);
				Rotate = glm::mat4_cast(rot);
				Rotate = Translete * Rotate;
			Transf[i] = parent_transform * Rotate;
			j++;
		}
		else
		{
			glm::mat4 temp(1.0f);
			glm_conv(temp, Bones[i].Transform);
			Transf[i] = parent_transform * temp;
		}
		glm::mat4 temp(1.0f);
		glm_conv(temp, Bones[i].Bindpose);
		fina[i] = Global * Transf[i] * temp;
	}
	glProgramUniformMatrix4fv(Shader_id, Animation_loc, int(fina.size()), GL_FALSE, glm::value_ptr(fina[0]));
}
*/
