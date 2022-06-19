#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "SFML/Graphics.hpp"

#include "Arial.h"

static inline constexpr float sg_TScalar = 10000.f;

struct Ray
{
private:
    static const sf::Color s_LightColor;
    static const sf::Color s_ShadowColor;
public:
    enum class Type { Light, Shadow, None };

    sf::Vector2f m_Origin;
    sf::Vector2f m_Intersection;
    Type m_Type = Type::None;

    inline Ray() = default;
    inline explicit Ray(const sf::Vector2f& origin) : m_Origin(origin) {}

    inline void Draw(sf::RenderWindow& window) const
    {
        if (m_Type == Type::Light)
        {
            const std::array<sf::Vertex, 2> line =
            {
                sf::Vertex(m_Origin, s_LightColor),
                sf::Vertex(m_Intersection, s_LightColor)
            };
            window.draw(&line[0], 2, sf::Lines);
        }
        else
        {
            const std::array<sf::Vertex, 2> line =
            {
                sf::Vertex(m_Origin, s_ShadowColor),
                sf::Vertex(m_Intersection, s_ShadowColor)
            };
            window.draw(&line[0], 2, sf::Lines);
        }
    }
};
const inline sf::Color Ray::s_LightColor = sf::Color(255, 255, 102);
const inline sf::Color Ray::s_ShadowColor = sf::Color(70, 70, 70);


struct RayPair
{
public:
    Ray light;
    Ray shadow;
    inline explicit RayPair(const sf::Vector2f& origin) : light(origin) {}
};


struct Text : public sf::Text
{
private:
    size_t m_Length = 0;
    float m_WindowXSize = 0.f;
    float m_YPos = 0.f;
    std::string m_Text;
private:
    inline void UpdatePosition()
    {
        size_t textLength = getString().getSize();
        if (textLength != m_Length)
        {
            SetPosition(m_YPos);
            m_Length = textLength;
        }
    }
public:
    inline void Init(const sf::Font& font, unsigned int windowXSize, float y, const std::string_view& text)
    {
        m_Length = text.size();
        m_WindowXSize = static_cast<float>(windowXSize);
        m_YPos = y;
        m_Text = text;

        setFont(font);
        setCharacterSize(20);
        setFillColor(sf::Color::White);
        setString(m_Text);
        SetPosition(m_YPos);
    }

    inline void SetPosition(float y)
    {
        sf::FloatRect rect = getLocalBounds();
        sf::Text::setPosition(m_WindowXSize - rect.width - 20.f, y);
    }

    inline void SetWindowXSize(unsigned int windowXSize) { m_WindowXSize = static_cast<float>(windowXSize); m_Length += 1; UpdatePosition(); }

    inline void Update(size_t validRays) { Update(std::to_string(validRays)); }
    inline void Update(const std::string& str)
    {
        setString(m_Text + str);
        UpdatePosition();
    }
};


struct DisplayTexts
{
public:
    template <class T>
    struct TextProperties
    {
    public:
        size_t textId = size_t();
        T value = T();
    };
private: // for generating texts
    sf::Font m_Font;
    unsigned int m_WindowSizeX;
    float m_YPos;
    float m_YOffset;
    size_t m_GeneratedTexts = 0;
    std::array<Text, 9> m_Texts;
    sf::RenderWindow& m_Window;

    const std::string onStr = "On";
    const std::string offStr = "Off";
    const std::string whiteStr = "White";
    const std::string blackStr = "Black";
public:
    TextProperties<size_t> fps;
    TextProperties<size_t> rays;
    TextProperties<size_t> lightRays;
    TextProperties<size_t> shadowRays;
    TextProperties<size_t> radius;
    TextProperties<bool> light;
    TextProperties<bool> shadow;
    TextProperties<bool> whiteTextColor;
    TextProperties<std::pair<unsigned int, std::string>> fpsLimit;
private:
    inline size_t GenerateText(const std::string& text)
    {
        m_Texts[m_GeneratedTexts].Init(m_Font, m_WindowSizeX, m_YPos, text);
        ++m_GeneratedTexts;
        m_YPos += m_YOffset;
        return m_GeneratedTexts - 1;
    }

