#include "kMath.h"
using namespace KamataEngine;

// Vector3�̑����Z
Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

// ������Z�q�I�[�o�[���[�h
// Vector3�̊|���Z
Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
};

// 2�����Z�q�I�[�o�[���[�h
// Vector3�̊|���Z
const Vector3 operator*(const Vector3& v, float s) {
	Vector3 temp(v);
	return temp *= s;
};

// ease In-Out
float easeInOut(float t, float x1, float x2) {
	float x;
	// easeOut
	float easedT = -(cos(float(M_PI * t)) - 1.0f) / 2.0f;

	x = (1.0f - easedT) * x1 + easedT * x2;
	return x;
};

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && // x��
	    (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && // y��
	    (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z))   // z��
	{
		return true;
	}
	return false;
}

//

float Lerp(float x1, float x2, float t) { return (1.0f - t) * x1 + t * x2; };

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t)); };

// 1, x����]�s��
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 ans = {0};

	ans.m[0][0] = 1;
	ans.m[1][1] = std::cos(radian);
	ans.m[1][2] = std::sin(radian);
	ans.m[2][1] = -std::sin(radian);
	ans.m[2][2] = std::cos(radian);
	ans.m[3][3] = 1;

	return ans;
};

// 2, y����]�s��
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 ans = {0};

	ans.m[0][0] = std::cos(radian);
	ans.m[0][2] = -std::sin(radian);
	ans.m[1][1] = 1;
	ans.m[2][0] = std::sin(radian);
	ans.m[2][2] = std::cos(radian);
	ans.m[3][3] = 1;

	return ans;
};

// 3, z����]�s��
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ans = {0};

	ans.m[0][0] = std::cos(radian);
	ans.m[0][1] = std::sin(radian);
	ans.m[1][0] = -std::sin(radian);
	ans.m[1][1] = std::cos(radian);
	ans.m[2][2] = 1;
	ans.m[3][3] = 1;

	return ans;
};

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 ans;
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			ans.m[a][b] = m1.m[a][0] * m2.m[0][b] + m1.m[a][1] * m2.m[1][b] + m1.m[a][2] * m2.m[2][b] + m1.m[a][3] * m2.m[3][b];
		}
	}
	return ans;
};

// �R�����A�t�B���ϊ��s��
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 S = {0};
	Matrix4x4 R = {0};
	Matrix4x4 rX = {0};
	Matrix4x4 rY = {0};
	Matrix4x4 rZ = {0};
	Matrix4x4 T = {0};
	Matrix4x4 ans = {0};

	S.m[0][0] = scale.x;
	S.m[1][1] = scale.y;
	S.m[2][2] = scale.z;
	S.m[3][3] = 1;

	rX = MakeRotateXMatrix(rotate.x);
	rY = MakeRotateYMatrix(rotate.y);
	rZ = MakeRotateZMatrix(rotate.z);

	R = Multiply(rX, Multiply(rY, rZ));

	T.m[0][0] = 1;
	T.m[1][1] = 1;
	T.m[2][2] = 1;
	T.m[3][3] = 1;
	T.m[3][0] = translate.x;
	T.m[3][1] = translate.y;
	T.m[3][2] = translate.z;

	ans.m[0][0] = S.m[0][0] * R.m[0][0];
	ans.m[0][1] = S.m[0][0] * R.m[0][1];
	ans.m[0][2] = S.m[0][0] * R.m[0][2];
	ans.m[1][0] = S.m[1][1] * R.m[1][0];
	ans.m[1][1] = S.m[1][1] * R.m[1][1];
	ans.m[1][2] = S.m[1][1] * R.m[1][2];
	ans.m[2][0] = S.m[2][2] * R.m[2][0];
	ans.m[2][1] = S.m[2][2] * R.m[2][1];
	ans.m[2][2] = S.m[2][2] * R.m[2][2];
	ans.m[3][0] = T.m[3][0];
	ans.m[3][1] = T.m[3][1];
	ans.m[3][2] = T.m[3][2];
	ans.m[3][3] = 1;

	return ans;
};

Vector3 MathTransform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 ans;

	ans.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	ans.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	ans.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	ans.x /= w;
	ans.y /= w;
	ans.z /= w;

	return ans;
}