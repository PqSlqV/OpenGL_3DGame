#include "camera.h"


Vec3::Vec3(const Vec3& start, const Vec3& end, const float& time)
{
	float scale0 = 1.0f - time;
	float scale1 = time;
	(*this) = (start * scale0) + (end * scale1);
}

void Vec3::operator+=(const Vec3& temp)
{
	(*this) = (*this) + temp;
}

void Vec3::operator-=(const Vec3& temp)
{
	(*this) = (*this) - temp;
}

Vec3 Vec3::operator+(const Vec3& temp) const
{
	return{ x + temp.x, y + temp.y, z + temp.z };
}

Vec3 Vec3::operator-(const Vec3& temp) const
{
	return{ x - temp.x, y - temp.y, z - temp.z }; 
}

float Vec3::operator*(const Vec3& temp) const
{
	return{ x * temp.x + y * temp.y + z * temp.z };
}

Vec3 Vec3::operator^(const Vec3& temp) const
{
	return{ y * temp.z - z * temp.y, z * temp.x - x * temp.z, x * temp.y - y * temp.x };
}

Vec3 Vec3::operator*(const float& temp) const
{
	return Vec3{ x * temp, y * temp, z * temp };
}

void Vec3::Print()
{
	std::cout << x << " " << y << " " << z << "\n";
}


//
//----------------Quaternion-----------------
//


Quaternion::Quaternion(const float& x, const float& y, const float& z)
{
	float rad = RAD / 2.0f;
	float cy = cos(z * rad);
	float sy = sin(z * rad);
	float cr = cos(x * rad);
	float sr = sin(x * rad);
	float cp = cos(y * rad);
	float sp = sin(y * rad);

	this->w = cy * cr * cp + sy * sr * sp;
	this->x = cy * sr * cp - sy * cr * sp;
	this->y = cy * cr * sp + sy * sr * cp;
	this->z = sy * cr * cp - cy * sr * sp;
	Normalize();
}

Quaternion::Quaternion(const Quaternion& start, const Quaternion& end, const float& time)
{
	float omega, cosom, sinom, scale0, scale1;
	// косинус угла
	cosom = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;
	Quaternion temp(end);
	if (cosom < 0.0f)
	{
		cosom = -cosom;
		temp.Invers();
		temp.w = -temp.w;
	}

	if ((1.0f - cosom) > 0.00001f)
	{
		// стандартный случай (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0f - time) * omega) / sinom;
		scale1 = sin(time * omega) / sinom;
	}
	else
	{
		// если маленький угол - линейна€ интерпол€ци€
		scale0 = 1.0f - time;
		scale1 = time;
	}

	this->x = scale0 * start.x + scale1 * temp.x;
	this->y = scale0 * start.y + scale1 * temp.y;
	this->z = scale0 * start.z + scale1 * temp.z;
	this->w = scale0 * start.w + scale1 * temp.w;
}


void Quaternion::Rotate_Vec3(Vec3& move)
{
	Quaternion temp(*this);
	float move_x = move.x;
	float move_y = move.y;
	float move_z = move.z;
	float ww = temp.w * temp.w;
	float xx = temp.x * temp.x;
	float yy = temp.y * temp.y;
	float zz = temp.z * temp.z;
	float wx = temp.w * temp.x;
	float wy = temp.w * temp.y;
	float wz = temp.w * temp.z;
	float xy = temp.x * temp.y;
	float xz = temp.x * temp.z;
	float yz = temp.y * temp.z;

	move.x = ww * move_x + 2 * wy * move_z - 2 * wz * move_y + xx * move_x + 2 * xy * move_y + 2 * xz * move_z - zz * move_x - yy * move_x;
	move.y = 2 * xy * move_x + yy * move_y + 2 * yz * move_z + 2 * wz * move_x - zz * move_y + ww * move_y - 2 * wx * move_z - xx * move_y;
	move.z = 2 * xz * move_x + 2 * yz * move_y + zz * move_z - 2 * wy * move_x - yy * move_z + 2 * wx * move_y - xx * move_z + ww * move_z;
}

void Quaternion::Conjugate(const Quaternion& temp)
{
	w = temp.w;
	x = -temp.x;
	y = -temp.y;
	z = -temp.z;
}

