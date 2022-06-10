#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "SFML/Graphics.hpp"

#include "Arial.h"

struct Line
{
public:
	enum class Type { Light, Shadow, Transparent };

	sf::Vector2f m_Origin, m_Direction;
	float t = 1.f;
	bool m_IsValid = false;
	Type m_Type = Type::Transparent;
	sf::Color m_DrawColor;

	Line() = default;
	Line(const sf::Vector2f& origin, const sf::Vector2f& direction) : m_Origin(origin), m_Direction(direction) {}

	void SetTransparent() { m_DrawColor = sf::Color::Transparent; m_Type = Type::Transparent; }
	void SetLight() { m_DrawColor = sf::Color(255, 255, 102); m_Type = Type::Light; }
	void SetDark() { m_DrawColor = sf::Color(70, 70, 70); m_Type = Type::Shadow; }

	void Draw(sf::RenderWindow& window) const
	{
		if (!m_IsValid || m_Type == Type::Transparent)
			return;

		const float x = m_Origin.x + m_Direction.x * t;
		const float y = m_Origin.y + m_Direction.y * t;
		const sf::Vertex line[2] =
		{
			sf::Vertex(sf::Vector2f(m_Origin.x, m_Origin.y), m_DrawColor),
			sf::Vertex(sf::Vector2f(x, y), m_DrawColor)
		};
		window.draw(line, 2, sf::Lines);
	}
};


struct CoordinateTransformer
{
public:
	const sf::Vector2f m_Origin; // 0-0

	CoordinateTransformer(const sf::RenderWindow& window) 
		: m_Origin(sf::Vector2f(static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f)) {}

	sf::Vector2f Transform(float x, float y) const
	{
		return sf::Vector2f(x, y) + m_Origin;
	}

	sf::Vector2f Transform(const sf::Vector2f& coord) const
	{
		return Transform(coord.x, coord.y);
	}

	sf::Vector2f Normalize(float x, float y) const
	{
		return sf::Vector2f(x, y) - m_Origin;
	}

	sf::Vector2f Normalize(const sf::Vector2f& coord) const
	{
		return Normalize(coord.x, coord.y);
	}
};


struct Text : public sf::Text
{
private:
	size_t m_Length;
	float m_WindowXSize;
	float m_YPos;
	std::string m_Text;
private:
	void InitObject(const sf::Font& font, float y, const std::string& text)
	{
		setFont(font);
		setCharacterSize(20);
		setFillColor(sf::Color::White);
		setString(m_Text);
		setPosition(m_YPos);
	}

	void UpdatePosition()
	{
		size_t textLength = getString().getSize();
		if (textLength != m_Length)
		{
			setPosition(m_YPos);
			m_Length = textLength;
		}
	}
public:
	Text() : m_Length(0), m_WindowXSize(0.f), m_YPos(0.f) {}
	Text(const sf::Font& font, unsigned int windowXSize, float y, const std::string& text)
		: m_Length(text.size()), m_WindowXSize(static_cast<float>(windowXSize)), m_YPos(y), m_Text(text) 
		{ InitObject(font, y, text); }

	void Init(const sf::Font& font, unsigned int windowXSize, float y, const std::string& text)
	{
		m_Length = text.size();
		m_WindowXSize = static_cast<float>(windowXSize);
		m_YPos = y;
		m_Text = text;
		InitObject(font, y, text); 
	}

	void setPosition(float y)
	{
		sf::FloatRect rect = getLocalBounds();
		sf::Text::setPosition(m_WindowXSize - rect.width - 20.f, y);
	}

	void SetWindowXSize(unsigned int windowXSize) { m_WindowXSize = static_cast<float>(windowXSize); m_Length += 1; UpdatePosition(); }
	
	void Update(size_t validRays) { Update(std::to_string(validRays)); }
	void Update(const std::string& str)
	{
		setString(m_Text + str);
		UpdatePosition();
	}
};



