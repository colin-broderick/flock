#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <set>

#include <SFML/Graphics.hpp>

#include <linked_list.hpp>

constexpr int GRID_SIZE = 40;

float GLOBAL_FACTOR = 2.0;

constexpr int bird_count = 1000;

float max_boid_speed = 2.0 / GLOBAL_FACTOR;

float alignment_strength = 1.0 / GLOBAL_FACTOR;
float alignment_distance_sq = 1.0 / GLOBAL_FACTOR;

float cohesion_strength = 1.0 / GLOBAL_FACTOR;
float cohesion_distance_sq = 1.0 / GLOBAL_FACTOR;

float avoidance_strength = 1.0 / GLOBAL_FACTOR;
float avoidance_distance_sq = 1.0 / GLOBAL_FACTOR;

struct Vector2;
std::ostream& operator<<(std::ostream& stream, Vector2& v);
template <class T>
Vector2 operator*(const Vector2& vector, const T& scalar);

const int GRID_SCALE_TO_PIXEL_SCALE = 20;

struct Vector2
{
    float x, y;

    static float distance_between(const Vector2& v1, const Vector2& v2)
    {
        return std::sqrt((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
    }

    static float sqdistance(const Vector2& v1, const Vector2& v2)
    {
        return (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y);
    }

    float magnitude()
    {
        return std::sqrt(x*x + y*y);
    }

    Vector2 normalize()
    {
        if (magnitude() > 0)
        {
            return *this * (1.0/magnitude());
        }
        return {0, 0};
    }
};

static inline Vector2 CENTRE = {GRID_SIZE/2.0, GRID_SIZE/2.0};

template <class T>
Vector2 operator*(const Vector2& vector, const T& scalar)
{
    return {vector.x * scalar, vector.y * scalar};
}

Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
    return {
        v1.x + v2.x,
        v1.y + v2.y
    };
}

Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
    return {
        v1.x - v2.x,
        v1.y - v2.y
    };
}

struct Boid
{
    Vector2 position;
    Vector2 velocity{0, 0};
    int previous_hash;
    Vector2 acceleration{0, 0};

    Boid(Vector2 position_, Vector2 velocity_)
    : position{position_}, velocity{velocity_}, previous_hash{position_hash(position)}
    {
    }

    static int position_hash(Vector2 position)
    {
        return static_cast<int>(position.x) * GRID_SIZE + static_cast<int>(position.y);
    }
};

void print(LinkedList<int>::Node* head)
{
    auto temp = head;
    while (temp != nullptr)
    {
        std::cout << temp->data << '\n';
        temp = temp->next;
    }
}

std::set<int> get_neighbours(int spatial_hash)
{
    // In 2D, get the eight surrounding neighbour hashes in a vector.
    // Must account for the walls of the grid, so that a cell near the edge
    // has fewer neighbours (5 or 3).
    
    std::set<int> results;

    results.insert(spatial_hash);
    int x = spatial_hash / GRID_SIZE;
    int y = spatial_hash % GRID_SIZE;

    int x_c, y_c;

    x_c = x; y_c = y - 1;
    if (!(y_c < 0))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }
    x_c = x; y_c = y + 1;
    if (!(y_c >= GRID_SIZE))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }

    x_c = x - 1; y_c = y - 1;
    if (!(x_c < 0 || y_c < 0))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }

    x_c = x - 1; y_c = y;
    if (!(x_c < 0))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }

    x_c = x - 1; y_c = y + 1;
    if (!(x_c < 0 || y_c >= GRID_SIZE))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }

    x_c = x + 1; y_c = y - 1;
    if (!(x_c >= GRID_SIZE || y_c < 0))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }
    
    x_c = x + 1; y_c = y;
    if (!(x_c >= GRID_SIZE))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }
    
    x_c = x + 1; y_c = y + 1;
    if (!(x_c >= GRID_SIZE || y_c >= GRID_SIZE))
    {
        results.insert(Boid::position_hash(Vector2{static_cast<float>(x_c), static_cast<float>(y_c)}));
    }

    return results;
}

