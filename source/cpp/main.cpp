/**
* @file main.cpp
* @brief NiceShot(3D)戦車ゲーム
* @author キムラジュン
*/
#include "../h/main.h"
#include "../h/Debug/debugproc.h"
#include "../h/Other/input.h"
#include "../h/Object/Camera/camera.h"
#include "../h/Object/Player/player.h"
#include "../h/Map/field.h"
#include "../h/Map/wall.h"
#include "../h/Map/sky.h"
#include "../h/Object/Shadow/shadow.h"
#include "../h/Object/Title/title.h"
#include "../h/Object/Result/result.h"
#include "../h/Object/Scene/Scene.h"
#include "../h/Object/Tutorial/tutorial.h"
#include "../h/Effect/effect.h"
#include "../h/Object/Bullet/bullet.h"
#include "../h/Effect/explosion.h"
#include "../h/Object/Rank/rank.h"
#include "../h/Object/CountDown/countdown.h"
#include "../h/Object/Item/item.h"
#include "../h/Effect/damege.h"
#include "../h/Object/Player/Status/status.h"
#include "../h/Other/sound.h"
#include "../h/Object/Bullet/bulletprediction.h"
#include "../h/Object/Bullet/bulletgauge.h"
#include "../h/Object/Player/VitalGauge/vitalgauge.h"
#include "../h/Object/ObjectClass/Instance/instance.h"
#include "../h/net/sock.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			//!< ウインドウのクラス名
#define WINDOW_NAME		_T("NiceShot！")	//!< ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
* @brief 初期化関数
* @param[in]　HWND hWnd, BOOL bWindow
* @return HRESULT
* @details 起動時の初期化
*/
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);

/**
* @brief メモリ開放関数
* @details ゲーム終了時にメモリ開放する
*/
void Uninit(void);

#ifdef _DEBUG
/**
* @brief FPS表示関数
* @details デバッグ実行時FPSを表示する
*/
void DrawFPS(void);
#endif


//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;				//!< Direct3D オブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;		//!< Deviceオブジェクト(描画に必要)
HWND				edit;						//!< ハンドル
MSG					msg;						//!< アプリピークメッセージ
bool				EndGame = false;			//!< ゲーム終了フラグ

#ifdef _DEBUG
static LPD3DXFONT	g_pD3DXFont = NULL;			//!< フォントへのポインタ
char				g_text[256] = { 0 };		//!< 表示させるテキスト
int					g_nCountFPS;				//!< FPSカウンタ
char				g_textSo[256] = { "aa" };	//!< 表示させるテキスト
DWORD				dwFrameCount;				//!< 時間計測用
#endif

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned)time(NULL));
	
	//UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	//UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND hWnd;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_W + GetSystemMetrics(SM_CXDLGFRAME) * 2,
		SCREEN_H + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL,
		NULL,
		hInstance,
		NULL);
	
	// DirectXの初期化(ウィンドウを作成してから行う)
	bool mode;
	//戻り値がint型で戻ってくる
	//ここのMessageBoxを呼ぶとwindowが出せる。
	//引数1はNULL。引数2は本文。
	//引数3はwindou名。
	//引数4はMB_YESNOCANCELこれでイエスノーキャンセルの三択が出せる。ここはいろんな種類があるから自分で調べるMB_OKでOKだけ
	//引数4の後ろに「| MB_ICONQUESTION」でアイコンを出すことができる。
	int id = MessageBox(NULL, "Windowモードでプレイしますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);

	//押された選択肢でスイッチ分岐
	switch (id)
	{
	case IDYES:		// YesならWindowモードで起動
		mode = true;
		break;
	case IDNO:		// Noならフルスクリーンモードで起動
		mode = false;
		break;
	case IDCANCEL:	// CANCELなら終了
	default:
		return -1;
		break;
	}

	//注意　メインで書き込む変数とマルチ変数　ぶつかるとやばい　資源の共有　タイミングぶつからないようにする
	//ざっくりとした関数一つでいいから作る　書き換える可能性のある時にアクセス許可をとる
	//同期化する変数だけ信号を待つ(ざっくりした関数のアクセス許可)　シンクロライズ?　

	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	//-----------------------------------------------------オブジェクト生成
	GAME_OBJECT_INSTANCE ObjectInstance;
	ObjectInstance.Generate();

	//-----------------------------------------------------オブジェクトアドレス取得
	ObjectInstance.Addressor(&ObjectInstance);

	//-----------------------------------------------------オブジェクト初期化
	ObjectInstance.Init();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	//マルチスレッド開始
	std::thread SubThread([&ObjectInstance]() {ObjectInstance.MySubThreadFunc(); });
	
	//フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime =
		dwFPSLastTime = timeGetTime();
	dwCurrentTime =
		dwFrameCount = 0;


	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	PlaySound(SOUND_LABEL_BGM_title01);

	// メッセージループ
	while (1)
	{
		if (msg.message == WM_QUIT)
		{// PostQuitMessage()が呼ばれたらループ終了
			break;
		}
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (EndGame == true)
			{
				break;
			}
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行
			{
#ifdef _DEBUG
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
#ifdef _DEBUG
				// FPS表示
				PrintDebugProc("FPS: %d\n", g_nCountFPS);
#endif
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				ObjectInstance.Update();

				// 描画処理
				ObjectInstance.Draw();

				dwFrameCount++;
			}
		}
	}

	//-----------------------------------------------------オブジェクト終了
	//シーケンス終了フラグ
	EndGame = true;

	ObjectInstance.GetMySocket()->MultThreadFlagFunc(false);
	ObjectInstance.GetMySocket()->GameSceneFlagFunc(false);

	//スレッド破棄
	SubThread.join();

	//delete
	ObjectInstance.Delete();

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();


	timeEndPeriod(1);				// 分解能を戻す

	return (int)msg.wParam;

}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			//DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.BackBufferWidth = SCREEN_W;							// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_H;							// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファフォーマットはディスプレイモードに合わせて使う
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// 映像信号に同期してフリップする
	d3dpp.Windowed = bWindow;									// ウィンドウモード
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使う

	if (bWindow)
	{// ウィンドウモード
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;							// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = 0;								// リフレッシュレート
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;			// インターバル
	}
	else
	{// フルスクリーンモード

		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;							// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;			// リフレッシュレート
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;			// インターバル

	}

	// デバイスオブジェクトの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp, &g_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&d3dpp, &g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&d3dpp, &g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートパラメータの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

