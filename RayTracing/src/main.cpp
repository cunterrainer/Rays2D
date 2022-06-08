#include <iostream>
#include <array>

#include "SFML/Graphics.hpp"

struct Line
{
public:
    sf::Vector2f m_Origin, m_Direction;
    float t = 1.f;
    bool m_IsValid = false;
	
    Line() = default;
    Line(const sf::Vector2f& origin, const sf::Vector2f& direction)
        : m_Origin(origin), m_Direction(direction) {}

    void Draw(sf::RenderWindow& window)
    {
        if (!m_IsValid)
            return;

		float x = m_Origin.x + m_Direction.x * t;
		float y = m_Origin.y + m_Direction.y * t;
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(m_Origin.x, m_Origin.y), sf::Color::Green),
			sf::Vertex(sf::Vector2f(x, y), sf::Color::Green)
		};
        window.draw(line, 2, sf::Lines);
    }
};


struct CoordinateTransformer 
{
public:
    sf::Vector2f m_Origin; // null point

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
		return coord + m_Origin;
    }
};



float GetCorrectTValue(const sf::Vector2f& origin, const sf::Vector2f& direction, float t1, float t2)
{
    const sf::Vector2f point1 = origin + direction * t1;
	const sf::Vector2f point2 = origin + direction * t2;

    const sf::Vector2f vec1 = point1 - origin;
	const sf::Vector2f vec2 = point2 - origin;

	const float length1 = vec1.x * vec1.x + vec1.y * vec1.y;
	const float length2 = vec2.x * vec2.x + vec2.y * vec2.y;

	return length1 < length2 ? t1 : t2;
}


Line CalculateRay(const Line& line, float radius, const CoordinateTransformer& tf)
{
	Line result;

    const float a = line.m_Direction.x * line.m_Direction.x + line.m_Direction.y * line.m_Direction.y;
	const float b = 2.f * line.m_Origin.x * line.m_Direction.x + 2.f * line.m_Origin.y * line.m_Direction.y;
    const float c = line.m_Origin.x * line.m_Origin.x + line.m_Origin.y * line.m_Origin.y - radius * radius;

	const float determinant = b * b - 4.f * a * c;
    if (determinant < 0)
        return Line();
    else if (determinant == 0)
        result.t = -b / (2.f * a);
    else
    {
        const float t1 = (-b + sqrt(determinant)) / (2.f * a);
	    const float t2 = (-b - sqrt(determinant)) / (2.f * a);
        result.t = GetCorrectTValue(line.m_Origin, line.m_Direction, t1, t2);
    }

    result.m_Direction = line.m_Direction;
	result.m_Origin = tf.Transform(line.m_Origin);
    result.m_IsValid = true;
	return result;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 750), "Rays");
    window.setFramerateLimit(10);

    CoordinateTransformer tf(window);

    sf::CircleShape circle(100.f);
    circle.setPosition(tf.Transform(0 - circle.getRadius(), 0 - circle.getRadius()));
    circle.setFillColor(sf::Color::White);

    //Line line({-150.f, 0.f}, { 1.f,0.3f });
	//line = CalculateRay(line, 100.f, window);

    float p1 = 350.f;
    float p2 = 0.f;
    float dir1 = 0.29f;
    float dir2 = 1.f;

	std::array<Line, 236> rays;
    float offset = 0.01f;
    float YAngle = dir1 *-1;
	float XAngle = dir2;
    size_t valid = 0;
    float XPos = p1*-1;
	float YPos = p2;
    float lastValidAngle = XAngle;
    for (size_t i = 0; i < rays.size(); ++i)
    {
        if (i == rays.size() / 4)
        {
			YAngle = dir1 * -1;
			XAngle = dir2;
			XPos = p1;
			YPos = p2*-1;
        }
        else if (i == rays.size() / 2)
        {
            YAngle = dir2;
            XAngle = dir1 * -1;
            XPos = p2;
            YPos = p1;
        }
        else if (i == (rays.size() / 4)*3)
        {
            YAngle = dir2;
            XAngle = dir1 * -1;
            XPos = p2*-1;
            YPos = p1*-1;
        }

        Line line({ XPos, YPos }, { XAngle, YAngle });
        rays[i] = CalculateRay(line, 100.f, tf);
        if (rays[i].m_IsValid)
        {
            lastValidAngle = XAngle;
            ++valid;
        }
        if (i < rays.size() / 2)
        {
            YAngle += offset;
        }
        else
		{
			XAngle += offset;
		}
    }
	
    std::cout << "Valid rays: " << valid << std::endl;
	std::cout << "Last valid angle: " << lastValidAngle << std::endl;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(105,105,105,255));
        window.draw(circle);
        //line.Draw(window);
        for (size_t i = 0; i < rays.size(); ++i)
        {
            rays[i].Draw(window);
        }
        window.display();
    }

    return 0;
}



//sf::Vector2f GetIntersection(Line line1, Line line2)
//{
//	sf::Vector2f intersection;
//	float a1 = line1.m_Direction.y;
//	float b1 = -line1.m_Direction.x;
//	float c1 = line1.m_Origin.x * line1.m_Direction.y - line1.m_Origin.y * line1.m_Direction.x;
//
//	float a2 = line2.m_Direction.y;
//	float b2 = -line2.m_Direction.x;
//	float c2 = line2.m_Origin.x * line2.m_Direction.y - line2.m_Origin.y * line2.m_Direction.x;
//
//	float det = a1 * b2 - a2 * b1;
//	if (det != 0)
//	{
//		intersection.x = (b2 * c1 - b1 * c2) / det;
//		intersection.y = (a1 * c2 - a2 * c1) / det;
//	}
//	else
//	{
//		intersection.x = 0;
//		intersection.y = 0;
//	}
//	return intersection;
//}