#include <iostream>
#include "window.h"
#include "grid.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"

#include <chrono>
#include <queue>
#include <unordered_set>
#include <algorithm>

float lastMoveTime = 0.0f;
float moveInterval = 10.0f;

struct Player
{
    float health = 100.0f;
    float maxHealth = 100.0f;
    int m_score;
    glm::vec3 m_position;
    glm::vec3 m_goal = glm::vec3(0.0f, 0.0f, 0.0f);
    float m_speed = 2.0f;
    float m_size;
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    std::queue<glm::vec3> m_path;

    Player(glm::vec3 pos = glm::vec3(0.0f), float s = 1.0f)
        : m_position(pos)
        , m_size(s)
    {}

    void create()
    {
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);

        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));
        glEnableVertexAttribArray(3);
    }

    void draw()
    {
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }

    void update(float dt)
    {
        if (m_path.empty())
            return;

        glm::vec3 dir = m_goal - m_position;
        float dist = glm::length(dir);
        if (dist < 0.05f)
        {
            m_path.pop();
            if (!m_path.empty())
            {
                m_goal = m_path.front();
            }
            return;
        }
        m_position += glm::normalize(dir) * dt * m_speed;
    }
};

struct Item
{
    glm::vec3 m_position;
    float m_rotation = 0.0f;
    float m_size;
    float m_pickupRadius = 0.5f;
    bool m_collected = false;
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;


    Item(glm::vec3 pos = glm::vec3(0.0f), float s = 1.0f)
        : m_position(pos)
        , m_size(s)
    {}

    void create()
    {
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glGenBuffers(1, &cubeEBO);

        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));
        glEnableVertexAttribArray(3);
    }

    void draw()
    {
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    }

    void update(float dt)
    {
    }
};

struct SpotLight
{
    glm::vec3 m_position;
    glm::vec3 m_direction = glm::vec3(0.0, -1.0, 0.0);
    glm::vec3 m_color;
    float m_cutoff = glm::cos(glm::radians(25.0f));

    glm::vec3 targetPos;
    float moveStartTime;
};


struct Node
{
    int x;
    int z;
    float gCost = 0.0f;
    float hCost = 0.0f;
    float fCost = gCost + hCost;
    Node* parent = nullptr;

    Node(int x, int z)
        : x(x)
        , z(z) {}
};

namespace a_Star
{
    struct NodeCompare
    {
        bool operator()(const Node* a, const Node* b) const
        {
            return a->fCost > b->fCost;
        }
    };

    float heuristic(const Node& a, const Node& b)
    {
        return abs(a.x - b.x) + abs(a.z - b.z);
    }

    std::vector<glm::vec2> findPath(Grid& grid, glm::vec2 start, glm::vec2 goal)
    {
        std::vector<Node*> openList;
        std::vector<Node*> closedList;

        Node* startNode = new Node(start.x, start.y);
        Node* goalNode = new Node(goal.x, goal.y);
        openList.push_back(startNode);

        auto nodeExists = [](std::vector<Node*>& list, int x, int z) {
            return std::any_of(list.begin(), list.end(), [&](Node* n) {
                return n->x == x && n->z == z;
                });
            };

        while (!openList.empty())
        {
            auto currentIt = std::min_element(openList.begin(), openList.end(),
                [](Node* a, Node* b) {return a->fCost < b->fCost; });

            Node* current = *currentIt;
            openList.erase(currentIt);
            closedList.push_back(current);

            if (current->x == goalNode->x && current->z == goalNode->z)
            {
                std::vector<glm::vec2> path;
                Node* temp = current;
                while (temp)
                {
                    path.push_back(glm::vec2(temp->x, temp->z));
                    temp = temp->parent;
                }
                std::reverse(path.begin(), path.end());
                std::cout << "size of path: " << path.size();
                return path;
            }
            std::vector<glm::ivec2> directions = { {1,0},{-1,0},{0,1},{0,-1} };
            for (auto& dir : directions)
            {
                int nx = current->x + dir.x;
                int nz = current->z + dir.y;

                if (nx < 0 || nz < 0 || nx >= grid.getSize() || nz >= grid.getSize())
                    continue;
                if (nodeExists(closedList, nx, nz))
                    continue;
                if (grid.wall(nx, nz))
                    continue;

                Node* neighbor = new Node(nx, nz);
                neighbor->gCost = current->gCost + 1;
                neighbor->hCost = heuristic(*neighbor, *goalNode);
                neighbor->parent = current;

                if (!nodeExists(openList, nx, nz))
                {
                    openList.push_back(neighbor);
                }
            }
        }
        return {};
    }
}

Item spawnItem(Grid& grid)
{
    int size = grid.getSize();

    while (true)
    {
        int x = rand() % size;
        int z = rand() % size;

        if (!grid.wall(x, z))
        {
            glm::vec3 pos = grid.getTileWorldPos(x, z);
            pos.y = 0.5f / 2.0f;
            return Item(pos);
        }
    }
}

