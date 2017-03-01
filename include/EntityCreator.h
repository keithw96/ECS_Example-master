#pragma once

#include "entityx/Entity.h"
#include "ICreatable.h"
#include "ScreenSize.h"
#include "components/Position.h"
#include "components/Background.h"
#include "components/Volume.h"
#include "components/Tank.h"
#include "components/Motion.h"
#include "components/Display.h"
#include "components/Turret.h"
#include "components/Parent.h"
#include "utils/LevelLoader.h"

#include "components/Wall.h" 
#include "PathNode.h"
#include "components/AI.h"
#include "ai/TankAI.h"
//#include "systems/AiControlSystem.h"

class WallCreator : public ICreatable
{
public:

	WallCreator(std::string type,
		sf::Vector2f position,
		double rotation);

	void create(entityx::Entity& entity);

private:
	std::string m_type;
	sf::Vector2f m_position;
	double m_rotation;
};

class PathNodeCreator : public ICreatable
{
public: 
	PathNodeCreator(std::string type, sf::Vector2f position, double rotation);

	void create(entityx::Entity& entity);
private:
	std::string m_type;
	sf::Vector2f m_position;
	double m_rotation;
};

class BackgroundCreator : public ICreatable
{
public:

	BackgroundCreator(const std::string& fileName);

	void create(entityx::Entity& entity) override;

private:

	std::string m_fileName;
};


class TankBaseCreator : public ICreatable
{
public:

	TankBaseCreator(sf::Vector2f position=sf::Vector2f(0,0), bool isAi = false);

	void create(entityx::Entity& entity);

private:
	
	sf::Vector2f m_position;

	bool m_isAi;
};

class TurretCreator : public ICreatable
{
public:

	TurretCreator(TankData const& tankData, entityx::Entity& parentEntity, bool isAi);

	void create(entityx::Entity& entity);

private:
	TankData const& m_tankData;
	entityx::Entity& m_parentEntity;
	bool m_isAi;
};
