#pragma once

struct Resources;

class Loader {
public:
    Loader(Resources* resources);
    ~Loader();

    void loadResources();
    void loadShaders();
    void loadAtlas();
private:
    Resources* m_resources;
};