void Quaternion::Normalize()
{
	float len = sqrt(w * w + x * x + y * y + z * z);
	w /= len;
	x /= len;
	y /= len;
	z /= len;
}

void Quaternion::Invers()
{
	x = -x;
	y = -y;
	z = -z;
}

Quaternion Quaternion::operator*(const Quaternion& temp)
{
	Quaternion result;
	result.w = w * temp.w - x * temp.x - y * temp.y - z * temp.z;
	result.x = w * temp.x + x * temp.w + y * temp.z - z * temp.y;
	result.y = w * temp.y + y * temp.w + z * temp.x - x * temp.z;
	result.z = w * temp.z + z * temp.w + x * temp.y - y * temp.x;
	return result;
}

void Quaternion::operator*=(const Quaternion& temp)
{
	(*this) = (*this)*temp;
}

void Quaternion::Get_Quaternion(float& X, float& Y, float& Z, float& W) const
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}

void Quaternion::Set_Quaternion(const float& X, const float& Y, const float& Z, const float& W)
{
	x = X;
	y = Y;
	z = Z;
	w = W;
}

//
//----------------Matrix-----------------
//


Matrix::Matrix(const Quaternion& temp)
{
	float X, Y, Z, W;
	temp.Get_Quaternion(X, Y, Z, W);
	matrix[0] = 1.0f - 2.0f * (Y * Y + Z * Z);
	matrix[1] = 2.0f * (X * Y + Z * W);
	matrix[2] = 2.0f * (X * Z - Y * W);

	matrix[4] = 2.0f * (X * Y - Z * W);
	matrix[5] = 1.0f - 2.0f * (X * X + Z * Z);
	matrix[6] = 2.0f * (Y * Z + X * W);

	matrix[8] = 2.0f * (X * Z + Y * W);
	matrix[9] = 2.0f * (Y * Z - X * W);
	matrix[10] = 1.0f - 2.0f * (X * X + Y * Y);
}
Matrix::Matrix(const Vec3& move)
{
	(*this)* move;
}
Matrix::Matrix(const Quaternion& temp, const Vec3& move)
{
	float X, Y, Z, W;
	temp.Get_Quaternion(X, Y, Z, W);
	matrix[0] = 1.0f - 2.0f * (Y * Y + Z * Z);
	matrix[1] = 2.0f * (X * Y + Z * W);
	matrix[2] = 2.0f * (X * Z - Y * W);

	matrix[4] = 2.0f * (X * Y - Z * W);
	matrix[5] = 1.0f - 2.0f * (X * X + Z * Z);
	matrix[6] = 2.0f * (Y * Z + X * W);

	matrix[8] = 2.0f * (X * Z + Y * W);
	matrix[9] = 2.0f * (Y * Z - X * W);
	matrix[10] = 1.0f - 2.0f * (X * X + Y * Y);
	matrix[12] = move.x;
	matrix[13] = move.y;
	matrix[14] = move.z;
}
const float* Matrix::Get_link() const
{
	return matrix;
}

void Matrix::Set_Projection(const float& fov, const float& near, const float& far, const float& aspect)
{
	float tanHalf = tan(fov * RAD / 2.0f);

	matrix[0] = 1.0f / (aspect * tanHalf);
	matrix[5] = 1.0f / (tanHalf);
	matrix[10] = -(far + near) / (far - near);
	matrix[11] = -1.0f;
	matrix[14] = -(2.0f * far * near) / (far - near);
	matrix[15] = 0.0f;
}

void Matrix::operator*(const Vec3& move)
{
	matrix[12] = move.x * matrix[0] + move.y * matrix[4] + move.z * matrix[8] + matrix[12];
	matrix[13] = move.x * matrix[1] + move.y * matrix[5] + move.z * matrix[9] + matrix[13];
	matrix[14] = move.x * matrix[2] + move.y * matrix[6] + move.z * matrix[10] + matrix[14];
	matrix[15] = move.x * matrix[3] + move.y * matrix[7] + move.z * matrix[11] + matrix[15];
}

