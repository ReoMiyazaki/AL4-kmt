#include "GameScene.h"

#include "Collision.h"
#include "Model.h"
#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteBG;
	delete objSkydome;
	delete objGround;
	delete objFighter;
	delete modelSkydome;
	delete modelGround;
	delete modelFighter;
	delete camera;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);

	this->dxCommon = dxCommon;
	this->input = input;

	// デバッグテキスト用テクスチャ読み込み
	Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	Sprite::LoadTexture(1, L"Resources/background.png");

	// カメラ生成
	camera = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight, input);

	// カメラ注視点をセット
	camera->SetTarget({ 0, 1, 0 });
	camera->SetDistance(3.0f);

	// 3Dオブジェクトにカメラをセット
	Object3d::SetCamera(camera);

	// 背景スプライト生成
	spriteBG = Sprite::Create(1, { 0.0f,0.0f });
	// 3Dオブジェクト生成
	objSkydome = Object3d::Create();
	objGround = Object3d::Create();
	objFighter = Object3d::Create();

	// テクスチャ2番に読み込み
	Sprite::LoadTexture(2, L"Resources/texture.png");

	modelSkydome = Model::CreateFromOBJ("skydome");
	modelGround = Model::CreateFromOBJ("ground");
	modelFighter = Model::CreateFromOBJ("chr_sword");

	objSkydome->SetModel(modelSkydome);
	objGround->SetModel(modelGround);
	objFighter->SetModel(modelFighter);

	// 当たり判定関係
	{
		// 弾の初期値を設定
		sphere.center = XMVectorSet(0, 2, 0, 1);	// 中心点座標
		sphere.radius = 1.0f;	// 半径
		// 平面の初期値を設定
		plane.normal = XMVectorSet(0, 2, 0, 1);	// 法線ベクトル
		plane.distance = 0.0f;	// 原点(0,0,0)からの距離
		//三角形の初期化を設定
		triangle.p0 = XMVectorSet(-1.0f, 0, -1.0f, 1);//左手前
		triangle.p1 = XMVectorSet(-1.0f, 0, +1.0f, 1);//右奥
		triangle.p2 = XMVectorSet(+1.0f, 0, -1.0f, 1);//右手前
		triangle.normal = XMVectorSet(0.0f, 1.0f, 0.0f, 1);//上向き
	}
}

void GameScene::Update()
{
	camera->Update();

	objSkydome->Update();
	objGround->Update();
	objFighter->Update();

	XMVECTOR moveX = XMVectorSet(0.01f, 0, 0, 0);
	XMVECTOR moveY = XMVectorSet(0, 0.01f, 0, 0);
	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// 移動後の座標を計算
		if (input->PushKey(DIK_RIGHT)) { sphere.center += moveX; }
		else if (input->PushKey(DIK_LEFT)) { sphere.center -= moveX; }
		if (input->PushKey(DIK_UP)) { sphere.center += moveY; }
		else if (input->PushKey(DIK_DOWN)) { sphere.center -= moveY; }
	}
	// stringstreamで変数の値を埋め込んで整形する
	std::ostringstream spherestr;
	spherestr << "Sphere:("
		<< std::fixed << std::setprecision(2)	// 小数点以下2桁まで
		<< sphere.center.m128_f32[0] << ","		// x
		<< sphere.center.m128_f32[1] << ","		// y
		<< sphere.center.m128_f32[2] << ")";	// z

	debugText.Print(spherestr.str(), 50, 180, 1.0f);

	XMVECTOR inter;
	bool hit1 = Collision::CheckSphere2Plane(sphere, plane, &inter);
	if (hit1)
	{
		debugText.Print("HIT", 50, 200, 1.0f);
		// stringstreamをリセットし、交点座標を埋め込む
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";

		//debugText.Print(spherestr.str(), 50, 220, 1.0f);
	}

	//球と三角形の当たり判定
	//XMVECTOR inter;
	bool hit2 = Collision::CheckSphere2Triangle(sphere, triangle, &inter);
	if (hit2)
	{
		//stringstreamをリセットし、交点座標を埋め込む
		std::ostringstream spherestr;
		spherestr.str("");
		spherestr.clear();
		spherestr << "("
			<< std::fixed << std::setprecision(2)
			<< inter.m128_f32[0] << ","
			<< inter.m128_f32[1] << ","
			<< inter.m128_f32[2] << ")";
		debugText.Print(spherestr.str(), 50, 220, 1.0f);
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// 背景スプライト描画
	//spriteBG->Draw();

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(cmdList);

	// 3Dオブクジェクトの描画
	objSkydome->Draw();
	objGround->Draw();
	objFighter->Draw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	//// 描画
	//sprite1->Draw();
	//sprite2->Draw();

	// デバッグテキストの描画
	debugText.DrawAll(cmdList);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}