float GetCorrectTValue(const sf::Vector2f& origin, const sf::Vector2f& direction, float t1, float t2, bool getShorter)
{
	const sf::Vector2f point1 = origin + direction * t1;
	const sf::Vector2f point2 = origin + direction * t2;

	const sf::Vector2f vec1 = point1 - origin;
	const sf::Vector2f vec2 = point2 - origin;

	const float length1 = vec1.x * vec1.x + vec1.y * vec1.y;
	const float length2 = vec2.x * vec2.x + vec2.y * vec2.y;

	if (length1 < length2)
	{
		if (getShorter)
			return t1;
		else
			return t2;
	}
	else
	{
		if (getShorter)
			return t2;
		else
			return t1;
	}

	//return length1 < length2 ? t1 : t2;
	//return length1 < length2 ? t2 : t1;
}


std::pair<Line, Line> CalculateRays(const Line& line, float radius, const CoordinateTransformer& tf, const sf::Vector2f& circlePos)
{
	Line light, shadow;

	const float a = line.m_Direction.x * line.m_Direction.x + line.m_Direction.y * line.m_Direction.y;
	const float b = 2.f * line.m_Origin.x * line.m_Direction.x - 2.f * line.m_Direction.x * circlePos.x + 2.f * line.m_Origin.y * line.m_Direction.y - 2.f * line.m_Direction.y * circlePos.y;
	const float c = line.m_Origin.x * line.m_Origin.x - 2.f * line.m_Origin.x * circlePos.x + circlePos.x * circlePos.x + line.m_Origin.y * line.m_Origin.y - 2.f * line.m_Origin.y * circlePos.y + circlePos.y * circlePos.y - radius * radius;

	const float discriminant = b * b - 4.f * a * c;
	if (discriminant < 0)
		return { Line(), Line() };
	else if (discriminant == 0)
	{
		light.t = -b / (2.f * a);
		shadow.t = light.t;
	}
	else
	{
		const float t1 = (-b + sqrt(discriminant)) / (2.f * a);
		const float t2 = (-b - sqrt(discriminant)) / (2.f * a);
		light.t = GetCorrectTValue(line.m_Origin, line.m_Direction, t1, t2, true);
		shadow.t = GetCorrectTValue(line.m_Origin, line.m_Direction, t1, t2, false);
	}

	light.m_Direction = line.m_Direction;
	light.m_Origin = tf.Transform(line.m_Origin);
	light.m_IsValid = true;
	light.SetLight();

	shadow.m_Direction = line.m_Direction;
	shadow.m_Origin = tf.Transform(line.m_Origin + shadow.t * line.m_Direction);
	shadow.t = shadow.t * 10000;
	shadow.m_IsValid = true;
	shadow.SetDark();
	return { light, shadow };
}


template <size_t S>
struct TextFactory
{
private:
	using cstringr = const std::string&;
	using pair = const std::pair<std::int32_t, size_t>&;
	using ppair = const std::tuple<std::int32_t, bool, cstringr, cstringr>&;
private:
	sf::Font m_Font;
	unsigned int m_WindowSizeX;
	float m_YPos;
	float m_YOffset;
	size_t m_GeneratedTexts;
	std::array<Text, S> m_Texts;
public:
	TextFactory(unsigned int windowXSize, float y, float yOffset)
		: m_WindowSizeX(windowXSize), m_YPos(y), m_YOffset(yOffset), m_GeneratedTexts(0)
		{ m_Font.loadFromMemory(sg_RawArialData, sg_RawArialDataRelativeSize); }

	void DrawTexts(sf::RenderWindow& window) const
	{
		for (size_t i = 0; i < m_GeneratedTexts; ++i)
			window.draw(m_Texts[i]);
	}

	void UpdateWindowSizeX(unsigned int windowXSize)
	{
		m_WindowSizeX = windowXSize;
		for (size_t i = 0; i < m_GeneratedTexts; ++i)
			m_Texts[i].SetWindowXSize(windowXSize);
	}

	void SetFillColor(const sf::Color& color)
	{
		for (size_t i = 0; i < m_GeneratedTexts; ++i)
			m_Texts[i].setFillColor(color);
	}

	void UpdateText(size_t index, const std::string& str)
	{
		m_Texts[index].Update(str);
	}

	void UpdateText(size_t index, size_t value)
	{
		m_Texts[index].Update(value);
	}

	void UpdateText(pair content)
	{
		m_Texts[content.first].Update(content.second);
	}

