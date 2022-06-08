#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>

#include "SudokuBoard.h"

#define DIR_NONE 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4


void SudokuBoard::ForEachQuad(ForEachQuadFunction func)
{
	for (size_t row = 0; row < m_Quads.size(); row++)
	{
		for (size_t column = 0; column < m_Quads[row].size(); column++)
		{
			if (func(m_Quads[row][column], row, column))
				return;
		}
	}
}


void SudokuBoard::ForEachField(ForEachFieldFunction func)
{
	int index = 0;
	for (size_t row = 0; row < (m_Quads.size() / 3); row++)
	{
		for (size_t column = 0; column < (m_Quads[row].size() / 3); column++)
		{
			func(m_Fields[index], row, column);
			++index;
		}
	}
}


void SudokuBoard::HandleKeyboardInput(int8_t number)
{
	ForEachQuad([&](SudokuQuad& quad, size_t, size_t) -> bool
	{
		if (quad.IsSelected())
		{
			quad.SetValue(number);
			return true;
		}
		return false;
	});
}


void SudokuBoard::MoveSelected(int8_t direction)
{
	std::pair<std::reference_wrapper<SudokuQuad>, int8_t> selectedRect = GetSelectedRect();
	int8_t index = selectedRect.second;

	if (index == -1)
	{
		direction = DIR_NONE;
		index = 0;
	}

	switch (direction)
	{
	case DIR_UP:
		if(index - 9 >= 0)
			index -= 9;
		break;
	case DIR_DOWN:
		if (index + 9 <= 80)
			index += 9;
		break;
	case DIR_LEFT:
		if(index - 1 >= 0)
			index -= 1;
		break;
	case DIR_RIGHT:
		if (index + 1 <= 80)
			index += 1;
		break;
	default:
		break;
	}

	if(index != selectedRect.second)
		selectedRect.first.get().SetSelected(false);

	int8_t counter = 0;
	ForEachQuad([&](SudokuQuad& quad, size_t, size_t) mutable -> bool
	{
		if(counter == index)
		{
			quad.SetSelected(true);
			return true;
		}
		
		++counter;
		return false;
	});
}


std::pair<std::reference_wrapper<SudokuQuad>, int8_t> SudokuBoard::GetSelectedRect()
{
	int8_t index = -1;
	size_t row_s = 0;
	size_t column_s = 0;
	bool found = false;

	ForEachQuad([&](SudokuQuad& quad, size_t row, size_t column) mutable -> bool
	{
		++index;
		if (quad.IsSelected())
		{
			row_s = row;
			column_s = column;
			found = true;
			return true;
		}
		return false;
	});
	
	if (!found)
		index = -1;

	return std::make_pair(std::ref(m_Quads[row_s][column_s]), index);
}


void SudokuBoard::SetRowColor(size_t row, sf::Color color)
{
	for (size_t i = 0; i < m_Quads[row].size(); i++)
	{
		m_Quads[row][i].setFillColor(color);
	}
}


void SudokuBoard::SetColumnColor(size_t column, sf::Color color)
{
	for (size_t i = 0; i < m_Quads.size(); i++)
	{
		m_Quads[i][column].setFillColor(color);
	}
}


void SudokuBoard::SetFieldColor(size_t field, sf::Color color)
{
	ForEachQuad([&](SudokuQuad& quad, size_t, size_t) -> bool
	{
		if (quad.GetField() == field)
		{
			quad.setFillColor(color);
		}
		return false;
	});
}


int8_t SudokuBoard::GetRowSum(size_t row)
{
	int8_t sum = 0;
	for (size_t i = 0; i < m_Quads[row].size(); i++)
	{
		sum += m_Quads[row][i].GetValue();
	}
	return sum;
}


int8_t SudokuBoard::GetColumnSum(size_t column)
{
	int8_t sum = 0;
	for (size_t i = 0; i < m_Quads.size(); i++)
	{
		sum += m_Quads[i][column].GetValue();
	}
	return sum;
}


