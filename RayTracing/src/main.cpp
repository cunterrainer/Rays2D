#include <iostream>
#include <array>
#include <vector>

#include "SFML/Graphics.hpp"

struct Line
{
public:
    sf::Vector2f m_Origin, m_Direction;
    float t = 1.f;
    bool m_IsValid = false;
	sf::Color m_DrawColor;
	
    Line() = default;
    Line(const sf::Vector2f& origin, const sf::Vector2f& direction)
        : m_Origin(origin), m_Direction(direction) {}

	void SetLight() { m_DrawColor = sf::Color(255,255,102); }
	void SetDark() { m_DrawColor = sf::Color(70, 70, 70); }

    void Draw(sf::RenderWindow& window)
    {
        if (!m_IsValid)
            return;

		float x = m_Origin.x + m_Direction.x * t;
		float y = m_Origin.y + m_Direction.y * t;
		sf::Vertex line[] =
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
    sf::Vector2f m_Origin; // 0-0

    CoordinateTransformer(const sf::RenderWindow& window)
    {
        m_Origin = sf::Vector2f((float)(window.getSize().x / 2.f), ((float)window.getSize().y / 2.f));
    }

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



float GetCorrectTValue(const sf::Vector2f& origin, const sf::Vector2f& direction, float t1, float t2, bool getShorter)
{
    const sf::Vector2f point1 = origin + direction * t1;
	const sf::Vector2f point2 = origin + direction * t2;

    const sf::Vector2f vec1 = point1 - origin;
	const sf::Vector2f vec2 = point2 - origin;

	const float length1 = vec1.x * vec1.x + vec1.y * vec1.y;
	const float length2 = vec2.x * vec2.x + vec2.y * vec2.y;

	if(length1 < length2)
	{
		if(getShorter)
			return t1;
		else
			return t2;
	}
	else
	{
		if(getShorter)
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


int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 750), "Rays");
    window.setFramerateLimit(60);

    CoordinateTransformer tf(window);

    sf::CircleShape circle(100.f, 70);
    circle.setPosition(tf.Transform(0 - circle.getRadius(), 0 - circle.getRadius()));
    circle.setFillColor(sf::Color::White);

    float p1 = -500.f;
    float p2 = -375.f;
	float dir1 = 0.f;
    float dir2 = 1.f;

	std::vector<Line> rays;
    float offset = 0.001f;
    float YAngle = dir1 *-1;
	float XAngle = dir2;
    float XPos = p1;
	float YPos = p2;
    float lastValidAngle = XAngle;
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::Resized)
			{
                sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                window.setView(sf::View(visibleArea));
			}
        }
		
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())
        {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            circle.setPosition({ static_cast<float>(mousePos.x) - circle.getRadius(), static_cast<float>(mousePos.y) - circle.getRadius()});
        }
        
        for(size_t i = 0; i < 10000; ++i)
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
        std::cout << "Valid rays: " << rays.size() << " | Last valid angle: " << lastValidAngle << '\r';
		YAngle = dir1 *-1;
		offset = 0.001f;

        window.clear(sf::Color(105,105,105,255));
        window.draw(circle);
        for (size_t i = 0; i < rays.size(); ++i)
        {
            rays.at(i).Draw(window);
        }
        window.display();
		rays.clear();
    }

    return 0;
}