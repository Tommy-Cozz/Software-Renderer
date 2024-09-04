#include "MyGame.h"
#include <OGL3D/Game/OGame.h>

MyGame::MyGame()
{
}

MyGame::~MyGame()
{
}

void MyGame::onCreate()
{

	OGame::onCreate();
	
	m_entity = getEntitySystem()->createEntity<OEntity>();
}

void MyGame::onUpdate(f32 deltaTime)
{
	m_elapsedSeconds += deltaTime;

	if (m_entity && m_elapsedSeconds >= 3.0f)
	{
		m_entity->release();
		m_entity = nullptr;
	}


}
