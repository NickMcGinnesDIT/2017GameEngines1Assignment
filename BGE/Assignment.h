#pragma once
#include "Game.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"
#include <btBulletDynamicsCommon.h>

namespace BGE
{
	class Assignment :
		public Game
	{
	private:

	public:
		Assignment(void);
		~Assignment(void);
		bool Initialise();
		void Update(float timeDelta);
		void Cleanup();
		void CreateWall();
		//shared_ptr<PhysicsController> bird;
		//shared_ptr<PhysicsController> itsabox;
		shared_ptr<PhysicsController> CreateSeagull(glm::vec3 position, float scale = 5);
		shared_ptr<PhysicsController> CreateSeagull2(glm::vec3 position);
		shared_ptr<PhysicsController> Assignment::CreateAnimal(glm::vec3 pos, float scale);
		void Assignment::MoveAnimal();
	
	
	
	};
};