std::ostream& operator<<(std::ostream& stream, std::set<int>& set)
{
    for (auto i : set)
    {
        stream << i << ", ";
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Vector2& v)
{
    stream << '(' << v.x << ", " << v.y << ')';
    return stream;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(GRID_SCALE_TO_PIXEL_SCALE * GRID_SIZE, GRID_SCALE_TO_PIXEL_SCALE * GRID_SIZE), "Boids");
    window.setFramerateLimit(60);

    std::array<LinkedList<Boid>, GRID_SIZE * GRID_SIZE> space;

    for (auto i = 0; i < bird_count; i++)
    {
        Boid b1{
            {
                (rand() / ((double)INT32_MAX))*GRID_SIZE,
                20
            },
            {
                (rand() / (double)INT32_MAX-0.5)*0.1,
                (rand() / (double)INT32_MAX-0.5)*0.1
            }
        };
        space[Boid::position_hash(b1.position)].push_front({b1});
    }

    std::array<std::set<int>, GRID_SIZE * GRID_SIZE> neighbour_list;
    for (std::size_t i = 0; i < neighbour_list.size(); i++)
    {
        neighbour_list[i] = get_neighbours(i);
    }

    sf::CircleShape circle(2.0f);
    circle.setFillColor(sf::Color::Green);
    sf::RectangleShape vertibar(sf::Vector2f(1.0, GRID_SIZE * GRID_SCALE_TO_PIXEL_SCALE));
    sf::RectangleShape horizbar(sf::Vector2f(GRID_SIZE * GRID_SCALE_TO_PIXEL_SCALE, 1.0));
    vertibar.setFillColor(sf::Color::Red);
    horizbar.setFillColor(sf::Color::Red);

    sf::CircleShape centre_circle(2.0);
    centre_circle.setFillColor(sf::Color::Red);
    centre_circle.setPosition(CENTRE.x * GRID_SCALE_TO_PIXEL_SCALE, CENTRE.y * GRID_SCALE_TO_PIXEL_SCALE);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        // Draw grid.
        // for (int i = 0; i < GRID_SIZE; i++)
        // {
        //     vertibar.setPosition(i * GRID_SCALE_TO_PIXEL_SCALE, 0);
        //     horizbar.setPosition(0, i * GRID_SCALE_TO_PIXEL_SCALE);
        //     window.draw(vertibar);
        //     window.draw(horizbar);
        // }

        for (auto& list : space)
        {
            auto boid1 = list.head;
            while (boid1)
            {
                boid1->data.acceleration = {0, 0};
                auto neighbour_cells = neighbour_list[boid1->data.previous_hash];
                int cohere_count = 0;
                int avoid_count = 0;
                int align_count = 0;
                Vector2 cohere_direction{0, 0};
                Vector2 align_acceleration{0, 0};
                Vector2 avoid_acceleration{0, 0};
                
                for (auto& cell : neighbour_cells)
                {
                    auto boid2 = space[cell].head;
                    while (boid2)
                    {
                        if (boid2 == boid1)
                        {
                            boid2 = boid2->next;
                            continue;
                        }

                        float sqdistance = Vector2::sqdistance(boid1->data.position, boid2->data.position);

                        // Cohesion
                        if (sqdistance < cohesion_distance_sq)
                        {
                            cohere_count++;
                            cohere_direction = cohere_direction + boid2->data.position;
                        }

                        // Avoidance
                        if (sqdistance < avoidance_distance_sq)
                        {
                            avoid_count++;
                            avoid_acceleration = avoid_acceleration + (boid1->data.position - boid2->data.position) * (avoidance_strength / sqdistance);
                        }

                        // Alignment
                        if (sqdistance < alignment_distance_sq)
                        {
                            align_count++;
                            align_acceleration = align_acceleration + boid2->data.velocity;
                        }

                        boid2 = boid2->next;
                    }
                }
                if (cohere_count != 0)
                {
                    cohere_direction = (cohere_direction * (1.0 / (double)cohere_count) - boid1->data.position) * cohesion_strength;
                    boid1->data.acceleration = boid1->data.acceleration + cohere_direction;
                }

                if (avoid_count != 0)
                {
                    avoid_acceleration = avoid_acceleration * (1.0 / (double)avoid_count);
                    boid1->data.acceleration = boid1->data.acceleration + avoid_acceleration;
                }

                if (align_count != 0)
                {
                    align_acceleration = align_acceleration * (1.0 / (double)align_count) * alignment_strength;
                    boid1->data.acceleration = boid1->data.acceleration + align_acceleration;
                }

                // Avoid edge
                auto sqdistance = Vector2::sqdistance(boid1->data.position, CENTRE);
                if (sqdistance > 400)
                {
                    boid1->data.acceleration = boid1->data.acceleration + (CENTRE - boid1->data.position)*(std::sqrt(sqdistance) - 20)*0.01;
                }

                // Avoid mouse
                auto mousepossf = sf::Mouse::getPosition(window);
                Vector2 mouseV{mousepossf.x / GRID_SCALE_TO_PIXEL_SCALE, mousepossf.y / GRID_SCALE_TO_PIXEL_SCALE};
                auto sqmousedistance = Vector2::sqdistance(boid1->data.position, mouseV);
                if (sqmousedistance < 100)
                {
                    boid1->data.acceleration = boid1->data.acceleration + (mouseV - boid1->data.position)*(std::sqrt(sqmousedistance) - 32)*0.1;
                }

                boid1 = boid1->next;
            }
        }

        for (auto& list : space)
        {
            auto boid = list.head;
            while (boid)
            {
                boid->data.velocity = boid->data.velocity + boid->data.acceleration * 0.1;
                boid->data.velocity = boid->data.velocity.normalize() * max_boid_speed;
                boid->data.position = boid->data.position + boid->data.velocity * 0.1;

                while (boid->data.position.x > GRID_SIZE)
                {
                    boid->data.position.x -= GRID_SIZE;
                }
                while (boid->data.position.x < 0)
                {
                    boid->data.position.x += GRID_SIZE;
                }
                while (boid->data.position.y < 0)
                {
                    boid->data.position.y += GRID_SIZE;
                }
                while (boid->data.position.y > GRID_SIZE)
                {
                    boid->data.position.y -= GRID_SIZE;
                }

                circle.setPosition(boid->data.position.x * GRID_SCALE_TO_PIXEL_SCALE, boid->data.position.y * GRID_SCALE_TO_PIXEL_SCALE);
                // text.setString(std::to_string(boid->data.previous_hash));
                // text.setPosition(boid->data.position.x * GRID_SCALE_TO_PIXEL_SCALE, boid->data.position.y * GRID_SCALE_TO_PIXEL_SCALE);
                // window.draw(text);
                window.draw(circle);

                auto h = Boid::position_hash(boid->data.position);
                if (h < 0 || h > GRID_SIZE * GRID_SIZE - 1)
                {
                    std::cout << "Emergency!!!! " << h << " " << boid->data.position.x << " " << boid->data.position.y << "\n";
                    h = 0;
                }
                if (h != boid->data.previous_hash)
                {
                    auto b = *boid;
                    space[boid->data.previous_hash].remove(boid);
                    space[h].push_front(b.data)->data.previous_hash = h;

                }
                boid = boid->next;
            }
        }

        window.draw(centre_circle);

        window.display();
    }

    return 0;
}