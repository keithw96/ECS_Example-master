#pragma once

#include "entityx/System.h"

#include "AI\TankAi.h"

class HUDSystem : public entityx::System<HUDSystem>
	, public entityx::Receiver<ReturnPath>
{
public:
	HUDSystem(sf::RenderWindow& window);

	void update(entityx::EntityManager& entities,
		entityx::EventManager& events,
		double dt);

	void configure(entityx::EventManager& events);

	void receive(const ReturnPath& node);
private:
	int m_currentNode;
	double m_currentTime;
	sf::RenderWindow& m_window;
	sf::String m_stringTime;
	sf::String m_stringNode;
	sf::Font m_font;
	sf::Text m_textTime;
	sf::Text m_textNode;


};