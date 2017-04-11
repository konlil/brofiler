#pragma once

#include <array>
#include <thread>

namespace Test
{
	// Test engine: emulates some hard CPU work.
	class Engine
	{
		static const size_t WORKER_THREAD_COUNT = 2;
		std::array<std::thread*, WORKER_THREAD_COUNT> workers;
		bool isAlive;
		double v_x;
		double v_y;

		void UpdateInput();
		void UpdateMessages();
		void UpdateLogic();
		void UpdateTasks();
		void UpdateScene();
		void Draw();

		void Render();
		void Update();
	public:
		Engine();
		~Engine();

		// Updates engine, should be called once per frame.
		// Returns false if it doesn't want to update any more.
		bool Frame();

		void UpdatePhysics();

		bool IsAlive() const { return isAlive; }
	};
}