void updateSpotLights(float curTime, float dt, std::vector<SpotLight>& lights, Grid& grid)
{
    if (curTime - lastMoveTime > moveInterval)
    {
        lastMoveTime = curTime;
        for (int i = 1; i < lights.size(); i++)
        {
            glm::ivec2 tile = grid.getWalkableTile();
            glm::vec3 pos = grid.getTileWorldPos(tile.x, tile.y);
            pos.y = 8.0f;
            lights[i].targetPos = pos;
            lights[i].moveStartTime = curTime;
            lights[i].m_direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
        }
    }

    for (int i = 1; i < lights.size(); i++)
    {
        float t = (curTime - lights[i].moveStartTime) / 1.0f; // 1.0 is duration.
        t = glm::clamp(t, 0.0f, 1.0f);

        lights[i].m_position = glm::mix(lights[i].m_position, lights[i].targetPos, t);
    }

}

float SpotlightInfluenceToPlayer(const SpotLight& light, const glm::vec3& position)
{
    glm::vec3 lightDir = glm::normalize(light.m_position - position);
    float diff = glm::max(glm::dot(glm::vec3(0.0, 1.0, 0.0), lightDir), 0.0f);
    float theta = glm::dot(lightDir, glm::normalize(-light.m_direction));
    float epsilon = 0.1f;
    float intensity = glm::smoothstep(light.m_cutoff, light.m_cutoff + epsilon, theta);

    return diff * intensity;
}


