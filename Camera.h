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

	//Линейная интерполяция
	Vec3(const Vec3& start, const Vec3& end, const float& time);
	void operator+=(const Vec3& temp);
	void operator-=(const Vec3& temp);
	Vec3 operator+ (const Vec3& temp) const;
	Vec3 operator- (const Vec3& temp) const;
	float operator* (const Vec3& temp) const;	// Скалярное произведение
	Vec3 operator^ (const Vec3& temp) const;	// Векторное произведение
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
	//Из элера в градусах
	Quaternion(const float& x, const float& y, const float& z);
	//Кватернион сферической интерполяции
	Quaternion(const Quaternion& start, const Quaternion& end, const float& time);
	//Вращает вектор кватернионом
	void Rotate_Vec3(Vec3& move);
	//Вычисление сопряженного кватерниона
	void Conjugate(const Quaternion& temp);
	//Нормализация кватерниона
	void Normalize();
	// Инвертезиция кватерниона
	void Invers();
	//Умножение кватернионов
	Quaternion operator*(const Quaternion& temp);
	//Домнажение на кватернион
	void operator*=(const Quaternion& temp);
	//Передает значения кватерниона
	void Get_Quaternion(float& X, float& Y, float& Z, float& W) const;
	void Set_Quaternion(const float& X, const float& Y, const float& Z, const float& W);
};

class Matrix
{
public:
	float matrix[16] = { 1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
	//Матрица 4х4
	Matrix() {};
	//Матрица из кватерниона, не требует транспонирования для OpenGL
	Matrix(const Quaternion& temp);
	//Матрица из вектора перемещения
	Matrix(const Vec3& move);
	//Матрица из кватерниона и вектора перемещения
	Matrix(const Quaternion& temp,const Vec3& move);

	//Указатель на матрицу
	const float* Get_link()const;
	//Устанавливает матрицу проекции, не требует транспонирования для OpenGL
	void Set_Projection(const float& fov, const float& near, const float& far, const float& aspect);
	//Умножает матрицу вида на вектор перемещения изменяются только[12][13][14][15], не требует транспонирования для OpenGL
	void operator*(const Vec3& move);
	//Перемножение 4х мерных матриц
	Matrix operator*(const Matrix& temp) const;
	void operator=(const Matrix& temp);
	//Транспонирование
	void Transponir();
	//Инвентирует только 3х3
	void Invers3x3();
	//Инвентирует 4х4
	void Invers4x4();
	void Ymm(Vec3& temp);
	//Матрица в кватернион и вектор перемещения
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
	//Инициализация камеры
	Camera();
	//Изменяет местоположение
	void Set_Move(Vec3& temp);
	//Устанавливает матрицу проекции
	void Set_Projection_Matrix(const float& fov, const float& near, const float& far, const float& aspect);
	//Переключение между свободной и обычной камерой
	void Set_Free_Camera();
	//Поворачивает по оси Z
	void Set_Rotate_View(const float& Z);
	//Поворачивает по осям X и Y
	void Set_Rotate_View(const float& X, const float& Y);
	//Устанавливает видовую матрицу, если были изменения
	void Set_View_Matrix();
private:
	//Сбрасывает видовую матрицу, для перехода из свободной в обычную камеру
	void Set_Rotate_View_Reset();
};