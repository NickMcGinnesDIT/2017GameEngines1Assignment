#include "PhysicsGame1.h"
#include "PhysicsController.h"
#include "Sphere.h"
#include "PhysicsCamera.h"
#include "Box.h"
#include "Cylinder.h"
#include "Steerable3DController.h"
#include "Ground.h"
#include "Content.h"
#include <btBulletDynamicsCommon.h>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/norm.hpp>
#include "VectorDrawer.h"
#include "Utils.h"

#include "PhysicsFactory.h"
#include "Game.h" 
#include "Model.h"
#include "dirent.h"
#include "Capsule.h" 

#include "Assignment.h"

using namespace BGE;

Assignment::Assignment(void)
{
}

Assignment::~Assignment(void)
{
}


//animal links
btHingeConstraint * Body_ArmLeft;
btHingeConstraint * Body_ArmRight;
btHingeConstraint * Body_LegLeft;
btHingeConstraint * Body_LegRight;
shared_ptr<PhysicsController> body, armLeft, armRight, legLeft, legRight;


bool Assignment::Initialise()
{

	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();


	dynamicsWorld->setGravity(btVector3(0, -19, 0));

	/*
	int i = 0,
	j = 0;
	int k = 5,
	l = 5;

	shared_ptr<PhysicsController> box[5][5];


	for (j = 0; j < l; ++j) {
	for (i = 0; i < k; ++i) {
	box[i][j] = physicsFactory->CreateBox(5, 5, 5, glm::vec3(5 * i, 5 * j, 0), glm::quat());
	}
	}
	*/

	//shared_ptr<PhysicsController> bird = CreateSeagull(glm::vec3(-10, 30, 0), 5);
	shared_ptr<PhysicsController> itsabox = CreateAnimal(glm::vec3(0, 20, 0), 20);
	//CreateBird(glm::vec3(-10, 20, 0));


	if (!Game::Initialise()) {
		return false;
	}



	return true;
}

void BGE::Assignment::Update(float timeDelta)
{
	//cyl->rigidBody->applyTorque(GLToBtVector(glm::vec3(0.0f, 0.0f, 1.0f)));

	Game::Update(timeDelta);

	//itsabox->rigidBody->applyCentralForce(btVector3(0, 10, 0));
	MoveAnimal();
}

void BGE::Assignment::Cleanup()
{
	Game::Cleanup();
}


