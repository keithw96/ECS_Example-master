#include "ai/TankAi.h"

int m_currentNode;

TankAi::TankAi(std::vector<sf::CircleShape> const & obstacles, std::vector<sf::CircleShape> const & pathNodes,  entityx::Entity::Id id)
	: m_aiBehaviour(AiBehaviour::FOLLOW_PATH)
	, m_steering(0, 0)
	, m_obstacles(obstacles)
	, m_pathNodes(pathNodes)
	
{
	m_currentNode = 0;
}

void TankAi::update(entityx::Entity::Id playerId,
	entityx::Entity::Id aiId,
	entityx::EntityManager& entities,
	entityx::EventManager& events,
	double dt)
{
	entityx::Entity aiTank = entities.get(aiId);
	Motion::Handle motion = aiTank.component<Motion>();
	Position::Handle position = aiTank.component<Position>();

	sf::Vector2f vectorToPlayer = seek(playerId,
		aiId,
		entities);

	sf::Vector2f vectorToPath = seekPath(aiId,
		entities,
		events);

	switch (m_aiBehaviour)
	{
	case AiBehaviour::SEEK_PLAYER:
		m_steering += thor::unitVector(vectorToPlayer);
		m_steering += collisionAvoidance(aiId, entities);
		m_steering = Math::truncate(m_steering, MAX_FORCE);
		m_velocity = Math::truncate(m_velocity + m_steering, MAX_SPEED);
		break;
	case AiBehaviour::FOLLOW_PATH:
		m_steering += thor::unitVector(vectorToPath);
		m_steering += collisionAvoidance(aiId, entities);
		m_steering = Math::truncate(m_steering, MAX_FORCE);
		m_velocity = Math::truncate(m_velocity + m_steering, MAX_SPEED);
		break;
	case AiBehaviour::STOP:
		motion->m_speed = 0;
	default:
		break;
	}

	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180; 

	auto currentRotation = position->m_rotation;	

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}
	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		position->m_rotation = static_cast<int>((position->m_rotation) + 1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		position->m_rotation = static_cast<int>((position->m_rotation) - 1) % 360;
	}

	
	if (thor::length(vectorToPlayer) < MAX_SEE_AHEAD)
	{
		m_aiBehaviour = AiBehaviour::STOP;
	}	
	else
	{
		motion->m_speed = thor::length(m_velocity);	
		m_aiBehaviour = AiBehaviour::FOLLOW_PATH;
	}
}

sf::Vector2f TankAi::seek(entityx::Entity::Id playerId,
						  entityx::Entity::Id aiId,
	                      entityx::EntityManager& entities) const
{
	Position::Handle aiPos = entities.get(aiId).component<Position>();
	sf::Vector2f aiVec = aiPos->m_position;

	Position::Handle playerPos = entities.get(playerId).component<Position>();
	sf::Vector2f playerVec = playerPos->m_position;

	sf::Vector2f pos(playerVec - aiVec);
	return pos;
}

sf::Vector2f TankAi::seekPath(entityx::Entity::Id aiId,
	entityx::EntityManager& entities,
	entityx::EventManager & events) const
{
	Position::Handle aiPos = entities.get(aiId).component<Position>();
	sf::Vector2f aiVec = aiPos->m_position;

	sf::Vector2f pos(m_pathNodes[m_currentNode].getPosition() - aiVec);

	if (distance(aiVec, m_pathNodes[m_currentNode].getPosition()) < 10.0f)
	{
		if (m_currentNode < m_pathNodes.size() - 1)
		{
			m_currentNode = m_currentNode + 1;

			events.emit<ReturnPath>(m_currentNode);

		}
		
		else
		{
			m_currentNode = 0;
			events.emit<ReturnPath>(m_currentNode);
		}
		//std::cout << std::to_string(m_currentNode) << std::endl;
	}

	

	return pos;
}

float TankAi::distance(sf::Vector2f tankPos,
		sf::Vector2f nodePos) const
{
	return sqrt((tankPos.x - nodePos.x) * (tankPos.x - nodePos.x) + (tankPos.y - nodePos.y) * (tankPos.y - nodePos.y));
}

sf::Vector2f TankAi::collisionAvoidance(entityx::Entity::Id aiId, 
									    entityx::EntityManager& entities)
{
	entityx::Entity aiTank = entities.get(aiId);
	Position::Handle aiPos = aiTank.component<Position>();	

	auto headingRadians = thor::toRadian(aiPos->m_rotation);	
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = aiPos->m_position + headingVector;	
	m_halfAhead = aiPos->m_position + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle(aiId, entities);
	sf::Vector2f avoidance(0, 0);
	if (mostThreatening.getRadius() != 0.0)
	{
		auto threatPos = mostThreatening.getPosition();
		auto mypos = aiPos->m_position;
		avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
		avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
	}
	else
	{
		avoidance *= 0.0f;
	}
    return avoidance;
}

const sf::CircleShape TankAi::findMostThreateningObstacle(entityx::Entity::Id aiId,
																     entityx::EntityManager& entities) 
{
	sf::CircleShape obstacle;
	sf::CircleShape mostThreatening(0);

	bool collision = false;

	for (int i = 0; i < m_obstacles.size(); i++)
	{
		obstacle = m_obstacles[i];
		
		if (Math::lineIntersectsCircle(m_ahead, m_halfAhead, obstacle))
		{
			mostThreatening = obstacle;
		}
	}

	return mostThreatening;
}

