#pragma once
#include<cmath>
#include <glad/glad.h>

#include <iostream>
const float PI = 3.1415926535f;
const float RAD = PI / 180.0f;
const float GRAD = 180.0f / PI;

struct Vec2
{
	float x = 0.0f, y = 0.0f;
};
struct Vec3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	Vec3() {};
	Vec3(const float& x,const float& y,const float& z):x(x),y(y),z(z) {};

	//�������� ������������
	Vec3(const Vec3& start, const Vec3& end, const float& time);
	void operator+=(const Vec3& temp);
	void operator-=(const Vec3& temp);
	Vec3 operator+ (const Vec3& temp) const;
	Vec3 operator- (const Vec3& temp) const;
	float operator* (const Vec3& temp) const;	// ��������� ������������
	Vec3 operator^ (const Vec3& temp) const;	// ��������� ������������
	Vec3 operator* (const float& temp)const;
	void Print();
};
struct Vec4
{
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
};
struct uVec4
{
	unsigned int x = 0, y = 0, z = 0, w = 0;
};


class Quaternion
{

	float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;
public:
	Quaternion() {};
	//�� ����� � ��������
	Quaternion(const float& x, const float& y, const float& z);
	//���������� ����������� ������������
	Quaternion(const Quaternion& start, const Quaternion& end, const float& time);
	//������� ������ ������������
	void Rotate_Vec3(Vec3& move);
	//���������� ������������ �����������
	void Conjugate(const Quaternion& temp);
	//������������ �����������
	void Normalize();
	// ������������ �����������
	void Invers();
	//��������� ������������
	Quaternion operator*(const Quaternion& temp);
	//���������� �� ����������
	void operator*=(const Quaternion& temp);
	//�������� �������� �����������
	void Get_Quaternion(float& X, float& Y, float& Z, float& W) const;
	void Set_Quaternion(const float& X, const float& Y, const float& Z, const float& W);
};

class Matrix
{
public:
	float matrix[16] = { 1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
	//������� 4�4
	Matrix() {};
	//������� �� �����������, �� ������� ���������������� ��� OpenGL
	Matrix(const Quaternion& temp);
	//������� �� ������� �����������
	Matrix(const Vec3& move);
	//������� �� ����������� � ������� �����������
	Matrix(const Quaternion& temp,const Vec3& move);

	//��������� �� �������
	const float* Get_link()const;
	//������������� ������� ��������, �� ������� ���������������� ��� OpenGL
	void Set_Projection(const float& fov, const float& near, const float& far, const float& aspect);
	//�������� ������� ���� �� ������ ����������� ���������� ������[12][13][14][15], �� ������� ���������������� ��� OpenGL
	void operator*(const Vec3& move);
	//������������ 4� ������ ������
	Matrix operator*(const Matrix& temp) const;
	void operator=(const Matrix& temp);
	//����������������
	void Transponir();
	//����������� ������ 3�3
	void Invers3x3();
	//����������� 4�4
	void Invers4x4();
	void Ymm(Vec3& temp);
	//������� � ���������� � ������ �����������
	void MatrixToQuaternion(Quaternion& quat, Vec3& move);
};

class Camera
{
	Quaternion orientation;
	Vec3 move;
	unsigned int Matrix_Buffer = 0;
	bool Free_Camera = false;
	bool Camera_Act = true;
	float Cam_X = 0.0f;
public:
	//������������� ������
	Camera();
	//�������� ��������������
	void Set_Move(Vec3& temp);
	//������������� ������� ��������
	void Set_Projection_Matrix(const float& fov, const float& near, const float& far, const float& aspect);
	//������������ ����� ��������� � ������� �������
	void Set_Free_Camera();
	//������������ �� ��� Z
	void Set_Rotate_View(const float& Z);
	//������������ �� ���� X � Y
	void Set_Rotate_View(const float& X, const float& Y);
	//������������� ������� �������, ���� ���� ���������
	void Set_View_Matrix();
private:
	//���������� ������� �������, ��� �������� �� ��������� � ������� ������
	void Set_Rotate_View_Reset();
};