Matrix Matrix::operator*(const Matrix& temp) const
{
	Matrix ret;
	ret.matrix[0] = temp.matrix[0] * matrix[0] + temp.matrix[1] * matrix[4] + temp.matrix[2] * matrix[8] + temp.matrix[3] * matrix[12];
	ret.matrix[1] = temp.matrix[0] * matrix[1] + temp.matrix[1] * matrix[5] + temp.matrix[2] * matrix[9] + temp.matrix[3] * matrix[13];
	ret.matrix[2] = temp.matrix[0] * matrix[2] + temp.matrix[1] * matrix[6] + temp.matrix[2] * matrix[10] + temp.matrix[3] * matrix[14];
	ret.matrix[3] = temp.matrix[0] * matrix[3] + temp.matrix[1] * matrix[7] + temp.matrix[2] * matrix[11] + temp.matrix[3] * matrix[15];

	ret.matrix[4] = temp.matrix[4] * matrix[0] + temp.matrix[5] * matrix[4] + temp.matrix[6] * matrix[8] + temp.matrix[7] * matrix[12];
	ret.matrix[5] = temp.matrix[4] * matrix[1] + temp.matrix[5] * matrix[5] + temp.matrix[6] * matrix[9] + temp.matrix[7] * matrix[13];
	ret.matrix[6] = temp.matrix[4] * matrix[2] + temp.matrix[5] * matrix[6] + temp.matrix[6] * matrix[10] + temp.matrix[7] * matrix[14];
	ret.matrix[7] = temp.matrix[4] * matrix[3] + temp.matrix[5] * matrix[7] + temp.matrix[6] * matrix[11] + temp.matrix[7] * matrix[15];

	ret.matrix[8] = temp.matrix[8] * matrix[0] + temp.matrix[9] * matrix[4] + temp.matrix[10] * matrix[8] + temp.matrix[11] * matrix[12];
	ret.matrix[9] = temp.matrix[8] * matrix[1] + temp.matrix[9] * matrix[5] + temp.matrix[10] * matrix[9] + temp.matrix[11] * matrix[13];
	ret.matrix[10] = temp.matrix[8] * matrix[2] + temp.matrix[9] * matrix[6] + temp.matrix[10] * matrix[10] + temp.matrix[11] * matrix[14];
	ret.matrix[11] = temp.matrix[8] * matrix[3] + temp.matrix[9] * matrix[7] + temp.matrix[10] * matrix[11] + temp.matrix[11] * matrix[15];

	ret.matrix[12] = temp.matrix[12] * matrix[0] + temp.matrix[13] * matrix[4] + temp.matrix[14] * matrix[8] + temp.matrix[15] * matrix[12];
	ret.matrix[13] = temp.matrix[12] * matrix[1] + temp.matrix[13] * matrix[5] + temp.matrix[14] * matrix[9] + temp.matrix[15] * matrix[13];
	ret.matrix[14] = temp.matrix[12] * matrix[2] + temp.matrix[13] * matrix[6] + temp.matrix[14] * matrix[10] + temp.matrix[15] * matrix[14];
	ret.matrix[15] = temp.matrix[12] * matrix[3] + temp.matrix[13] * matrix[7] + temp.matrix[14] * matrix[11] + temp.matrix[15] * matrix[15];
	return ret;

}

void Matrix::operator=(const Matrix& temp)
{
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = temp.matrix[i];
	}
}

void Matrix::Transponir()
{
	Matrix temp(*this);
	//0
	matrix[1] = temp.matrix[4];
	matrix[2] = temp.matrix[8];
	matrix[3] = temp.matrix[12];

	matrix[4] = temp.matrix[1];
	//5
	matrix[6] = temp.matrix[9];
	matrix[7] = temp.matrix[13];

	matrix[8] = temp.matrix[2];
	matrix[9] = temp.matrix[6];
	//10
	matrix[11] = temp.matrix[14];

	matrix[12] = temp.matrix[3];
	matrix[13] = temp.matrix[7];
	matrix[14] = temp.matrix[11];
}