shared_ptr<PhysicsController> Assignment::CreateAnimal(glm::vec3 pos, float scale)
{
	float halfScale = scale / 2;
	glm::vec3 bodyPos = glm::vec3(pos.x, pos.y, pos.z);
	glm::vec3 armLeftPos = glm::vec3(pos.x + (scale + halfScale), pos.y, pos.z+12);
	glm::vec3 armRightPos = glm::vec3(pos.x - (scale + halfScale), pos.y, pos.z+12);
	glm::vec3 legLeftPos = glm::vec3(pos.x + (scale + halfScale), pos.y, pos.z - 12);
	glm::vec3 legRightPos = glm::vec3(pos.x - (scale + halfScale), pos.y, pos.z - 12);

	body = physicsFactory->CreateBox(scale, scale*0.15, scale*3.00, bodyPos, glm::quat(), false, true);
	armLeft = physicsFactory->CreateCylinder(halfScale, halfScale, armLeftPos, glm::angleAxis(270.0f, glm::vec3(0, 0, 1)), false, true);
	armRight = physicsFactory->CreateCylinder(halfScale, halfScale, armRightPos, glm::angleAxis(270.0f, glm::vec3(0, 0, 1)), false, true);
	legLeft = physicsFactory->CreateCylinder(halfScale, halfScale, legLeftPos, glm::angleAxis(270.0f, glm::vec3(0, 0, 1)), false, true);
	legRight = physicsFactory->CreateCylinder(halfScale, halfScale, legRightPos, glm::angleAxis(270.0f, glm::vec3(0, 0, 1)), false, true);

	
	btTransform bodyTransform, armLeftTransfrom, armRightTransform, legLeftTransform, legRightTransform;
	bodyTransform.setIdentity();
	bodyTransform.setOrigin(btVector3(bodyPos.x, bodyPos.y, bodyPos.z));


	//body to armLeft
	armLeftTransfrom.setIdentity();
	armLeftTransfrom.setOrigin(btVector3(armLeftPos.x, armLeftPos.y, armLeftPos.z));
	armLeftTransfrom.setRotation(GLToBtQuat(glm::angleAxis(270.0f,glm::vec3(0, 1, 0))));
	
	Body_ArmLeft = new btHingeConstraint(*armLeft->rigidBody, *body->rigidBody, bodyTransform, armLeftTransfrom);
	//btFixedConstraint * Body_ArmLeft = new btFixedConstraint(*armLeft->rigidBody, *body->rigidBody, bodyLeftTransform, armLeftTransfrom);
	
	dynamicsWorld->addConstraint(Body_ArmLeft);
	
	Body_ArmLeft->setAxis(btVector3(0, 1, 0));
	Body_ArmLeft->enableAngularMotor(true, 75, 50);
	
	//body to legLeft
	legLeftTransform.setIdentity();
	legLeftTransform.setOrigin(btVector3(legLeftPos.x, legLeftPos.y, legLeftPos.z));
	legLeftTransform.setRotation(GLToBtQuat(glm::angleAxis(270.0f, glm::vec3(0, 1, 0))));

	Body_LegLeft = new btHingeConstraint(*legLeft->rigidBody, *body->rigidBody, bodyTransform, legLeftTransform);
	dynamicsWorld->addConstraint(Body_LegLeft);
	Body_LegLeft->setAxis(btVector3(0, 1, 0));

	//body to armRight
	armRightTransform.setIdentity();
	armRightTransform.setOrigin(btVector3(armRightPos.x, armRightPos.y, armRightPos.z));
	armRightTransform.setRotation(GLToBtQuat(glm::angleAxis(270.0f, glm::vec3(0, 1, 0))));

	Body_ArmRight = new btHingeConstraint(*armRight->rigidBody, *body->rigidBody, bodyTransform, armRightTransform);
	//btFixedConstraint * Body_ArmRight = new btFixedConstraint(*body->rigidBody, *armRight->rigidBody, bodyRightTransform, armRightTransform);
	dynamicsWorld->addConstraint(Body_ArmRight);
	Body_ArmRight->setAxis(btVector3(0, 1, 0));
	Body_ArmRight->enableAngularMotor(true, 75, 50);

	//body to legRight
	legRightTransform.setIdentity();
	legRightTransform.setOrigin(btVector3(legRightPos.x, legRightPos.y, legRightPos.z));
	legRightTransform.setRotation(GLToBtQuat(glm::angleAxis(270.0f, glm::vec3(0, 1, 0))));

	Body_LegRight = new btHingeConstraint(*legRight->rigidBody, *body->rigidBody, bodyTransform, legRightTransform);
	dynamicsWorld->addConstraint(Body_LegRight);
	Body_LegRight->setAxis(btVector3(0, 1, 0));

	return body;
}

void Assignment::MoveAnimal()
{
	
}

