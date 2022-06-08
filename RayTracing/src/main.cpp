#include <iostream>
#include <array>
#define _USE_MATH_DEFINES
#include <math.h>

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



float GetCorrectTValue(const sf::Vector2f& origin, const sf::Vector2f& direction, float t1, float t2)
{
	// using pythagorean theorem to determine the length
    const sf::Vector2f point1 = origin + direction * t1;
	const sf::Vector2f point2 = origin + direction * t2;

    const sf::Vector2f vec1 = point1 - origin;
	const sf::Vector2f vec2 = point2 - origin;

	const float length1 = vec1.x * vec1.x + vec1.y * vec1.y;
	const float length2 = vec2.x * vec2.x + vec2.y * vec2.y;

	return length1 < length2 ? t1 : t2;
}


Line CalculateRay(const Line& line, float radius, const CoordinateTransformer& tf, const sf::Vector2f& circlePos)
{
	/*
        Equation of a circle:
		Circle (cx | cy) with radius r
		(x | y) = intersection with the circle
        (x - cx)^2 + (y - cy)^2 - r^2 = 0

		Equation of a line:
		c = (x  |  y) = intersection with the line
		a = (ax | ay) = origin of the line
		b = (bx | by) = direction of the line
		t = scalar
		
		c = a + b*t
		(x | y) = (ax | ay) + (bx | by) * t
		-> x = ax + bx*t
		-> y = ay + by*t

		Insert the line equation into the circle equation:
		reminder: (x - cx)^2 + (y - cy)^2 - r^2 = 0
		(ax + bx*t - cx)^2 + (ay + by*t - cy)^2 - r^2 = 0

		Bring it in the form of a quadratic equation:
		reminder: ax^2 + bx + c = 0
		We're gonna solve for t: at^2 + bt + c = 0

		1. Step: Solve the parantheses
		(ax + bxt - cx)^2 = (ax + bxt - cx) * (ax + bxt - cx)
		(ax + bxt - cx) * (ax + bxt - cx)
        = ax^2 + ax*bx*t - ax*cx + ax*bx*t + bx^2*t^2 - bx*t*cx - ax*cx - bx*t*cx + cx^2
		= ax^2 + 2ax*bx*t - 2ax*cx + bx^2*t^2 - 2bx*t*cx + cx^2
		
		Solving the y paranthesis works the same way:
        (ay + byt - cy)^2
		= ay^2 + 2ay*by*t - 2ay*cy + by^2*t^2 - 2by*t*cy + cy^2

		Insert it into the quadratic equation:
		ax^2 + 2ax*bx*t - 2ax*cx + bx^2*t^2 - 2bx*t*cx + cx^2 + ay^2 + 2ay*by*t - 2ay*cy + by^2*t^2 - 2by*t*cy + cy^2 - r^2 = 0
		
		2. Step: Simplify it by bringing same terms together
        (bx^2 + by^2)t^2 + (2ax*bx - 2bx*cx + 2ay*by - 2by*cy)t + (ax^2 - 2ax*cx + cx^2 + ay^2 - 2ay*cy + cy^2 - r^2) = 0
		------Y------       ----------------Y----------------      -----------------------Y-------------------------
              a                             b   					                      c

		3. Step: Solve the quadratic equation using the quadratic formula
		Check the discriminant:
		d = b^2 - 4ac
		if d < 0: no intersection
		if d = 0: one intersection
		if d > 0: two intersections

		if one or two intersections exist continue

		4. Step: Calculate the t values
		Two intersections (t values):
		t1|2 = (-b +- sqrt(d)) / 2a
		=> t1 = (-b + sqrt(d)) / 2a
		=> t2 = (-b - sqrt(d)) / 2a

		One intersection (t value):
		t = -b / 2a (Since the discriminant is 0)

		4. Step: Get the correct t value (the one that is closer to the origin, only if you're not interested in the second hit and if you have two intersections)
		Get both intersection points:
		c1 = a + b*t1
		c2 = a + b*t2

		Calculate the vectors
		v1 = c1 - a
		v2 = c2 - a

		Calculate the distance between the origin and the intersection points (pythagorean theorem):
		d1 = v1x^2 + v1y^2
		d2 = v2x^2 + v2y^2
		
		Check which one is closer to the origin:
		if d1 < d2: t1 is the correct t value
		if d2 < d1: t2 is the correct t value

		Now you have all you need to calculate the intersection point:
		c = a + b*t
		The vector from the origin to the intersection point is:
		v = c - a
		That's your ray!

		-----------------------------------------------------------------------------------------------
		Example:
		a = (-300 | 0)
		b = (1 | 0)
		r = 100
		Circle = (0 | 0)
		
		insert it into the quadratic equation:
		(bx^2 + by^2)t^2 + (2ax*bx - 2bx*cx + 2ay*by - 2by*cy)t + (ax^2 - 2ax*cx + cx^2 + ay^2 - 2ay*cy + cy^2 - r^2) = 0
		=> (1^2 + 0^2)t^2 + (2*-300*1 - 2*1*0 + 2*0*0 - 2*0*0)t + ((-300)^2 - 2*-300*0 + 0^2 + 0^2 - 2*0*0 + 0^2 - 100^2) = 0
		=>  1t^2 - 600t + 80000 = 0

		Solve the quadratic equation:
		discriminant = b^2 - 4ac => d = (-600)^2 - 4*1*80000
		d = 40000
        -> d > 0: two intersections

		insert it into the quadratic formular:
		t = (-b +- sqrt(d)) / 2*a
		=> t1 = (600 + sqrt(d)) / 2*1
		=> t2 = (600 - sqrt(d)) / 2*1

		t1 = 400
		t2 = 200

		get the correct t value:
		c1 = a + b*t1
		c2 = a + b*t2

		c1 = (-300 | 0) + (1 | 0)*400 = ( 100 | 0)
		c2 = (-300 | 0) + (1 | 0)*200 = (-100 | 0)

		Calculate the vectors
		v1 = c1 - a
		v2 = c2 - a

		v1 = (100 | 0) - (-300 | 0) = (400 | 0)
		v2 = (-100 | 0) - (-300 | 0) = (200 | 0)

		Calculate the distance between the origin and the intersection points (pythagorean theorem):
		d1 = v1x^2 + v1y^2
		d2 = v2x^2 + v2y^2

		d1 = (400)^2 + 0^2 = 160000
		d2 = (200)^2 + 0^2 = 40000
		
		Check which one is closer to the origin:
		if d1 < d2: t1 is the correct t value
		if d2 < d1: t2 is the correct t value

		d2 < d1 => t2 is the correct t value

		c = a + b*t2
		c = (-300 | 0) + (1 | 0)*200 = (-100 | 0)
		v = c - a
		v = (-100 | 0) - (-300 | 0) = (200 | 0)
		That's your ray!
    */
	Line result;

    const float a = line.m_Direction.x * line.m_Direction.x + line.m_Direction.y * line.m_Direction.y;
	const float b = 2.f * line.m_Origin.x * line.m_Direction.x - 2 * line.m_Direction.x * circlePos.x + 2.f * line.m_Origin.y * line.m_Direction.y - 2.f * line.m_Direction.y * circlePos.y;
    const float c = line.m_Origin.x * line.m_Origin.x - 2.f * line.m_Origin.x * circlePos.x + circlePos.x * circlePos.x + line.m_Origin.y * line.m_Origin.y - 2.f * line.m_Origin.y * circlePos.y + circlePos.y * circlePos.y - radius * radius;

	const float discriminant = b * b - 4.f * a * c;
    if (discriminant < 0)
        return Line();
    else if (discriminant == 0)
        result.t = -b / (2.f * a);
    else
    {
        const float t1 = (-b + sqrt(discriminant)) / (2.f * a);
	    const float t2 = (-b - sqrt(discriminant)) / (2.f * a);
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
    window.setFramerateLimit(60);

    CoordinateTransformer tf(window);

    sf::CircleShape circle(100.f, 70);
    circle.setPosition(tf.Transform(0 - circle.getRadius(), 0 - circle.getRadius()));
    circle.setFillColor(sf::Color::White);

    float p1 = 350.f;
    float p2 = 0.f;
    float dir1 = 0.29f;
    float dir2 = 1.f;

	std::array<Line, 400> rays;
    float offset = 0.01f;
    float YAngle = dir1 *-1;
	float XAngle = dir2;
    size_t valid = 0;
    float XPos = p1*-1;
	float YPos = p2;
    float lastValidAngle = XAngle;
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())
        {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			circle.setPosition({ (float)mousePos.x - circle.getRadius(), (float)mousePos.y - circle.getRadius() });
        }

        for (size_t i = 0; i < rays.size(); ++i)
        {
            if (i == 0)
            {
                YAngle = dir1 * -1;
                XAngle = dir2;
                XPos = p1 * -1;
                YPos = p2;
            }
            else if (i == rays.size() / 4)
            {
                YAngle = dir1 * -1;
                XAngle = dir2;
                XPos = p1;
                YPos = p2 * -1;
            }
            else if (i == rays.size() / 2)
            {
                YAngle = dir2;
                XAngle = dir1 * -1;
                XPos = p2;
                YPos = p1;
            }
            else if (i == (rays.size() / 4) * 3)
            {
                YAngle = dir2;
                XAngle = dir1 * -1;
                XPos = p2 * -1;
                YPos = p1 * -1;
            }

            Line line({ XPos, YPos }, { XAngle, YAngle });
            rays[i] = CalculateRay(line, circle.getRadius(), tf, tf.Normalize(circle.getPosition().x + circle.getRadius(), circle.getPosition().y + circle.getRadius()));
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
        std::cout << "Valid rays: " << valid << " | Last vali angle: " << lastValidAngle << '\r';
		valid = 0;

        window.clear(sf::Color(105,105,105,255));
        window.draw(circle);
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