#ifdef _DEBUG
// 情報表示用フォントを設定
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Terminal"), &g_pD3DXFont);
#endif

	// FOGの設定
	FLOAT StartPos = 50;  //開始位置
	FLOAT EndPos = 300; //終了位置
	g_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);								//フォグ：OFF
	g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));	//カラー
	g_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);						//頂点モード
	g_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);					//テーブルモード
	g_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&StartPos));					//開始位置
	g_pD3DDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&EndPos));						//終了位置	

	//初期化処理
	InitSound(hWnd);
	InitDebugProc();
	InitCamera();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	if (g_pD3DDevice != NULL)
	{// デバイスの開放
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if (g_pD3D != NULL)
	{// Direct3Dオブジェクトの開放
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

//=============================================================================
// デバイスの取得
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

#ifdef _DEBUG
//=============================================================================
// デバッグ時にどの制御になったかゲーム画面で判断する文字描画関数
//=============================================================================
void DrawTextType(void)
{
	TCHAR str[256];
	RECT rect = { 10, 90, SCREEN_W, SCREEN_H };
	wsprintf(str, _T("%s\n"), g_text);
	g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0x00, 0xff));
}

//=============================================================================
// どの制御になったかゲーム画面で判断する文字描画関数
//=============================================================================
void DrawTextTypeSo(void)
{
	TCHAR str[256];
	RECT rect = { 10, 90, SCREEN_W, SCREEN_H };
	wsprintf(str, _T("%s\n"), g_textSo);
	g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0x00, 0xff));
}

//=============================================================================
// 表示させる文字列をg_textに書き込む
//=============================================================================
void SetTextSo(char *moji)
{
	strcpy(g_textSo, moji);
}

//=============================================================================
// デバッグ時に表示させる文字列をg_textに書き込む
//=============================================================================
void SetText(char *moji)
{
	strcpy(g_text, moji);
}

//=============================================================================
// FPS表示処理
//=============================================================================
void DrawFPS(void)
{
	TCHAR str[256];

	//ここのセットを加えることでデバッグ時の表示項目を増やせる
	{
		RECT rect = { 0, 0, SCREEN_W, SCREEN_H };
		wsprintf(str, _T("FPS:%d\n"), g_nCountFPS);
		// テキスト描画
		g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));//ARGBで色変えれる
	}

	{
		RECT rect = { 0, 50, SCREEN_W, SCREEN_H };
		wsprintf(str, _T("%d %d %d %d %d %d %d %d \n"), GetPadData(PLAYER01,0), GetPadData(PLAYER01, 1), GetPadData(PLAYER01, 2), GetPadData(PLAYER01, 3), 
			GetPadData(PLAYER01, 4), GetPadData(PLAYER01, 5), GetPadData(PLAYER01, 6), GetPadData(PLAYER01, 7));
		// テキスト描画
		g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0x00, 0x00));
	}

	{
		RECT rect = { 0, 70, SCREEN_W, SCREEN_H };
		wsprintf(str, _T("%d %d %d %d %d %d %d %d \n"), GetPadData(PLAYER01, 8), GetPadData(PLAYER01, 9), GetPadData(PLAYER01, 10), GetPadData(PLAYER01, 11), 
			GetPadData(PLAYER01, 12), GetPadData(PLAYER01, 13), GetPadData(PLAYER01, 14), GetPadData(PLAYER01, 15));
		// テキスト描画
		g_pD3DXFont->DrawText(NULL, str, -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0x00, 0x00));
	}

}
#endif

//=============================================================================
// 自作乱数　線形合同法
//=============================================================================
int MyRandFunc(int *X, int Max)
{
	//Xのバッファ
	int buff = *X;

	//任意数AB
	int A = 1664525;
	int B = 1013904223;
	int M = 2147483647;
	//線形合同法で計算　結果を次のシード値とする
	int ret = abs((A*buff + B) % M);
	*X = ret;
	//中途半端な数のあまりを使用する　%(Max)は上限値でもある
	ret = abs(ret % Max);
	return ret;
}

//=============================================================================
// ピークメッセージのセット 終了するときにQuitを代入する
//=============================================================================
void SetMsg(UINT Setmsg)
{
	msg.message = Setmsg;
}

//=============================================================================
// ピークメッセージのセット 終了するときにQuitを代入する
//=============================================================================
MSG GetMsg(void)
{
	return msg;
}

//=============================================================================
// ゲーム終了フラグセッター
//=============================================================================
void SetEndGame(bool flag)
{
	EndGame = flag;
}

//=============================================================================
// ゲーム終了フラグゲッター
//=============================================================================
bool GetEndGame(void)
{
	return EndGame;
}
