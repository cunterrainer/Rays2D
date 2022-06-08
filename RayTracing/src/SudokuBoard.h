#pragma once

#include <array>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#include "SudokuQuad.h"

class SudokuBoard
{
private:
	using ForEachQuadFunction = std::function<bool(SudokuQuad&, size_t, size_t)>;
	using ForEachFieldFunction = std::function<void(sf::RectangleShape&, size_t, size_t)>;
private:
	std::array<std::array<SudokuQuad, 9>, 9> m_Quads;
	std::array<sf::RectangleShape, 9> m_Fields;
	sf::RectangleShape m_Background;

	const sf::Vector2f m_Position;
	sf::Font m_Font;
	bool m_Finished = false;
private:
	void ForEachQuad(ForEachQuadFunction func);
	void ForEachField(ForEachFieldFunction func);
	void HandleKeyboardInput(int8_t number);
	void MoveSelected(int8_t direction);
	std::pair<std::reference_wrapper<SudokuQuad>, int8_t> GetSelectedRect();
	void SetRowColor(size_t row, sf::Color color);
	void SetColumnColor(size_t column, sf::Color color);
	void SetFieldColor(size_t field, sf::Color color);
	int8_t GetRowSum(size_t row);
	int8_t GetColumnSum(size_t column);
	int8_t GetFieldSum(size_t field);
	bool ValidateRows();
	bool ValidateColumns();
	bool ValidateFields();
	bool ValidateGame();
	void Reset();
public:
	SudokuBoard(const sf::Vector2f& position);
	void Draw(sf::RenderWindow& window);
	void HandleInput(sf::Event& event);
	void GenerateBoard();
};