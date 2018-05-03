#include "Engine.h"	
#include "core/Texture.h"
#include "SpriteBatch.h"
#include "CubeBatch.h"
#include "InputManager.h"
#include "Projects/TestSence.h"
#include "Projects/AlphaSence.h"
#include "Projects/Demo.h"
#include "Network/Message.h"
#include "LuaVM.h"

Engine::~Engine()
{

}


Engine::Engine()
: Singleton<Engine>()
{

}


void Engine::Init()
{
	
	LuaVM::GetInstance()->Init();
	// LuaVM::GetInstance()->doLuaString(R"(
    //               print("hello world");
    //               )" );
	
	//mSence = new TestNetwork();
	mSence = new AlphaSence();
	// mSence = new Demo();
    InputManager::GetInstance()->SetKeyCallback();
    InputManager::GetInstance()->SetScrollCallback();
    InputManager::GetInstance()->SetMouseCallback();
    InputManager::GetInstance()->SetMouseButtonCallback();
	TimerManager::GetInstance();


	LuaVM::GetInstance()->OnGameInit();
}

void Engine::Update(double dt)
{
    m_DeltaTime = dt;
	
	TimerManager::GetInstance()->Update();
    mSence->Update();
	LuaVM::GetInstance()->OnGameUpdate(dt);

	if (InputManager::GetInstance()->IsKeyUp(GLFW_KEY_F5) )
	{
		LuaVM::GetInstance()->CallSimpleFunc("GotoDebug");
	}
}

void Engine::Draw()
{ 
    mSence->Draw();
	LuaVM::GetInstance()->OnGameDraw();
}

void Engine::Destroy()
{
	TimerManager::GetInstance()->DeleteSingleton();
	InputManager::GetInstance()->DeleteSingleton();
}
void Engine::DispatchMove(MoveMessage msg)
{
	static_cast<Demo*>(mSence)->OnMove(msg);
}
void Engine::SetClient(Client* client)
{

	static_cast<Demo*>(mSence)->SetClient(client);
}