void Matrix::Invers3x3()
{
	float det = matrix[0] * (matrix[5] * matrix[10] - matrix[9] * matrix[6]) -
		matrix[1] * (matrix[4] * matrix[10] - matrix[6] * matrix[8]) +
		matrix[2] * (matrix[4] * matrix[9] - matrix[5] * matrix[8]);

	float invdet = 1.0f / det;
	float Out[9] = { 0 };

	Out[0] = (matrix[5] * matrix[10] - matrix[9] * matrix[6]) * invdet;
	Out[1] = (matrix[2] * matrix[9] - matrix[1] * matrix[10]) * invdet;
	Out[2] = (matrix[1] * matrix[6] - matrix[2] * matrix[5]) * invdet;
	Out[3] = (matrix[6] * matrix[8] - matrix[4] * matrix[10]) * invdet;
	Out[4] = (matrix[0] * matrix[10] - matrix[2] * matrix[8]) * invdet;
	Out[5] = (matrix[4] * matrix[2] - matrix[0] * matrix[6]) * invdet;
	Out[6] = (matrix[4] * matrix[9] - matrix[8] * matrix[5]) * invdet;
	Out[7] = (matrix[8] * matrix[1] - matrix[0] * matrix[9]) * invdet;
	Out[8] = (matrix[0] * matrix[5] - matrix[4] * matrix[1]) * invdet;
	matrix[0] = Out[0];
	matrix[1] = Out[1];
	matrix[2] = Out[2];
	matrix[4] = Out[3];
	matrix[5] = Out[4];
	matrix[6] = Out[5];
	matrix[7] = Out[6];
	matrix[8] = Out[7];
	matrix[10] = Out[8];

}