shared_ptr<PhysicsController> Assignment::CreateSeagull(glm::vec3 position, float scale)
{

	//	float scale = 5;
	float head_rad = scale / 1.5;
	shared_ptr<PhysicsController> head = physicsFactory->CreateSphere(head_rad, position + glm::vec3(0, scale + head_rad, scale), glm::quat());
	shared_ptr<PhysicsController> body = physicsFactory->CreateSphere(scale, position, glm::quat());
	//btHingeConstraint * head_body = new btHingeConstraint(*head->rigidBody, *body->rigidBody, GLToBtVector(glm::vec3(0, -(head_rad), -(head_rad))), GLToBtVector(glm::vec3(0, scale - 2, scale - 2)), btVector3(1, 0, 0), btVector3(1, 0, 0));	//	body hing bit thing is scale - 2 because head position and scale
	btTransform t1, t2;
	t1.setIdentity();
	t2.setIdentity();
	t1.setOrigin(btVector3(0, -(head_rad), -(head_rad)));
	t2.setOrigin(btVector3(0, scale - 2, scale - 2));
	btFixedConstraint * head_body = new btFixedConstraint(*head->rigidBody, *body->rigidBody, t1, t2);
	dynamicsWorld->addConstraint(head_body);

	/*
	**	wings
	*/

	float shoulder_rad = glm::abs((glm::sqrt(2 * (scale * scale)) - scale) / 2);

	glm::vec3 left_wing_hinge = position + glm::vec3(((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0);
	glm::vec3 right_wing_hinge = position + glm::vec3(-((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0);

	//shared_ptr<PhysicsController> left_shoulder = physicsFactory->CreateBox(scale * 2, scale / 5, scale, left_wing_hinge + glm::vec3((scale + shoulder_rad), 0, 0), glm::quat());

	glm::quat shoulder_rot = glm::angleAxis(90.0F, glm::vec3(1, 0, 0));
	shared_ptr<PhysicsController> left_shoulder = physicsFactory->CreateCylinder(shoulder_rad - 0.5, scale / 5, left_wing_hinge, shoulder_rot);
	btHingeConstraint * body_l_shoulder = new btHingeConstraint(*body->rigidBody, *left_shoulder->rigidBody, GLToBtVector(glm::vec3(((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3(0, 0, 0)), btVector3(0, 0, 0), btVector3(0, 1, 0));
	Game::dynamicsWorld->addConstraint(body_l_shoulder);
	body_l_shoulder->enableAngularMotor(true, 20, 20);

	/*
	shared_ptr<PhysicsController> left_wing_stick = physicsFactory->CreateBox(scale, scale / 10, scale / 10, left_wing_hinge + glm::vec3(scale / 2, 0, scale / 5), glm::quat());
	btHingeConstraint * l_shoulder_stick = new btHingeConstraint(*left_shoulder->rigidBody, *left_wing_stick->rigidBody, btVector3(shoulder_rad, 0, 0), btVector3(-(scale - 0.5), 0, 0), btVector3(0, 1, 0), btVector3(0, 0, 1));
	Game::dynamicsWorld->addConstraint(l_shoulder_stick);
	*/

	/*
	shared_ptr<PhysicsController> left_wing_stable1 = physicsFactory->CreateBox(scale / 10, scale / 10, scale / 5, left_wing_hinge + glm::vec3(scale / 2, -(scale / 5), scale / 5), glm::quat());
	shared_ptr<PhysicsController> left_wing_stable2 = physicsFactory->CreateBox(scale / 10, scale / 10, scale / 5, left_wing_hinge + glm::vec3(scale / 2, -(scale / 5), scale / 5), glm::quat());
	btTransform t7, t8;
	t7.setIdentity();
	t8.setIdentity();
	t7.setOrigin(GLToBtVector(left_wing_hinge + glm::vec3(scale / 1.5, (scale / 5), 0) - position));
	//t7.setOrigin(GLToBtVector(left_wing_hinge - position));
	t8.setOrigin(btVector3(0, 0, 0));
	btFixedConstraint * left_wing_stable1_h = new btFixedConstraint(*body->rigidBody, *left_wing_stable1->rigidBody, t7, t8);
	dynamicsWorld->addConstraint(left_wing_stable1_h);

	btTransform t9, t10;
	t9.setIdentity();
	t10.setIdentity();
	t9.setOrigin(GLToBtVector(left_wing_hinge + glm::vec3(scale / 1.5, -(scale / 5), 0) - position));
	t10.setOrigin(btVector3(0, 0, 0));
	btFixedConstraint * left_wing_stable2_h = new btFixedConstraint(*body->rigidBody, *left_wing_stable2->rigidBody, t9, t10);
	dynamicsWorld->addConstraint(left_wing_stable2_h);
	*/


	//shared_ptr<PhysicsController> left_wing_stick = physicsFactory->CreateBox(scale * 2, scale / 10, scale / 10, left_wing_hinge + glm::vec3((scale) + 2, 0, scale / 5), glm::quat());
	shared_ptr<PhysicsController> left_wing_stick = physicsFactory->CreateBox(scale * 2, scale / 10, scale, left_wing_hinge + glm::vec3((scale)+2, 0, scale / 5), glm::quat());
	btHingeConstraint * l_shoulder_stick = new btHingeConstraint(*left_shoulder->rigidBody, *left_wing_stick->rigidBody, btVector3(shoulder_rad, 0, 0), btVector3(-((scale * 2) - shoulder_rad), 0, 0), btVector3(0, 1, 0), btVector3(0, 0, 1));
	Game::dynamicsWorld->addConstraint(l_shoulder_stick);

	/*
	btTransform t13, t14;
	t13.setIdentity();
	t14.setIdentity();
	t13.setOrigin(GLToBtVector(left_wing_hinge - position + glm::vec3((scale) + 2, 0, 0)));
	t14.setOrigin(GLToBtVector(glm::vec3( -((scale / 2) + 2), 0, 0)));
	//btFixedConstraint * left_stick_stable = new btFixedConstraint(*body->rigidBody, *left_wing_stick->rigidBody, t13, t14);
	btHingeConstraint * left_stick_stable = new btHingeConstraint(*body->rigidBody, *left_wing_stick->rigidBody, GLToBtVector(left_wing_hinge - position + glm::vec3((scale)+2, 0, 0)), GLToBtVector(glm::vec3(-((scale / 2) + 2), 0, 0)), btVector3(0, 0, 1), btVector3(0, 0, 1));
	dynamicsWorld->addConstraint(left_stick_stable);
	*/


	/*
	shared_ptr<PhysicsController> left_wing = physicsFactory->CreateBox(scale * 2, scale / 5, scale, left_wing_hinge + glm::vec3((scale + shoulder_rad), 0, 0) + glm::vec3((scale / 2) + 2, 0, scale / 5), glm::quat());

	btTransform t11, t12;
	t11.setIdentity();
	t12.setIdentity();
	t11.setOrigin(GLToBtVector(left_wing_hinge + glm::vec3((scale) + 2, 0, 0)));
	//t12.setOrigin(GLToBtVector(left_wing_hinge + glm::vec3(((scale)+shoulder_rad), 0, 0) - position));
	t12.setOrigin(GLToBtVector(left_wing_hinge + glm::vec3((scale * 2 ) + 2 + shoulder_rad, 0, 0)));
	btFixedConstraint * left_stick_wing = new btFixedConstraint(*left_wing_stick->rigidBody, *left_wing->rigidBody, t11, t12);
	dynamicsWorld->addConstraint(left_stick_wing);
	*/

	//shared_ptr<PhysicsController> right_shoulder = physicsFactory->CreateBox(scale * 2, scale / 5, scale, right_wing_hinge + glm::vec3(-(scale + shoulder_rad), 0, 0), glm::quat());


	shared_ptr<PhysicsController> right_shoulder = physicsFactory->CreateCylinder(shoulder_rad - 0.5, scale / 5, right_wing_hinge, shoulder_rot);
	btHingeConstraint * body_r_shoulder = new btHingeConstraint(*body->rigidBody, *right_shoulder->rigidBody, GLToBtVector(glm::vec3(-((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3(0, 0, 0)), btVector3(0, 0, 0), btVector3(0, 1, 0));
	Game::dynamicsWorld->addConstraint(body_r_shoulder);
	body_r_shoulder->enableAngularMotor(true, -20, 20);

	shared_ptr<PhysicsController> right_wing_stick = physicsFactory->CreateBox(scale * 2, scale / 10, scale, right_wing_hinge + glm::vec3(-(scale / 2), 0, scale / 5), glm::quat());
	btHingeConstraint * r_shoulder_stick = new btHingeConstraint(*right_shoulder->rigidBody, *right_wing_stick->rigidBody, btVector3(-shoulder_rad, 0, 0), btVector3((scale * 2) - shoulder_rad, 0, 0), btVector3(0, 1, 0), btVector3(0, 0, 1));
	Game::dynamicsWorld->addConstraint(r_shoulder_stick);

	/*
	shared_ptr<PhysicsController> left_wing = physicsFactory->CreateBox(scale * 2, scale / 5, scale, left_wing_hinge + glm::vec3((scale + shoulder_rad), 0, 0), glm::quat());
	shared_ptr<PhysicsController> right_wing = physicsFactory->CreateBox(scale * 2, scale / 5, scale, right_wing_hinge + glm::vec3(-(scale + shoulder_rad), 0, 0), glm::quat());
	btHingeConstraint * body_l_wing = new btHingeConstraint(*body->rigidBody, *left_wing->rigidBody, GLToBtVector(glm::vec3(((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3(-(scale + shoulder_rad), 0, 0)), btVector3(0, 0, 1), btVector3(0, 0, 1));
	btHingeConstraint * body_r_wing = new btHingeConstraint(*body->rigidBody, *right_wing->rigidBody, GLToBtVector(glm::vec3(-((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3((scale + shoulder_rad), 0, 0)), btVector3(0, 0, 1), btVector3(0, 0, 1));

	//body_l_wing->enableAngularMotor(true, -5, 5);
	//body_r_wing->enableAngularMotor(true, 5, 5);
	//body_l_wing->;

	Game::dynamicsWorld->addConstraint(body_l_wing);
	Game::dynamicsWorld->addConstraint(body_r_wing);
	*/


	/*
	**	legs
	*/

	float hip_rad = ((scale + (scale / 2) - (head_rad / 2)) - (scale * (glm::sqrt(3.0) / 2))) / 2;
	shared_ptr<PhysicsController> left_leg = physicsFactory->CreateCylinder((scale / 10), head_rad, position + glm::vec3((scale / 2), -(scale + (scale / 2.5)), 0), glm::quat());
	shared_ptr<PhysicsController> right_leg = physicsFactory->CreateCylinder((scale / 10), head_rad, position + glm::vec3(-(scale / 2), -(scale + (scale / 2.5)), 0), glm::quat());
	/*
	btHingeConstraint * body_l_leg = new btHingeConstraint(*body->rigidBody, *left_leg->rigidBody, btVector3((scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0), btVector3(0, (head_rad / 2) + hip_rad, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
	btHingeConstraint * body_r_leg = new btHingeConstraint(*body->rigidBody, *right_leg->rigidBody, btVector3(-(scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0), btVector3(0, (head_rad / 2) + hip_rad, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
	dynamicsWorld->addConstraint(body_l_leg);
	dynamicsWorld->addConstraint(body_r_leg);
	*/
	btTransform t3, t4;
	t3.setIdentity();
	t4.setIdentity();
	t3.setOrigin(btVector3((scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0));
	t4.setOrigin(btVector3(0, (head_rad / 2) + hip_rad, 0));
	btFixedConstraint * body_l_leg = new btFixedConstraint(*body->rigidBody, *left_leg->rigidBody, t3, t4);
	dynamicsWorld->addConstraint(body_l_leg);

	btTransform t5, t6;
	t5.setIdentity();
	t6.setIdentity();
	t5.setOrigin(btVector3(-(scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0));
	t6.setOrigin(btVector3(0, (head_rad / 2) + hip_rad, 0));
	btFixedConstraint * body_r_leg = new btFixedConstraint(*body->rigidBody, *right_leg->rigidBody, t5, t6);
	dynamicsWorld->addConstraint(body_r_leg);



	return body;
}

shared_ptr<PhysicsController> Assignment::CreateSeagull2(glm::vec3 position)
{

	float scale = 5;
	float head_rad = scale / 1.5;
	shared_ptr<PhysicsController> head = physicsFactory->CreateSphere(head_rad, position + glm::vec3(0, scale + head_rad, scale), glm::quat());
	shared_ptr<PhysicsController> body = physicsFactory->CreateSphere(scale, position, glm::quat());
	//btHingeConstraint * head_body = new btHingeConstraint(*head->rigidBody, *body->rigidBody, GLToBtVector(glm::vec3(0, -(head_rad), -(head_rad))), GLToBtVector(glm::vec3(0, scale - 2, scale - 2)), btVector3(1, 0, 0), btVector3(1, 0, 0));	//	body hing bit thing is scale - 2 because head position and scale
	btTransform t1, t2;
	t1.setIdentity();
	t2.setIdentity();
	t1.setOrigin(btVector3(0, -(head_rad), -(head_rad)));
	t2.setOrigin(btVector3(0, scale - 2, scale - 2));
	btFixedConstraint * head_body = new btFixedConstraint(*head->rigidBody, *body->rigidBody, t1, t2);
	dynamicsWorld->addConstraint(head_body);

	/*
	**	wings
	*/

	float shoulder_rad = glm::abs((glm::sqrt(2 * (scale * scale)) - scale) / 2);

	glm::vec3 left_wing_hinge = position + glm::vec3(((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0);
	glm::vec3 right_wing_hinge = position + glm::vec3(-((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0);

	//shared_ptr<PhysicsController> left_shoulder = physicsFactory->CreateBox(scale * 2, scale / 5, scale, left_wing_hinge + glm::vec3((scale + shoulder_rad), 0, 0), glm::quat());

	//glm::quat shoulder_rot = glm::angleAxis(90.0F, glm::vec3(1, 0, 0));
	//shared_ptr<PhysicsController> left_shoulder = physicsFactory->CreateCylinder(shoulder_rad - 0.2, scale / 5, left_wing_hinge, shoulder_rot);
	//btHingeConstraint * body_l_shoulder = new btHingeConstraint(*body->rigidBody, *left_wing->rigidBody, GLToBtVector(glm::vec3(((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3(-(scale + shoulder_rad), 0, 0)), btVector3(0, 0, 1), btVector3(0, 0, 1));
	//Game::dynamicsWorld->addConstraint(body_l_shoulder);

	//shared_ptr<PhysicsController> right_shoulder = physicsFactory->CreateBox(scale * 2, scale / 5, scale, right_wing_hinge + glm::vec3(-(scale + shoulder_rad), 0, 0), glm::quat());


	shared_ptr<PhysicsController> left_wing = physicsFactory->CreateBox(scale * 2, scale / 5, scale, left_wing_hinge + glm::vec3((scale + shoulder_rad), 0, 0), glm::quat());
	shared_ptr<PhysicsController> right_wing = physicsFactory->CreateBox(scale * 2, scale / 5, scale, right_wing_hinge + glm::vec3(-(scale + shoulder_rad), 0, 0), glm::quat());
	btHingeConstraint * body_l_wing = new btHingeConstraint(*body->rigidBody, *left_wing->rigidBody, GLToBtVector(glm::vec3(((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3(-(scale + shoulder_rad), 0, 0)), btVector3(0, 0, 1), btVector3(0, 0, 1));
	btHingeConstraint * body_r_wing = new btHingeConstraint(*body->rigidBody, *right_wing->rigidBody, GLToBtVector(glm::vec3(-((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), ((scale + shoulder_rad) * (1 / glm::sqrt(2.0))), 0)), GLToBtVector(glm::vec3((scale + shoulder_rad), 0, 0)), btVector3(0, 0, 1), btVector3(0, 0, 1));

	body_l_wing->enableAngularMotor(true, -5, 5);
	body_r_wing->enableAngularMotor(true, 5, 5);
	//body_l_wing->;

	Game::dynamicsWorld->addConstraint(body_l_wing);
	Game::dynamicsWorld->addConstraint(body_r_wing);



	/*
	**	legs
	*/

	float hip_rad = ((scale + (scale / 2) - (head_rad / 2)) - (scale * (glm::sqrt(3.0) / 2))) / 2;
	shared_ptr<PhysicsController> left_leg = physicsFactory->CreateCylinder((scale / 10), head_rad, position + glm::vec3((scale / 2), -(scale + (scale / 2.5)), 0), glm::quat());
	shared_ptr<PhysicsController> right_leg = physicsFactory->CreateCylinder((scale / 10), head_rad, position + glm::vec3(-(scale / 2), -(scale + (scale / 2.5)), 0), glm::quat());
	/*
	btHingeConstraint * body_l_leg = new btHingeConstraint(*body->rigidBody, *left_leg->rigidBody, btVector3((scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0), btVector3(0, (head_rad / 2) + hip_rad, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
	btHingeConstraint * body_r_leg = new btHingeConstraint(*body->rigidBody, *right_leg->rigidBody, btVector3(-(scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0), btVector3(0, (head_rad / 2) + hip_rad, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
	dynamicsWorld->addConstraint(body_l_leg);
	dynamicsWorld->addConstraint(body_r_leg);
	*/
	btTransform t3, t4;
	t3.setIdentity();
	t4.setIdentity();
	t3.setOrigin(btVector3((scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0));
	t4.setOrigin(btVector3(0, (head_rad / 2) + hip_rad, 0));
	btFixedConstraint * body_l_leg = new btFixedConstraint(*body->rigidBody, *left_leg->rigidBody, t3, t4);
	dynamicsWorld->addConstraint(body_l_leg);

	btTransform t5, t6;
	t5.setIdentity();
	t6.setIdentity();
	t5.setOrigin(btVector3(-(scale / 2), -((scale * (glm::sqrt(3.0) / 2)) + hip_rad), 0));
	t6.setOrigin(btVector3(0, (head_rad / 2) + hip_rad, 0));
	btFixedConstraint * body_r_leg = new btFixedConstraint(*body->rigidBody, *right_leg->rigidBody, t5, t6);
	dynamicsWorld->addConstraint(body_r_leg);



	return body;
}