int main()
{
    Window window(600, 600, "test");
    Shader shader("assets/shaders/grid.vert", "assets/shaders/grid.frag");
    Shader menuAndEndShader("assets/shaders/menuAndEnd.vert", "assets/shaders/menuAndEnd.frag");
    Shader billboard("assets/shaders/billboard.vert", "assets/shaders/billboard.frag");
    Camera camera(window.width(), window.height(), glm::vec3(5.0f));
    window.setCamera(&camera);
    std::vector<SpotLight> spotlights;
    Player player;

    Grid grid(0.5f);
    window.setGrid(&grid);

    player.m_position = grid.getTileWorldPos(0, 0);
    player.m_goal = grid.getTileWorldPos(0, 0);

    //static light, always at the start position
    SpotLight light;
    light.m_position = grid.getTileWorldPos(0, 0) + glm::vec3(0.0, 8.0f, 0.0);
    light.m_color = glm::vec3(1.0f, 1.0f, 0.3f);
    light.targetPos = light.m_position;
    light.moveStartTime = glfwGetTime();
    spotlights.push_back(light);

    window.setTileCallback([&](int x, int z) {
        glm::vec2 start = grid.getTileIndex(player.m_position);
        glm::vec2 goal = glm::vec2(x, z);

        std::vector<glm::vec2> path = a_Star::findPath(grid, start, goal);

        std::queue<glm::vec3> pathW;
        for (auto& tile : path)
        {
            glm::vec3 world = grid.getTileWorldPos(tile.x, tile.y);
            pathW.push(world);
        }

        if (!pathW.empty())
        {
            player.m_path = pathW;
            player.m_goal = player.m_path.front();
        }
        });

    unsigned int gridTex = loadTexture("assets/textures/ground.png", false);
    unsigned int wallTex = loadTexture("assets/textures/wall.png", false);
    unsigned int playerTex = loadTexture("assets/models/gnomeTextures/albedo.jpg", false);
    unsigned int mainmenu = loadTexture("assets/textures/mainmenu.png", true);
    unsigned int end = loadTexture("assets/textures/end.png", true);

    bool success = MODEL_LOADING::loadModel("assets/models/gnome.fbx", player.m_vertices, player.m_indices);
    player.create();
    success = MODEL_LOADING::loadModel("assets/models/wall.fbx", grid.getWallVerts(), grid.getWallIndices());
    grid.create();

    srand(time(NULL));
    std::vector<Item> items;
    for (int i = 0; i < 5; i++)
    {
        Item item = spawnItem(grid);
        success = MODEL_LOADING::loadModel("assets/models/wall.fbx", item.m_vertices, item.m_indices);
        item.create();
        items.push_back(item);
    }

    for (int i = 1; i < 3; i++)
    {
        SpotLight light;
        glm::vec2 tile = grid.getWalkableTile();
        light.m_position = grid.getTileWorldPos(tile.x, tile.y) + glm::vec3(0.0, 8.0f, 0.0);
        light.m_color = glm::vec3(1.0f, 1.0f, 0.3);
        light.targetPos = light.m_position;
        light.moveStartTime = glfwGetTime();
        spotlights.push_back(light);
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    while (!window.isWindowClosed())
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(startTime - currentTime).count();
        currentTime = startTime;
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        if (player.health <= 0.0f)
        {
            grid.setState(Grid::GameState::END);
        }
        if (grid.state() == Grid::GameState::MENU)
        {
            menuAndEndShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mainmenu);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            window.pollEvents();
            window.swapBuffers();

            if (glfwGetKey(window.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS)
            {
                grid.setState(Grid::GameState::PLAYING);
            }
            continue;
        }
        else if (grid.state() == Grid::GameState::END)
        {
            if (glfwGetKey(window.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                window.setWindowShouldClose(true);
            }
            menuAndEndShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, end);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            window.pollEvents();
            window.swapBuffers();
            continue;

        }

        camera.update();


        float total = 0.0f;
        for (int i = 0; i < 3; i++)
        {
            total += SpotlightInfluenceToPlayer(spotlights[i], player.m_position);
        }

        if (total > 0.2f)
        {
            player.health += dt * 5.0f;
        }
        else
        {
            player.health -= dt * 5.0f;
        }
        player.health = glm::clamp(player.health, 0.0f, 100.0f);
        
        /* draw grid */
        shader.use();
        shader.setUniformInt("UV", 0);
        shader.setUniformInt("isGlowing", false);
        shader.setUniformMat4("model", camera.model());
        shader.setUniformMat4("view", camera.view());
        shader.setUniformMat4("proj", camera.proj());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gridTex);
        grid.draw();

        //walls
        for (int z = 0; z < grid.getSize(); z++)
        {
            for (int x = 0; x < grid.getSize(); x++)
            {
                if (grid.wall(x, z))
                {
                    glm::vec3 pos = grid.getTileWorldPos(x, z);
                    pos.y = 0.5f / 2.0f;

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, pos);
                    model = glm::scale(model, glm::vec3(0.25));

                    shader.use();
                    shader.setUniformInt("UV", 0);
                    shader.setUniformInt("isGlowing", false);
                    shader.setUniformMat4("model", model);
                    shader.setUniformMat4("view", camera.view());
                    shader.setUniformMat4("proj", camera.proj());
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, wallTex);
                    grid.drawWall();
                }
            }
        }

        // collectable cubes
        static float elapsedTime = 0.0f;
        elapsedTime += dt;
        for (auto& item : items)
        {
            float distBetweenPlayerAndItem = glm::distance(player.m_position, item.m_position);
            if (distBetweenPlayerAndItem < item.m_pickupRadius)
            {
                player.m_score += 1;
                glm::vec2 pos = grid.getWalkableTile();
                item.m_position = grid.getTileWorldPos(pos.x, pos.y);
            }
            item.m_rotation += 90.0f * dt;
            if (item.m_rotation > 360.0f)
                item.m_rotation -= 360.0f;

            glm::mat4 itemModel = glm::mat4(1.0f);
            item.m_position.y = sin(elapsedTime * 3.0f) * 0.1f;
            itemModel = glm::translate(itemModel, item.m_position);
            itemModel = glm::rotate(itemModel, glm::radians(item.m_rotation), glm::vec3(0.0, 1.0, 0.0));
            itemModel = glm::scale(itemModel, glm::vec3(0.25f));

            shader.use();
            shader.setUniformInt("isGlowing", true);
            shader.setUniformInt("useTexture", false);
            shader.setUniformFloat("t", elapsedTime);
            shader.setUniformMat4("model", itemModel);
            shader.setUniformMat4("view", camera.view());
            shader.setUniformMat4("proj", camera.proj());
            item.draw();
        }

        /* draw ai what user will control*/
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, player.m_position);
        model = glm::rotate(model,glm::radians(-90.0f), {1.0, 0.0, 0.0});
        model = glm::scale(model, glm::vec3(player.m_size));

        shader.use();
        shader.setUniformInt("UV", 0);
        shader.setUniformInt("isGlowing", false);
        shader.setUniformMat4("model", model);
        shader.setUniformMat4("view", camera.view());
        shader.setUniformMat4("proj", camera.proj());
        player.update(dt);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, playerTex);
        player.draw();

        //BillBoard
        billboard.use();
        billboard.setUniformMat4("view", camera.view());
        billboard.setUniformMat4("proj", camera.proj());
        billboard.setUniformVec3("position", player.m_position);
        billboard.setUniformFloat("percent", player.health / player.maxHealth);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //light stuff
        updateSpotLights(glfwGetTime(), dt, spotlights, grid);
        shader.use();
        shader.setUniformInt("spotCount", 3);
        shader.setUniformVec3("ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        for (int i = 0; i < 3; i++)
        {
            shader.setUniformVec3("spotlights[" + std::to_string(i) + "].position", spotlights[i].m_position);
            shader.setUniformVec3("spotlights[" + std::to_string(i) + "].direction", spotlights[i].m_direction);
            shader.setUniformVec3("spotlights[" + std::to_string(i) + "].color", spotlights[i].m_color);
            shader.setUniformFloat("spotlights[" + std::to_string(i) + "].cutoff", spotlights[i].m_cutoff);
        }

        window.pollEvents();
        window.swapBuffers();
    }
}