void Matrix::Invers4x4()
{
	float inv[16], det;
	int i;

	inv[0] = matrix[5] * matrix[10] * matrix[15] - matrix[5] * matrix[11] * matrix[14] - matrix[9] * matrix[6] * matrix[15] + matrix[9] * matrix[7] * matrix[14] + matrix[13] * matrix[6] * matrix[11] - matrix[13] * matrix[7] * matrix[10];
	inv[4] = -matrix[4] * matrix[10] * matrix[15] + matrix[4] * matrix[11] * matrix[14] + matrix[8] * matrix[6] * matrix[15] - matrix[8] * matrix[7] * matrix[14] - matrix[12] * matrix[6] * matrix[11] + matrix[12] * matrix[7] * matrix[10];
	inv[8] = matrix[4] * matrix[9] * matrix[15] - matrix[4] * matrix[11] * matrix[13] - matrix[8] * matrix[5] * matrix[15] + matrix[8] * matrix[7] * matrix[13] + matrix[12] * matrix[5] * matrix[11] - matrix[12] * matrix[7] * matrix[9];
	inv[12] = -matrix[4] * matrix[9] * matrix[14] + matrix[4] * matrix[10] * matrix[13] + matrix[8] * matrix[5] * matrix[14] - matrix[8] * matrix[6] * matrix[13] - matrix[12] * matrix[5] * matrix[10] + matrix[12] * matrix[6] * matrix[9];
	inv[1] = -matrix[1] * matrix[10] * matrix[15] + matrix[1] * matrix[11] * matrix[14] + matrix[9] * matrix[2] * matrix[15] - matrix[9] * matrix[3] * matrix[14] - matrix[13] * matrix[2] * matrix[11] + matrix[13] * matrix[3] * matrix[10];
	inv[5] = matrix[0] * matrix[10] * matrix[15] - matrix[0] * matrix[11] * matrix[14] - matrix[8] * matrix[2] * matrix[15] + matrix[8] * matrix[3] * matrix[14] + matrix[12] * matrix[2] * matrix[11] - matrix[12] * matrix[3] * matrix[10];
	inv[9] = -matrix[0] * matrix[9] * matrix[15] + matrix[0] * matrix[11] * matrix[13] + matrix[8] * matrix[1] * matrix[15] - matrix[8] * matrix[3] * matrix[13] - matrix[12] * matrix[1] * matrix[11] + matrix[12] * matrix[3] * matrix[9];
	inv[13] = matrix[0] * matrix[9] * matrix[14] - matrix[0] * matrix[10] * matrix[13] - matrix[8] * matrix[1] * matrix[14] + matrix[8] * matrix[2] * matrix[13] + matrix[12] * matrix[1] * matrix[10] - matrix[12] * matrix[2] * matrix[9];
	inv[2] = matrix[1] * matrix[6] * matrix[15] - matrix[1] * matrix[7] * matrix[14] - matrix[5] * matrix[2] * matrix[15] + matrix[5] * matrix[3] * matrix[14] + matrix[13] * matrix[2] * matrix[7] - matrix[13] * matrix[3] * matrix[6];
	inv[6] = -matrix[0] * matrix[6] * matrix[15] + matrix[0] * matrix[7] * matrix[14] + matrix[4] * matrix[2] * matrix[15] - matrix[4] * matrix[3] * matrix[14] - matrix[12] * matrix[2] * matrix[7] + matrix[12] * matrix[3] * matrix[6];
	inv[10] = matrix[0] * matrix[5] * matrix[15] - matrix[0] * matrix[7] * matrix[13] - matrix[4] * matrix[1] * matrix[15] + matrix[4] * matrix[3] * matrix[13] + matrix[12] * matrix[1] * matrix[7] - matrix[12] * matrix[3] * matrix[5];
	inv[14] = -matrix[0] * matrix[5] * matrix[14] + matrix[0] * matrix[6] * matrix[13] + matrix[4] * matrix[1] * matrix[14] - matrix[4] * matrix[2] * matrix[13] - matrix[12] * matrix[1] * matrix[6] + matrix[12] * matrix[2] * matrix[5];
	inv[3] = -matrix[1] * matrix[6] * matrix[11] + matrix[1] * matrix[7] * matrix[10] + matrix[5] * matrix[2] * matrix[11] - matrix[5] * matrix[3] * matrix[10] - matrix[9] * matrix[2] * matrix[7] + matrix[9] * matrix[3] * matrix[6];
	inv[7] = matrix[0] * matrix[6] * matrix[11] - matrix[0] * matrix[7] * matrix[10] - matrix[4] * matrix[2] * matrix[11] + matrix[4] * matrix[3] * matrix[10] + matrix[8] * matrix[2] * matrix[7] - matrix[8] * matrix[3] * matrix[6];
	inv[11] = -matrix[0] * matrix[5] * matrix[11] + matrix[0] * matrix[7] * matrix[9] + matrix[4] * matrix[1] * matrix[11] - matrix[4] * matrix[3] * matrix[9] - matrix[8] * matrix[1] * matrix[7] + matrix[8] * matrix[3] * matrix[5];
	inv[15] = matrix[0] * matrix[5] * matrix[10] - matrix[0] * matrix[6] * matrix[9] - matrix[4] * matrix[1] * matrix[10] + matrix[4] * matrix[2] * matrix[9] + matrix[8] * matrix[1] * matrix[6] - matrix[8] * matrix[2] * matrix[5];

	det = matrix[0] * inv[0] + matrix[1] * inv[4] + matrix[2] * inv[8] + matrix[3] * inv[12];
	det = 1.0f / det;

	for (i = 0; i < 16; i++)
		matrix[i] = inv[i] * det;
}

void Matrix::Ymm(Vec3& temp)
{
	Vec3 aa;
	aa.x = temp.x * matrix[0] + temp.y * matrix[1] + temp.z * matrix[2] + matrix[3];
	aa.y = temp.x * matrix[4] + temp.y * matrix[5] + temp.z * matrix[6] + matrix[7];
	aa.z = temp.x * matrix[8] + temp.y * matrix[9] + temp.z * matrix[10] + matrix[11];
	temp = aa;
}

