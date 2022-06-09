#include <iostream>
#include <vector>
#include <string>

#include "SFML/Graphics.hpp"

#include "Arial.h"

struct Line
{
public:
    enum class Type { Light, Shadow, Transparent, None };

    sf::Vector2f m_Origin, m_Direction;
    float t = 1.f;
    bool m_IsValid = false;
	Type m_Type = Type::None;
	sf::Color m_DrawColor;
	
    Line() = default;
    Line(const sf::Vector2f& origin, const sf::Vector2f& direction)
        : m_Origin(origin), m_Direction(direction) {}

	void SetTransparent() { m_DrawColor = sf::Color::Transparent; m_Type = Type::Transparent; }
    void SetLight() { m_DrawColor = sf::Color(255, 255, 102); m_Type = Type::Light; }
	void SetDark() { m_DrawColor = sf::Color(70, 70, 70); m_Type = Type::Shadow; }

    void Draw(sf::RenderWindow& window)
    {
        if (!m_IsValid || m_Type == Type::None || m_Type == Type::Transparent)
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


struct Text : public sf::Text
{
private:
    size_t m_Length = 0;
    float m_WindowXSize;
    float m_YPos;
	const std::string m_Text;
public:
    void setPosition(float y)
    {
        sf::FloatRect rect = getLocalBounds();
        sf::Text::setPosition(m_WindowXSize - rect.width - 20.f, y);
    }
public:
    Text(const sf::Font& font, unsigned int windowXSize, float y, const std::string& text)
        : m_WindowXSize(static_cast<float>(windowXSize)), m_YPos(y), m_Text(text)
    {
        setFont(font);
        setCharacterSize(20);
        setFillColor(sf::Color::White);
        setString(m_Text);
        m_Length = getString().getSize();
        setPosition(m_YPos);
    }

    void SetWindowXSize(unsigned int windowXSize) { m_WindowXSize = static_cast<float>(windowXSize); m_Length += 1; }

    void UpdatePosition(size_t validRays)
    {
        UpdatePosition(std::to_string(validRays));
    }

    void UpdatePosition(const std::string& str)
    {
        setString(m_Text + str);

        if (getString().getSize() != m_Length)
        {
            setPosition(m_YPos);
            m_Length = getString().getSize();
        }
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
    size_t framerateLimit = 60;
    std::string framerateLimitStr = std::to_string(framerateLimit);
    window.setFramerateLimit(framerateLimit);

    sf::Font font;
    font.loadFromMemory(sg_RawArialData, sg_RawArialDataRelativeSize);

    float yPosText = 0.f;
	const float yPosTextOffset = 30.f;

    Text fpsText(font, window.getSize().x, yPosText, "FPS: ");
	Text raysText(font, window.getSize().x, yPosText += yPosTextOffset, "Valid rays: ");
    Text lightRaysText(font, window.getSize().x, yPosText += yPosTextOffset, "Valid light rays: ");
    Text shadowRaysText(font, window.getSize().x, yPosText += yPosTextOffset, "Valid shadow rays: ");
    Text radiusText(font, window.getSize().x, yPosText += yPosTextOffset, "Radius(Up,Down,->,<-): ");
    Text lightText(font, window.getSize().x, yPosText += yPosTextOffset, "Light(a): ");
	Text shadowText(font, window.getSize().x, yPosText += yPosTextOffset, "Shadow(d): ");
    Text textColorText(font, window.getSize().x, yPosText += yPosTextOffset, "Text color(e): ");
    Text framerateLimitText(font, window.getSize().x, yPosText += yPosTextOffset, "Framerate limit(w/s/f): ");

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
	
	bool showLight = true;
	bool showShadow = true;
    bool whiteTextColor = true;

    float fps = 0.f;
    const sf::Clock clock = sf::Clock::Clock();
    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;
	
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
				fpsText.SetWindowXSize(window.getSize().x);
                raysText.SetWindowXSize(window.getSize().x);
				lightRaysText.SetWindowXSize(window.getSize().x);
				shadowRaysText.SetWindowXSize(window.getSize().x);
				radiusText.SetWindowXSize(window.getSize().x);
				lightText.SetWindowXSize(window.getSize().x);
				shadowText.SetWindowXSize(window.getSize().x);
                textColorText.SetWindowXSize(window.getSize().x);
				framerateLimitText.SetWindowXSize(window.getSize().x);
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
                    showLight = !showLight;
				else if (event.key.code == sf::Keyboard::D)
					showShadow = !showShadow;
                else if (event.key.code == sf::Keyboard::S) 
                {
					framerateLimit = framerateLimit == 1 ? 1 : framerateLimit - 1;
					window.setFramerateLimit(framerateLimit);
                    framerateLimitStr = std::to_string(framerateLimit);
                }
                else if (event.key.code == sf::Keyboard::W)
                {
					framerateLimit = framerateLimit + 1;
					window.setFramerateLimit(framerateLimit);
                    framerateLimitStr = std::to_string(framerateLimit);
                }
                else if (event.key.code == sf::Keyboard::F)
				{
                    window.setFramerateLimit(SIZE_MAX);
                    framerateLimitStr = "Off";
				}
                else if (event.key.code == sf::Keyboard::E)
                {
					whiteTextColor = !whiteTextColor;
                    if (whiteTextColor)
                    {
                        fpsText.setFillColor(sf::Color::White);
                        raysText.setFillColor(sf::Color::White);
                        lightRaysText.setFillColor(sf::Color::White);
						shadowRaysText.setFillColor(sf::Color::White);
						radiusText.setFillColor(sf::Color::White);
						lightText.setFillColor(sf::Color::White);
						shadowText.setFillColor(sf::Color::White);
						textColorText.setFillColor(sf::Color::White);
						framerateLimitText.setFillColor(sf::Color::White);
                    }
                    else
                    {
                        fpsText.setFillColor(sf::Color::Black);
                        raysText.setFillColor(sf::Color::Black);
                        lightRaysText.setFillColor(sf::Color::Black);
                        shadowRaysText.setFillColor(sf::Color::Black);
                        radiusText.setFillColor(sf::Color::Black);
                        lightText.setFillColor(sf::Color::Black);
                        shadowText.setFillColor(sf::Color::Black);
						textColorText.setFillColor(sf::Color::Black);
                        framerateLimitText.setFillColor(sf::Color::Black);
                    }
                }
            }
        }
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())
        {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            circle.setPosition({ static_cast<float>(mousePos.x) - circle.getRadius(), static_cast<float>(mousePos.y) - circle.getRadius()});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && window.hasFocus())
        {
			circle.setRadius(circle.getRadius() + 1.f);
        }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && window.hasFocus())
		{
			if(circle.getRadius() != 1.f)
			    circle.setRadius(circle.getRadius() - 1.f);
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
        raysText.UpdatePosition(rays.size());
		lightRaysText.UpdatePosition(static_cast<size_t>(rays.size()) / 2);
		shadowRaysText.UpdatePosition(static_cast<size_t>(rays.size()) / 2);
        radiusText.UpdatePosition(static_cast<size_t>(circle.getRadius()));
        framerateLimitText.UpdatePosition(framerateLimitStr);
		if(showLight)
		    lightText.UpdatePosition("On");
		else
			lightText.UpdatePosition("Off");
		if(showShadow)
			shadowText.UpdatePosition("On");
		else
			shadowText.UpdatePosition("Off");
		
        if(whiteTextColor)
			textColorText.UpdatePosition("White");
		else
			textColorText.UpdatePosition("Black");
		
		YAngle = dir1;
		offset = 0.001f;
		
		
        if (!showLight || !showShadow)
        {
            for (Line& ray : rays)
            {
                if (ray.m_Type == Line::Type::Light && !showLight)
                    ray.SetTransparent();
				else if (ray.m_Type == Line::Type::Shadow && !showShadow)
					ray.SetTransparent();
            }
        }
		
        window.clear(sf::Color(105,105,105,255));
        window.draw(circle);
        for (size_t i = 0; i < rays.size(); ++i)
        {
            rays.at(i).Draw(window);
        }
        window.draw(fpsText);
        window.draw(raysText);
        window.draw(lightRaysText);
		window.draw(shadowRaysText);
		window.draw(radiusText);
		window.draw(lightText);
		window.draw(shadowText);
        window.draw(textColorText);
        window.draw(framerateLimitText);
        window.display();
		rays.clear();

        currentTime = clock.getElapsedTime();
        fps = 1.f / (currentTime.asSeconds() - previousTime.asSeconds());
        previousTime = currentTime;
        fpsText.UpdatePosition(static_cast<size_t>(fps));
    }

    return 0;
}