    inline void UpdateText(size_t textId, const std::string& strTrue, const std::string& strFalse, bool decider)
    {
        if (decider)
            m_Texts[textId].Update(strTrue);
        else
            m_Texts[textId].Update(strFalse);
    }
public:
    inline DisplayTexts(unsigned int windowXSize, float yPos, float yOffset, sf::RenderWindow& window)
        : m_WindowSizeX(windowXSize), m_YPos(yPos), m_YOffset(yOffset), m_Window(window)
    {
        m_Font.loadFromMemory(sg_RawArialData, sg_RawArialDataRelativeSize);

        fps.textId = GenerateText("FPS: ");
        rays.textId = GenerateText("Rays: ");
        lightRays.textId = GenerateText("Light rays: ");
        shadowRays.textId = GenerateText("Shadow rays: ");
        radius.textId = GenerateText("Radius(Up,Down,->,<-): ");
        light.textId = GenerateText("Light(a): ");
        shadow.textId = GenerateText("Shadow(d): ");
        whiteTextColor.textId = GenerateText("Text color(e): ");
        fpsLimit.textId = GenerateText("FPS limit(w/s/f): ");

        rays.value = 0;
        lightRays.value = 0;
        shadowRays.value = 0;
        light.value = true;
        shadow.value = true;
        whiteTextColor.value = true;
        fpsLimit.value = std::make_pair(60, "60");
    }

    inline void DrawTexts() const
    {
        for (size_t i = 0; i < m_GeneratedTexts; ++i)
            m_Window.draw(m_Texts[i]);
    }

    inline void UpdateWindowSizeX(unsigned int windowXSize)
    {
        m_WindowSizeX = windowXSize;
        for (size_t i = 0; i < m_GeneratedTexts; ++i)
            m_Texts[i].SetWindowXSize(windowXSize);
    }

    inline void SetFillColor(const sf::Color& color)
    {
        for (size_t i = 0; i < m_GeneratedTexts; ++i)
            m_Texts[i].setFillColor(color);
    }

    inline void UpdateText(const TextProperties<size_t>& text)
    {
        m_Texts[text.textId].Update(text.value);
    }

    inline void Update()
    {
        rays.value = lightRays.value + shadowRays.value;
        UpdateText(rays);
        UpdateText(lightRays);
        UpdateText(shadowRays);
        UpdateText(radius);
        UpdateText(light.textId, onStr, offStr, light.value);
        UpdateText(shadow.textId, onStr, offStr, shadow.value);
        UpdateText(whiteTextColor.textId, whiteStr, blackStr, whiteTextColor.value);
        m_Texts[fpsLimit.textId].Update(fpsLimit.value.second);

        lightRays.value = 0;
        shadowRays.value = 0;
    }
};


struct LightSource : public sf::CircleShape
{
public:
    sf::Vector2f m_Direction;
    sf::Vector2f m_Origin;
    inline LightSource(float radius, size_t pointCount) : sf::CircleShape(radius, pointCount) {}

    inline void SetPosition(float x, float y)
    {
        sf::CircleShape::setPosition(x, y);
        m_Origin = { x + getRadius(), y + getRadius() };
    }
};



struct InputHandler
{
private:
    inline void DecreaseCircleRadius()
    {
        float radius = circle.getRadius();
        if (radius != 1.f)
        {
            radius -= 1.f;
            circle.setRadius(radius);
            texts.radius.value = static_cast<size_t>(radius);
            circleOrLightMoved = true;
        }
    }

    inline void IncreaseCircleRadius()
    {
        const float radius = circle.getRadius() + 1.f;
        circle.setRadius(radius);
        texts.radius.value = static_cast<size_t>(radius);
        circleOrLightMoved = true;
    }

    inline void UpdateFPSLimit()
    {
        window.setFramerateLimit(texts.fpsLimit.value.first);
        texts.fpsLimit.value.second = std::to_string(texts.fpsLimit.value.first);
    }

    inline void ToggleFPSLimit()
    {
        if (texts.fpsLimit.value.second == "Off")
        {
            UpdateFPSLimit();
        }
        else
        {
            window.setFramerateLimit(UINT_MAX);
            texts.fpsLimit.value.second = "Off";
        }
    }

    inline void UpdateTextColor()
    {
        texts.whiteTextColor.value = !texts.whiteTextColor.value;
        if (texts.whiteTextColor.value)
            texts.SetFillColor(sf::Color::White);
        else
            texts.SetFillColor(sf::Color::Black);
    }

    inline void ResizeViewPort(const sf::Event& event)
    {
        const sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
        window.setView(sf::View(visibleArea));
        texts.UpdateWindowSizeX(window.getSize().x);
    }

    inline void ToggleRays(DisplayTexts::TextProperties<bool>& ray)
    {
        ray.value = !ray.value;
        circleOrLightMoved = true;
    }