int8_t SudokuBoard::GetFieldSum(size_t field)
{
	int8_t sum = 0;
	ForEachQuad([&](SudokuQuad& quad, size_t, size_t) -> bool
	{
		if (quad.GetField() == field)
		{
			sum += quad.GetValue();
		}
		return false;
	});
	return sum;
}


bool SudokuBoard::ValidateRows()
{
	bool wrongRows = false;
	for (size_t row = 0; row < m_Quads.size(); row++)
	{
		if (GetRowSum(row) != 45)
		{
			SetRowColor(row, sf::Color::Red);
			wrongRows = true;
		}
	}
	return !wrongRows;
}


bool SudokuBoard::ValidateColumns()
{
	bool wrongColumns = false;
	for (size_t column = 0; column < m_Quads[0].size(); column++)
	{
		if (GetColumnSum(column) != 45)
		{
			SetColumnColor(column, sf::Color::Red);
			wrongColumns = true;
		}
	}
	return !wrongColumns;
}


bool SudokuBoard::ValidateFields()
{
	bool wrongFields = false;
	for (size_t i = 0; i < m_Fields.size(); ++i)
	{
		if (GetFieldSum(i) != 45)
		{
			SetFieldColor(i, sf::Color::Red);
			wrongFields = true;
		}
	}
	return !wrongFields;
}


bool SudokuBoard::ValidateGame()
{
	m_Finished = true;
	
	ValidateFields();

	if (!ValidateFields() || !ValidateRows() || !ValidateColumns())
		return false;

	for (size_t i = 0; i < m_Quads.size(); ++i)
		SetRowColor(i, sf::Color::Green);
	return true;
}


void SudokuBoard::Reset()
{
	for (size_t i = 0; i < m_Quads.size(); ++i)
	{
		SetRowColor(i, sf::Color::White);
	}
	m_Finished = false;
}


SudokuBoard::SudokuBoard(const sf::Vector2f& position)
	: m_Position(position)
{
	if(!m_Font.loadFromFile("res/arial.ttf"))
		std::cerr << "Failed to load font" << std::endl;
	
	float fieldOutlineThickness = 4.f;
	m_Background.setFillColor(sf::Color::Transparent);
	m_Background.setOutlineColor(sf::Color::Black);
	m_Background.setOutlineThickness(fieldOutlineThickness);
	m_Background.setPosition(position);
	m_Background.setSize(sf::Vector2f(QUAD_SIZE*3, QUAD_SIZE*3));

	ForEachField([&](sf::RectangleShape& field, size_t row, size_t column)
		{
			sf::Vector2f nextPos = position;
			nextPos.x += column * QUAD_SIZE;
			nextPos.y += row * QUAD_SIZE;
			field.setFillColor(sf::Color::Transparent);
			field.setOutlineColor(sf::Color::Black);
			field.setOutlineThickness(fieldOutlineThickness / 2.f);
			field.setPosition(nextPos);
			field.setSize(sf::Vector2f(QUAD_SIZE, QUAD_SIZE));
		});

	
	uint8_t counter = 0;
	ForEachQuad([&](SudokuQuad& quad, size_t row, size_t column) -> bool
		{
			int8_t field = 0;
			if (counter <= 2 || (counter >= 9 && counter <= 11) || (counter >= 18 && counter <= 20))
				field = 0;
			else if ((counter >= 3 && counter <= 5) || (counter >= 12 && counter <= 14) || (counter >= 21 && counter <= 23))
				field = 1;
			else if ((counter >= 6 && counter <= 8) || (counter >= 15 && counter <= 17) || (counter >= 24 && counter <= 26))
				field = 2;
			else if ((counter >= 27 && counter <= 29) || (counter >= 36 && counter <= 38) || (counter >= 45 && counter <= 47))
				field = 3;
			else if ((counter >= 30 && counter <= 32) || (counter >= 39 && counter <= 41) || (counter >= 48 && counter <= 50))
				field = 4;
			else if ((counter >= 33 && counter <= 35) || (counter >= 42 && counter <= 44) || (counter >= 51 && counter <= 53))
				field = 5;
			else if ((counter >= 54 && counter <= 56) || (counter >= 63 && counter <= 65) || (counter >= 72 && counter <= 74))
				field = 6;
			else if ((counter >= 57 && counter <= 59) || (counter >= 66 && counter <= 68) || (counter >= 75 && counter <= 77))
				field = 7;
			else if ((counter >= 60 && counter <= 62) || (counter >= 69 && counter <= 71) || (counter >= 78 && counter <= 80))
				field = 8;

			sf::Vector2f nextPos = position;
			nextPos.x += column * RECT_SIZE;
			nextPos.y += row * RECT_SIZE;
			quad.Init(nextPos, m_Font, field);
			++counter;
			return false;
		});
}


