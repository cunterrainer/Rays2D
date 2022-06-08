#pragma once

#include <array>
#include <cstdint>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#define QUAD_SIZE 300.f
#define RECT_SIZE (QUAD_SIZE / 3.f)


class SudokuQuad : public sf::RectangleShape
{
private:
	int8_t m_Value = 0;
	int8_t m_Field = 0;

	sf::Text m_Text;
	bool m_Selected = false;
private:
	void SetFont(const sf::Font& font)
	{
		m_Text.setFont(font);
		m_Text.setCharacterSize(65);
		m_Text.setFillColor(sf::Color::Black);
	}

	void setPosition(const sf::Vector2f pos)
	{
		sf::RectangleShape::setPosition(pos);

		sf::FloatRect bounds = m_Text.getLocalBounds();
		float XPos = (pos.x + ((RECT_SIZE - bounds.width) / 2.f)) - bounds.left;
		float YPos = (pos.y + ((RECT_SIZE - bounds.height) / 2.f)) - bounds.top;

		m_Text.setPosition(XPos, YPos);
	}
public:
	void Init(const sf::Vector2f& pos, const sf::Font& font, int8_t field)
	{
		m_Field = field;
		SetFont(font);
		UpdateText();
		
		setPosition(pos);
		setSize(sf::Vector2f(RECT_SIZE, RECT_SIZE));
		setFillColor(sf::Color::White);
		setOutlineColor(sf::Color::Black);
		setOutlineThickness(1.f);
	}

	void SetValue(int8_t number)
	{
		m_Value = number;
		UpdateText();
	}

	void UpdateText()
	{
		m_Text.setString(std::to_string(m_Value));
	}

	int8_t GetValue() const
	{
		return m_Value;
	}

	int8_t GetField() const
	{
		return m_Field;
	}

	void SetSelected(bool selected)
	{
		m_Selected = selected;
	}

	bool IsSelected() const
	{
		return m_Selected;
	}

	void Draw(sf::RenderWindow& window, bool gameFinished)
	{
		if (!m_Selected && getFillColor() != sf::Color::White && !gameFinished)
			setFillColor(sf::Color::White);
		else if(m_Selected && sf::Color(255, 131, 0, 255) != getFillColor() && !gameFinished)
			setFillColor(sf::Color(255, 131, 0, 255));

		window.draw(*this);

		if(m_Value != 0)
			window.draw(m_Text);
	}
};