void Matrix::MatrixToQuaternion(Quaternion& quat, Vec3& move)
{
	Transponir();
	float T = matrix[0] + matrix[5] + matrix[10] + 1.0f;
	if (T > 0.0f)
	{
		float S = 0.5f / sqrt(T);
		float W = 0.25f / S;
		float X = (matrix[9] - matrix[6]) * S;
		float Y = (matrix[2] - matrix[8]) * S;
		float Z = (matrix[4] - matrix[1]) * S;
		quat.Set_Quaternion(X, Y, Z, W);
	}
	else
	{
		if (matrix[0] > matrix[5] && matrix[0] > matrix[10])
		{
			float S = sqrt(1.0f + matrix[0] - matrix[5] - matrix[10]) * 2.0f;
			float W = (matrix[6] + matrix[9]) / S;
			float X = 0.5f / S;
			float Y = (matrix[1] + matrix[4]) / S;
			float Z = (matrix[2] + matrix[8]) / S;
			quat.Set_Quaternion(X, Y, Z, W);
		}
		else if (matrix[5] > matrix[0] && matrix[5] > matrix[10])
		{
			float S = sqrt(1.0f + matrix[5] - matrix[0] - matrix[10]) * 2.0f;
			float W = (matrix[2] + matrix[8]) / S;
			float X = (matrix[1] + matrix[4]) / S;
			float Y = 0.5f / S;
			float Z = (matrix[2] + matrix[9]) / S;
			quat.Set_Quaternion(X, Y, Z, W);
		}
		else if (matrix[10] > matrix[0] && matrix[10] > matrix[5])
		{
			float S = sqrt(1.0f + matrix[10] - matrix[0] - matrix[5]) * 2.0f;
			float W = (matrix[1] + matrix[4]) / S;
			float X = (matrix[2] + matrix[8]) / S;
			float Y = (matrix[6] + matrix[9]) / S;
			float Z = 0.5f / S;
			quat.Set_Quaternion(X, Y, Z, W);
		}
	}
	Transponir();
	move.x = matrix[12];
	move.y = matrix[13];
	move.z = matrix[14];
	quat.Normalize();
}


//
//----------------Camera-----------------
//


Camera::Camera()
{
	glCreateBuffers(1, &Matrix_Buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, Matrix_Buffer);
	glNamedBufferData(Matrix_Buffer, 128, NULL, GL_DYNAMIC_DRAW);
}

void Camera::Set_Move(Vec3& temp)
{
	Quaternion quat(orientation);
	quat.Invers();
	quat.Rotate_Vec3(temp);
	move.x += temp.x;
	move.y += temp.y;
	move.z += temp.z;
	Camera_Act = true;
}

void Camera::Set_Projection_Matrix(const float& fov, const float& near, const float& far, const float& aspect)
{
	Matrix temp;
	temp.Set_Projection(fov, near, far, aspect);
	glNamedBufferSubData(Matrix_Buffer, 0, 64, temp.Get_link());
}

void Camera::Set_Free_Camera()
{
	if (Free_Camera)
	{
		Free_Camera = false;
		Set_Rotate_View_Reset();
	}
	else
		Free_Camera = true;
}

void Camera::Set_Rotate_View(const float& Z)
{
	if (Free_Camera)
	{
		Quaternion tempZ(0.0f, 0.0f, Z);
		orientation = tempZ * orientation;
		Camera_Act = true;
	}
}

void Camera::Set_Rotate_View(const float& X, const float& Y)
{
	if (Free_Camera)
	{
		Quaternion tempXY(X, Y, 0.0f);
		orientation = tempXY * orientation;
	}
	else
	{
		Cam_X += X;
		if (Cam_X > 89.0f)
			Cam_X = 89.0f;
		else if (Cam_X < -89.0f)
			Cam_X = -89.0f;
		else
		{
			Quaternion tempX(X, 0.0f, 0.0f);
			orientation = tempX * orientation;
		}
		Quaternion tempY(0.0f, Y, 0.0f);
		orientation *= tempY;

	}
	Camera_Act = true;
}

void Camera::Set_View_Matrix()
{
	if (Camera_Act)
	{
		Matrix Rotate(orientation);
		Rotate* move;
		glNamedBufferSubData(Matrix_Buffer, 64, 64, Rotate.Get_link());
		Camera_Act = false;
	}

	/*	—оздаетс€ матрица LookAt
		move - вли€ет на то, куда смотрим, а Zось на рассто€ние до объекта

		move.x += temp.x;
		move.y += temp.y;
		move.z += temp.z;
		Matrix Rotate(orientation);
		Matrix Position;
		Position* move;
		Matrix Transform = Position * Rotate;
		*/
}

void Camera::Set_Rotate_View_Reset()
{
	orientation = Quaternion();
	Cam_X = 0.0f;
	Camera_Act = true;
}