void SudokuBoard::Draw(sf::RenderWindow& window)
{
	ForEachQuad([&](SudokuQuad& quad, size_t, size_t) -> bool
		{
			quad.Draw(window, m_Finished);
			return false;
		});
	
	for(size_t i = 0; i < m_Fields.size(); i++)
	{
		window.draw(m_Fields[i]);
	}
	window.draw(m_Background);
}


void SudokuBoard::HandleInput(sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
	{
		int XPos = event.mouseButton.x;
		int YPos = event.mouseButton.y;
		bool alreadySelected = false;
		ForEachQuad([&](SudokuQuad& quad, size_t, size_t) -> bool
		{
			if (XPos >= quad.getPosition().x && XPos <= quad.getPosition().x + RECT_SIZE && YPos >= quad.getPosition().y && YPos <= quad.getPosition().y + RECT_SIZE && !alreadySelected)
			{
				alreadySelected = true;
				quad.SetSelected(true);
			}
			else
				quad.SetSelected(false);
			return false;
		});
	}

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code > 26 && event.key.code < 36) // 1-9
			HandleKeyboardInput(event.key.code - 26);
		else if (event.key.code > 75 && event.key.code < 85) // numpad
			HandleKeyboardInput(event.key.code - 75);
		else if (event.key.code == sf::Keyboard::Space)
			ValidateGame();
		else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
			MoveSelected(DIR_RIGHT);
		else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
			MoveSelected(DIR_LEFT);
		else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
			MoveSelected(DIR_UP);
		else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
			MoveSelected(DIR_DOWN);
		else if (event.key.code == sf::Keyboard::Escape)
			Reset();
	}
}


void SudokuBoard::GenerateBoard()
{
	static bool done = false;
	if (!done)
	{
		std::array<std::array<int8_t, 9>, 9> board;

		for (size_t i = 0; i < board.size(); i++)
		{
			for (size_t j = 0; j < board[i].size(); j++)
			{
				board[i][j] = (j + 1);
			}
		}

		for (size_t i = 0; i < board.size(); i++)
		{
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::shuffle(board[i].begin(), board[i].end(), std::default_random_engine(seed));
		}

		for (size_t i = 0; i < board.size(); i++)
		{
			for (size_t j = 0; j < board[i].size(); j++)
			{
				m_Quads[i][j].SetValue(board[i][j]);
			}
		}
	}

	if (!done && ValidateGame())
	{
		done = true;
	}

	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	//for (size_t i = 0; i < m_Quads.size(); i++)
	//{
	//static bool done = false;
	//if (!done)
	//{
	//	std::random_device dev;
	//	std::mt19937 rng(dev());
	//	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 9);
	//	//std::shuffle(m_Quads[i].begin(), m_Quads[i].end(), std::default_random_engine(seed));
	//	ForEachQuad([&](SudokuQuad& quad, size_t, size_t) -> bool
	//		{
	//			quad.SetValue(dist6(rng));
	//			return false;
	//		});
	//	//}
	//}
	//
	//if (ValidateGame() && !done)
	//{
	//	done = true;
	//	std::cout << "Generated board" << std::endl;
	//}
	//else if (!done)
	//{
	//	Reset();
	//}
}