#include "HUDSystem.h"

HUDSystem::HUDSystem(sf::RenderWindow& window) : m_window(window)
{
	m_stringNode = "current node = 0";
	m_stringTime = "current time = ";
	std::cout << "Past Lap Times: " << std::endl;
	if (!m_font.loadFromFile("c:/windows/fonts/comic.ttf"))
	{
		std::cout << "cant load font file" << std::endl;
	}

	//m_textNode.setString(m_stringNode);
	m_textNode.setPosition(100, 100);
	m_textNode.setFont(m_font);

	//m_textTime.setString(m_stringTime);
	m_textTime.setPosition(100, 200);
	m_textTime.setFont(m_font);
}

void HUDSystem::update(entityx::EntityManager& entities,
					   entityx::EventManager& events, 
					   double dt)
{
	m_currentTime += dt/160;

	m_stringTime = "Time: " + std::to_string(m_currentTime);
	//m_stringNode = m_stringTime + std::to_string(m_currentNode);

	m_textTime.setString(m_stringTime);
	m_textNode.setString(m_stringNode);

	m_window.draw(m_textTime);
	m_window.draw(m_textNode);

}

void HUDSystem::configure(entityx::EventManager& events)
{
	events.subscribe<ReturnPath>(*this);
	
}

void HUDSystem::receive(const ReturnPath& node)
{
	m_currentNode = node.m_node;
	m_stringNode = "current node = " + std::to_string(m_currentNode);
	if (m_currentNode == 0)
	{
		std::cout << "  Lap: " << m_currentTime << std::endl;
		m_currentTime = 0;
	}
	
}