#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "cubeVerts.h"

//struct vertex
//{
//    glm::vec3 position;
//    glm::vec3 color;
//    glm::vec2 texCoord;
//};

class Grid
{
public:
    enum class GameState
    {
        MENU,
        PLAYING,
        END
    };
   
    Grid(float tileSize);
    ~Grid();
    bool loadFromFile(const std::string& path);
    void generateGrid(std::vector<vertex>& vertices, std::vector<unsigned int>& indices, int size);
    void create();
    glm::vec3 getTileWorldPos(int x, int z);
    glm::vec2 getTileIndex(glm::vec3& wPos);
    glm::vec2 getWalkableTile();
    int getSize();
    void setState(GameState state);
    GameState state() const { return m_state; }
    std::vector<vertex>& getWallVerts();
    std::vector<unsigned int>& getWallIndices();
    bool wall(int x, int z);
    void setWall(int x, int z, bool value);
    void draw();
    void drawWall();
private:
    std::vector<vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<vertex> m_wallVertices;
    std::vector<unsigned int> m_wallIndices;
    unsigned int m_vao, m_vbo, m_ebo;
    unsigned int m_wvao, m_wvbo, m_webo;
    float m_tileSize = 0.0f;
    float m_half = 0.0f;
    int m_size = 0;

    std::vector<std::vector<bool>> m_walls;
    GameState m_state = GameState::MENU;

};

