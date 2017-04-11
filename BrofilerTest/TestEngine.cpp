#include "Brofiler.h"
#include "Config.h"
#include "Platform/Platform.h"
#include "TestEngine.h"
#include <math.h>
#include <vector>
#include <cstdlib>

#ifdef BF_PLATFORM_WINDOWS
#pragma comment(lib, "winmm.lib")
#endif

namespace Test
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WorkerThread(void* _engine)
{
	Engine* engine = (Engine*)_engine;
	BROFILER_THREAD("Worker")
	
	while (engine->IsAlive())
	{
		// Emulate "wait for events" message
		Brofiler::Platform::Sleep(5);
		engine->UpdatePhysics();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const unsigned long REPEAT_COUNT = 128 * 1024;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<unsigned long N>
void SlowFunction()
{ PROFILE
	// Make it static to fool compiler and prevent it from skipping
	static float value = 0.0f;

	int T = rand() % 1000 + 127 * 1024;
	for (unsigned long i = 0; i < T; ++i)
		value = (value + sin((float)i)) * 0.5f;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SlowFunction2()
{ PROFILE
	// Make it static to fool compiler and prevent it from skipping
	static std::vector<float> values(1024 * 1024);

	int T = rand() % 1000 + 127 * 1024;
	values.reserve(T);

	for (size_t i = 1; i < values.size(); ++i)
	{
		values[i] += i;
		values[i] *= i;
		values[i] /= i;
		values[i] -= i;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Engine::Frame()
{
#ifdef BF_PLATFORM_WINDOWS
	DWORD t0 = timeGetTime();
#endif

	Update();
	Render();

#ifdef BF_PLATFORM_WINDOWS
	DWORD t = timeGetTime() - t0;
	BROFILER_COUNTER_SET(FrameTime, (double)(t));
#endif

	return true;
}

void Engine::Update()
{
	BROFILER_CATEGORY_WITH_MASK(BROFILER_MASK_UPDATE, "Update", Brofiler::Color::Chartreuse);

	UpdateInput();

	UpdateMessages();

	UpdateLogic();

	//UpdateTasks();

	UpdateScene();

	//UpdatePhysics();

#ifdef BF_PLATFORM_WINDOWS
	v_x += 1;
	v_y = sin(v_x);
	BROFILER_COUNTER_SET(SIN, v_y);
#endif

}

void Engine::Render()
{
	BROFILER_CATEGORY_WITH_MASK(BROFILER_MASK_RENDER, "Draw", Brofiler::Color::Beige);
	Draw();
}

#define BROFILER_MASK_UPDATE_INPUT (1<< (BROFILER_MASK_RESERVED_COUNT+1))
#define BROFILER_MASK_UPDATE_SCENE (1<< (BROFILER_MASK_RESERVED_COUNT+2))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::UpdateInput()
{
	BROFILER_CATEGORY_WITH_MASK(BROFILER_MASK_UPDATE | BROFILER_MASK_UPDATE_INPUT, "UpdateInput", Brofiler::Color::SteelBlue);
	SlowFunction2();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::UpdateMessages()
{
	BROFILER_CATEGORY("UpdateMessages", Brofiler::Color::Orange);
	SlowFunction<REPEAT_COUNT>();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::UpdateLogic()
{
	BROFILER_CATEGORY("UpdateLogic", Brofiler::Color::Orchid);
	SlowFunction<REPEAT_COUNT>();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::UpdateTasks()
{
	BROFILER_CATEGORY("UpdateTasks", Brofiler::Color::SkyBlue);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::UpdateScene()
{
	BROFILER_CATEGORY_WITH_MASK(BROFILER_MASK_UPDATE | BROFILER_MASK_UPDATE_SCENE, "UpdateScene", Brofiler::Color::SkyBlue);
	SlowFunction<REPEAT_COUNT>();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::Draw()
{
	BROFILER_CATEGORY("Draw", Brofiler::Color::Salmon);
	SlowFunction<REPEAT_COUNT>();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::UpdatePhysics()
{ 
	BROFILER_CATEGORY("UpdatePhysics", Brofiler::Color::Wheat);
	SlowFunction2();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Engine::Engine() : isAlive(true)
{
#ifdef BF_PLATFORM_WINDOWS
	timeBeginPeriod(1);
#endif

	for (size_t i = 0; i < WORKER_THREAD_COUNT; ++i)
	{
		workers[i] = new std::thread(std::bind(&WorkerThread, this));
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Engine::~Engine()
{
	isAlive = false;

	for (size_t i = 0; i < workers.size(); ++i)
	{
		workers[i]->join();
		delete workers[i];
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}