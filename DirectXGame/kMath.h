#pragma once

#include "AABB.h"
// #include "Matrix4x4.h"
#include <math\Matrix4x4.h>
#include <math\Vector3.h>
// #include "Vector3.h"
#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>
// ������Z�q�I�[�o�[���[�h
// Vector3�̑����Z
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhv);

KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

// ������Z�q�I�[�o�[���[�h
// Vector3�̊|���Z
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);

// 2�����Z�q�I�[�o�[���[�h
// Vector3�̊|���Z
const KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float s);

// ease In-Out
float easeInOut(float t, float x1, float x2);

// Lerp
float Lerp(float x1, float x2, float t);

KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);

// 1, x����]�s��
KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);

// 2, y����]�s��
KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);

// 3, z����]�s��
KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);

KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

// �R�����A�t�B���ϊ��s��
KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

// 3, ���W�ϊ�
KamataEngine::Vector3 MathTransform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);