	void UpdateText(ppair content)
	{
		if(std::get<1>(content))
			UpdateText(std::get<0>(content), std::get<2>(content));
		else
			UpdateText(std::get<0>(content), std::get<3>(content));
	}

	std::int32_t GenerateText(const std::string& text)
	{
		if (m_GeneratedTexts == S)
			return -1;
		m_Texts[m_GeneratedTexts].Init(m_Font, m_WindowSizeX, m_YPos, text);
		++m_GeneratedTexts;
		m_YPos += m_YOffset;
		return m_GeneratedTexts-1;
	}
	
	void UpdateTexts(pair rays, pair radius, std::pair<std::int32_t, const std::string&> fpsLimit, 
		ppair lightText, ppair shadowText, ppair textColorText)
	{
		UpdateText(rays);
		UpdateText(rays.first + 1, rays.second / 2); // light ray
		UpdateText(rays.first + 2, rays.second / 2); // shadow ray

		UpdateText(radius);
		UpdateText(fpsLimit.first, fpsLimit.second);
		
		UpdateText(lightText);
		UpdateText(shadowText);
		UpdateText(textColorText);
	}
};


struct DisplayedTexts
{
public:
	const std::string onStr = "On";
	const std::string offStr = "Off";
	const std::string whiteStr = "White";
	const std::string blackStr = "Black";

	TextFactory<9> factory;
	std::int32_t fps, rays, lightRays, shadowRays, radius, light, shadow, textColor, fpsLimit;
	bool lightOn, shadowOn, whiteTextColor;
	std::string fpsLimitStr;
	unsigned int fpsLimitValue = 60;

	DisplayedTexts(unsigned int windowSizeX, float yPos, float yOffset)
		: factory(windowSizeX, yPos, yOffset)
	{
		fps        = factory.GenerateText("FPS: ");
		rays       = factory.GenerateText("Rays: ");
		lightRays  = factory.GenerateText("Light rays: ");
		shadowRays = factory.GenerateText("Shadow rays: ");
		radius     = factory.GenerateText("Radius(Up,Down,->,<-): ");
		light      = factory.GenerateText("Light(a): ");
		shadow     = factory.GenerateText("Shadow(d): ");
		textColor  = factory.GenerateText("Text color(e): ");
		fpsLimit   = factory.GenerateText("FPS limit(w/s/f): ");

		lightOn = true;
		shadowOn = true;
		whiteTextColor = true;

		fpsLimitStr = std::to_string(fpsLimitValue);
	}

	void UpdateTexts(size_t raysAmount, float radiusValue)
	{
		factory.UpdateTexts(
			{rays, raysAmount},
			{radius, static_cast<size_t>(radiusValue)},
			{fpsLimit, fpsLimitStr },
			std::make_tuple(light, lightOn, onStr, offStr),
			std::make_tuple(shadow, shadowOn, onStr, offStr),
			std::make_tuple(textColor, whiteTextColor, whiteStr, blackStr)
		);
	}

	void UpdateText(int32_t index, size_t value)
	{
		factory.UpdateText(index, value);
	}

	void SetFillColor(const sf::Color& color)
	{
		factory.SetFillColor(color);
	}

	void UpdateWindowSizeX(unsigned int windowXSize)
	{
		factory.UpdateWindowSizeX(windowXSize);
	}

