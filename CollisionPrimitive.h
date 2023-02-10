#pragma once
/// <summary>
/// 当たり判定プリミティブ
/// </summary>

#include<DirectXMath.h>

/// <summary>
/// 球
/// </summary>
struct Sphere
{
	DirectX::XMVECTOR center = { 0,0,0,1 };
	float radius = 1.0f;
};

/// <summary>
/// 平面
/// </summary>
struct Plane
{
	DirectX::XMVECTOR normal = { 0,1,0,0 };
	float distance = 0.0f;
};

/// <summary>
/// 法線付き三角形(時計回りが表面)
/// </summary>
class Triangle {
public:
	// 頂点座標3つ
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
	// 法線ベクトル
	DirectX::XMVECTOR normal;
};

