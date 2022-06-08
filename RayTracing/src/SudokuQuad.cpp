#include "SudokuQuad.h"

//void SudokuQuad::Init(const sf::Vector2f& position, const sf::Font& font, int8_t startRow, int8_t startColumn)
//{
//	m_Position = position;
//	m_Background.setFillColor(sf::Color::White);
//	m_Background.setOutlineColor(sf::Color::Black);
//	m_Background.setOutlineThickness(4.f);
//	m_Background.setPosition(m_Position);
//	m_Background.setSize(sf::Vector2f(QUAD_SIZE, QUAD_SIZE));
//
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		m_Rects[i].setFillColor(sf::Color::White);
//		m_Rects[i].setOutlineColor(sf::Color::Black);
//		m_Rects[i].setOutlineThickness(1.f);
//		m_Rects[i].setSize(sf::Vector2f(RECT_SIZE, RECT_SIZE));
//
//		m_Rects[i].SetFont(font);
//		m_Rects[i].UpdateText();
//	}
//
//	m_Rects[0].setPosition(m_Position, startRow, startColumn);
//	m_Rects[1].setPosition(m_Position + sf::Vector2f(RECT_SIZE, 0.f), startRow, startColumn + 1);
//	m_Rects[2].setPosition(m_Position + sf::Vector2f(RECT_SIZE*2, 0.f), startRow, startColumn + 2);
//	++startRow;
//
//	m_Rects[3].setPosition(m_Position + sf::Vector2f(0.f, RECT_SIZE), startRow, startColumn);
//	m_Rects[4].setPosition(m_Position + sf::Vector2f(RECT_SIZE, RECT_SIZE), startRow, startColumn + 1);
//	m_Rects[5].setPosition(m_Position + sf::Vector2f(RECT_SIZE*2, RECT_SIZE), startRow, startColumn + 2);
//	++startRow;
//	
//	m_Rects[6].setPosition(m_Position + sf::Vector2f(0.f, RECT_SIZE * 2), startRow, startColumn);
//	m_Rects[7].setPosition(m_Position + sf::Vector2f(RECT_SIZE, RECT_SIZE * 2), startRow, startColumn + 1);
//	m_Rects[8].setPosition(m_Position + sf::Vector2f(RECT_SIZE * 2, RECT_SIZE*2), startRow, startColumn + 2);
//}
//
//
//void SudokuQuad::Draw(sf::RenderWindow& window, bool gameFinished)
//{
//	window.draw(m_Background);
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		m_Rects[i].Draw(window, gameFinished);
//	}
//}
//
//
//uint8_t SudokuQuad::GetSum() const
//{
//	uint8_t sum = 0;
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		sum += m_Rects[i].GetValue();
//	}
//	return sum;
//}
//
//
//uint8_t SudokuQuad::GetRowSum(const int8_t row) const
//{
//	uint8_t sum = 0;
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		if(m_Rects[i].GetRow() == row)
//			sum += m_Rects[i].GetValue();
//	}
//	return sum;
//}
//
//
//uint8_t SudokuQuad::GetColumnSum(const int8_t column) const
//{
//	uint8_t sum = 0;
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		if (m_Rects[i].GetColumn() == column)
//			sum += m_Rects[i].GetValue();
//	}
//	return sum;
//}
//
//
//
//
//void SudokuQuad::SetColor(const sf::Color& color)
//{
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		m_Rects[i].setFillColor(color);
//	}
//}
//
//
//void SudokuQuad::SetRowColor(int8_t row, const sf::Color& color)
//{
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		if(m_Rects[i].GetRow() == row)
//			m_Rects[i].setFillColor(color);
//	}
//}
//
//
//void SudokuQuad::SetColumnColor(int8_t column, const sf::Color& color)
//{
//	for (size_t i = 0; i < m_Rects.size(); ++i)
//	{
//		if (m_Rects[i].GetColumn() == column)
//			m_Rects[i].setFillColor(color);
//	}
//}