    inline void HandleEventInput()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized)
            {
                ResizeViewPort(event);
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                {
                    DecreaseCircleRadius();
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    IncreaseCircleRadius();
                }
                else if (event.key.code == sf::Keyboard::A)
                {
                    ToggleRays(texts.light);
                }
                else if (event.key.code == sf::Keyboard::D)
                {
                    ToggleRays(texts.shadow);
                }
                else if (event.key.code == sf::Keyboard::S)
                {
                    texts.fpsLimit.value.first = texts.fpsLimit.value.first == 1 ? 1 : texts.fpsLimit.value.first - 1;
                    UpdateFPSLimit();
                }
                else if (event.key.code == sf::Keyboard::W)
                {
                    texts.fpsLimit.value.first = texts.fpsLimit.value.first + 1;
                    UpdateFPSLimit();
                }
                else if (event.key.code == sf::Keyboard::F)
                {
                    ToggleFPSLimit();
                }
                else if (event.key.code == sf::Keyboard::E)
                {
                    UpdateTextColor();
                }
            }
        }
    }

    inline void HandleFrameInput()
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus())
        {
            const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            circle.setPosition(static_cast<float>(mousePos.x) - circle.getRadius(), static_cast<float>(mousePos.y) - circle.getRadius());
            circleOrLightMoved = true;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && window.hasFocus())
        {
            const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            lightSource.SetPosition(static_cast<float>(mousePos.x) - lightSource.getRadius(), static_cast<float>(mousePos.y) - lightSource.getRadius());
            circleOrLightMoved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && window.hasFocus())
        {
            IncreaseCircleRadius();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && window.hasFocus())
        {
            DecreaseCircleRadius();
        }
    }
public:
    sf::RenderWindow& window;
    DisplayTexts& texts;
    sf::CircleShape& circle;
    LightSource& lightSource;
    bool circleOrLightMoved = true;

    inline InputHandler(sf::RenderWindow& windowr, DisplayTexts& textsr, sf::CircleShape& circler, LightSource& lightSourcer)
        : window(windowr), texts(textsr), circle(circler), lightSource(lightSourcer) {}

    inline void HandleInput()
    {
        HandleEventInput();
        HandleFrameInput();
    }
};


inline void SetProperValues(Ray& light, Ray& shadow, const sf::Vector2f& origin, const sf::Vector2f& direction, float t1, float t2);
inline RayPair CalculateRays(const sf::Vector2f& origin, const sf::Vector2f& direction, float radius, const sf::Vector2f& circlePos);


inline void SetProperValues(Ray& light, Ray& shadow, const sf::Vector2f& origin, const sf::Vector2f& direction, float t1, float t2)
{
    const sf::Vector2f point1 = origin + direction * t1;
    const sf::Vector2f point2 = origin + direction * t2;

    const sf::Vector2f vec1 = point1 - origin;
    const sf::Vector2f vec2 = point2 - origin;

    const float length1 = vec1.x * vec1.x + vec1.y * vec1.y;
    const float length2 = vec2.x * vec2.x + vec2.y * vec2.y;

    if (length1 < length2)
    {
        shadow.m_Origin = point2;
        shadow.m_Intersection = shadow.m_Origin + direction * (t2 * sg_TScalar);
        light.m_Intersection = point1;
    }
    else
    {
        shadow.m_Origin = point1;
        shadow.m_Intersection = shadow.m_Origin + direction * (t1 * sg_TScalar);
        light.m_Intersection = point2;
    }
}