	void DrawTexts(sf::RenderWindow& window)
	{
		factory.DrawTexts(window);
	}
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 750), "Rays");

	DisplayedTexts texts(window.getSize().x, 0.f, 30.f);
	window.setFramerateLimit(texts.fpsLimitValue);

	const CoordinateTransformer tf(window);

	sf::CircleShape circle(100.f, 70);
	circle.setPosition(tf.Transform(0 - circle.getRadius(), 0 - circle.getRadius()));
	circle.setFillColor(sf::Color::White);

	const float p1 = -500.f;
	const float p2 = -375.f;
	const float dir1 = 0.f;
	const float dir2 = 1.f;

	std::vector<Line> rays;
	float offset = 0.001f;
	float YAngle = dir1;
	float XAngle = dir2;
	float XPos = p1;
	float YPos = p2;
	float lastValidAngle = XAngle;

	float fps = 0.f;
	const sf::Clock clock;
	sf::Time previousTime = clock.getElapsedTime();
	sf::Time currentTime;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
				window.setView(sf::View(visibleArea));
				texts.UpdateWindowSizeX(window.getSize().x);
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Left)
				{
					if (circle.getRadius() != 1.f)
						circle.setRadius(circle.getRadius() - 1.f);
				}
				else if (event.key.code == sf::Keyboard::Right)
					circle.setRadius(circle.getRadius() + 1.f);
				else if (event.key.code == sf::Keyboard::A)
					texts.lightOn = !texts.lightOn;
				else if (event.key.code == sf::Keyboard::D)
					texts.shadowOn = !texts.shadowOn;
				else if (event.key.code == sf::Keyboard::S)
				{
					texts.fpsLimitValue = texts.fpsLimitValue == 1 ? 1 : texts.fpsLimitValue - 1;
					window.setFramerateLimit(texts.fpsLimitValue);
					texts.fpsLimitStr = std::to_string(texts.fpsLimitValue);
				}
				else if (event.key.code == sf::Keyboard::W)
				{
					texts.fpsLimitValue = texts.fpsLimitValue + 1;
					window.setFramerateLimit(texts.fpsLimitValue);
					texts.fpsLimitStr = std::to_string(texts.fpsLimitValue);
				}
				else if (event.key.code == sf::Keyboard::F)
				{
					if (texts.fpsLimitStr == "Off")
					{
						window.setFramerateLimit(texts.fpsLimitValue);
						texts.fpsLimitStr = std::to_string(texts.fpsLimitValue);
					}
					else
					{
						window.setFramerateLimit(UINT_MAX);
						texts.fpsLimitStr = "Off";
					}
				}
				else if (event.key.code == sf::Keyboard::E)
				{
					texts.whiteTextColor = !texts.whiteTextColor;
					if (texts.whiteTextColor)
						texts.SetFillColor(sf::Color::White);
					else 
						texts.SetFillColor(sf::Color::Black);
				}
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())
		{
			const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			circle.setPosition({ static_cast<float>(mousePos.x) - circle.getRadius(), static_cast<float>(mousePos.y) - circle.getRadius() });
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && window.hasFocus())
		{
			circle.setRadius(circle.getRadius() + 1.f);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && window.hasFocus())
		{
			if (circle.getRadius() != 1.f)
				circle.setRadius(circle.getRadius() - 1.f);
		}


		for (size_t i = 0; i < 10000; ++i)
		{
			if (i == 3000)
				offset = 0.01f;
			else if (i == 6000)
				offset = 0.1f;
			else if (i == 8000)
				offset = 1.f;

			Line line({ XPos, YPos }, { XAngle, YAngle });
			std::pair<Line, Line> lines = CalculateRays(line, circle.getRadius(), tf, tf.Normalize(circle.getPosition().x + circle.getRadius(), circle.getPosition().y + circle.getRadius()));
			if (lines.first.m_IsValid)
			{
				lastValidAngle = YAngle;
				rays.push_back(std::move(lines.first));
				rays.push_back(std::move(lines.second));
			}
			YAngle += offset;
		}
		texts.UpdateTexts(rays.size(), circle.getRadius());

		YAngle = dir1;
		offset = 0.001f;


		if (!texts.lightOn || !texts.shadowOn)
		{
			for (Line& ray : rays)
			{
				if (ray.m_Type == Line::Type::Light && !texts.lightOn)
					ray.SetTransparent();
				else if (ray.m_Type == Line::Type::Shadow && !texts.shadowOn)
					ray.SetTransparent();
			}
		}

		window.clear(sf::Color(105, 105, 105, 255));
		window.draw(circle);
		for (size_t i = 0; i < rays.size(); ++i)
			rays.at(i).Draw(window);
		texts.DrawTexts(window);
		window.display();
		rays.clear();

		currentTime = clock.getElapsedTime();
		fps = 1.f / (currentTime.asSeconds() - previousTime.asSeconds());
		previousTime = currentTime;
		texts.UpdateText(texts.fps, static_cast<size_t>(fps));
	}

	return 0;
}