inline RayPair CalculateRays(const sf::Vector2f& origin, const sf::Vector2f& direction, float radius, const sf::Vector2f& circlePos)
{
    RayPair rays(origin);

    const float a = direction.x * direction.x + direction.y * direction.y;
    const float b = 2.f * origin.x * direction.x - 2.f * direction.x * circlePos.x + 2.f * origin.y * direction.y - 2.f * direction.y * circlePos.y;
    const float c = origin.x * origin.x - 2.f * origin.x * circlePos.x + circlePos.x * circlePos.x + origin.y * origin.y - 2.f * origin.y * circlePos.y + circlePos.y * circlePos.y - radius * radius;

    float discriminant = b * b - 4.f * a * c;
    if (discriminant < 0)
        return rays;

    const float denominator = 2.f * a;
    if (discriminant > 0)
    {
        discriminant = sqrtf(discriminant);
        const float t1 = (-b + discriminant) / denominator;
        const float t2 = (-b - discriminant) / denominator;
        SetProperValues(rays.light, rays.shadow, origin, direction, t1, t2);
    }
    else // discriminant == 0
    {
        const float t = -b / denominator;
        rays.light.m_Intersection = origin + direction * t;
        rays.shadow.m_Origin = rays.light.m_Intersection;
        rays.shadow.m_Intersection = rays.shadow.m_Origin + direction * (t * sg_TScalar); // arbitrary scalar
    }

    rays.light.m_Type = Ray::Type::Light;
    rays.shadow.m_Type = Ray::Type::Shadow;
    return rays;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 750), "Playing with rays");

    DisplayTexts texts(window.getSize().x, 0.f, 30.f, window);
    window.setFramerateLimit(texts.fpsLimit.value.first);

    LightSource lightSoure(20.f, 50);
    lightSoure.setFillColor(sf::Color(255, 255, 102));
    lightSoure.SetPosition(0.f, 0.f);

    sf::CircleShape circle(100.f, 70);
    circle.setPosition((static_cast<float>(window.getSize().x) / 2.f) - circle.getRadius(), (static_cast<float>(window.getSize().y) / 2.f) - circle.getRadius());
    circle.setFillColor(sf::Color::White);
    texts.radius.value = static_cast<size_t>(circle.getRadius());

    InputHandler ih(window, texts, circle, lightSoure);

    constexpr float YDir = 0.f;
    constexpr float XDir = 1000.f;
    constexpr float XOff = 0.228f; // 0.225f
    constexpr float YOff = 0.228f; // 0.225f

    float YOffset = YOff;
    float XOffset = XOff;
    lightSoure.m_Direction = sf::Vector2f(XDir, YDir);
    constexpr size_t numRays = 4450;
    std::vector<Ray> rays;
    rays.reserve(numRays * 2);

    const sf::Clock clock;
    sf::Time previousTime = clock.getElapsedTime();
    sf::Time currentTime;

    size_t lastLightRaysValue = 0;
    size_t lastShadowRaysValue = 0;

    const sf::Color backgroundColor(105, 105, 105, 255);

    while (window.isOpen())
    {
        ih.HandleInput();

        window.clear(backgroundColor);
        window.draw(lightSoure);
        window.draw(circle);

        if ((texts.light.value || texts.shadow.value) && ih.circleOrLightMoved)
        {
            ih.circleOrLightMoved = false;
            rays.clear();
            const sf::Vector2f circleRealPosition = circle.getPosition();
            const sf::Vector2f circlePosition(circleRealPosition.x + static_cast<float>(texts.radius.value), circleRealPosition.y + static_cast<float>(texts.radius.value));

            lightSoure.m_Direction.x = XDir;
            lightSoure.m_Direction.y = YDir;
            YOffset = YOff;
            XOffset = XOff;

            bool foundInArea = false;
            for (size_t j = 0; j < 2; ++j)
            {
                bool found = false;
                size_t foundIndex = 0;
                for (size_t i = 0; i < numRays; ++i)
                {
                    if ((lightSoure.m_Direction.x < 0.f && lightSoure.m_Direction.y > 750.f)
                        || (lightSoure.m_Direction.x < 0.f && lightSoure.m_Direction.y < 0.f))
                        break;

                    RayPair lines = CalculateRays(lightSoure.m_Origin, lightSoure.m_Direction, static_cast<float>(texts.radius.value), circlePosition);
                    if (lines.light.m_Type == Ray::Type::Light) // is only true if ray hits the circle
                    {
                        if (!found)
                            foundIndex = i;
                        found = true;

                        rays.emplace_back(lines.light);
                        rays.emplace_back(lines.shadow);
                        if (texts.light.value)
                        {
                            lines.light.Draw(window);
                            ++texts.lightRays.value;
                        }
                        if (texts.shadow.value)
                        {
                            lines.shadow.Draw(window);
                            ++texts.shadowRays.value;
                        }
                    }
                    else if (found)
                    {
                        if (foundIndex != 0)
                            foundInArea = true;
                        break;
                    }

                    lightSoure.m_Direction.y += YOffset;
                    lightSoure.m_Direction.x -= XOffset;
                }
                if (foundInArea)
                    break;

                lightSoure.m_Direction.x = XDir;
                lightSoure.m_Direction.y = YDir;
                YOffset = YOff * -1;
                XOffset = XOff;
            }

            lastLightRaysValue = texts.lightRays.value;
            lastShadowRaysValue = texts.shadowRays.value;
        }
        else
        {
            for (const Ray& ray : rays)
            {
                if (ray.m_Type == Ray::Type::Light && texts.light.value)
                {
                    ray.Draw(window);
                    texts.lightRays.value = lastLightRaysValue;
                }
                else if (ray.m_Type == Ray::Type::Shadow && texts.shadow.value)
                {
                    ray.Draw(window);
                    texts.shadowRays.value = lastShadowRaysValue;
                }
            }
        }

        texts.Update();
        texts.DrawTexts();
        window.display();

        currentTime = clock.getElapsedTime();
        texts.fps.value = static_cast<size_t>(1.f / (currentTime.asSeconds() - previousTime.asSeconds()));
        previousTime = currentTime;
        texts.UpdateText(texts.fps);
    }

    return 0;
}