#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "OGEX.hpp"

My::SceneManager::~SceneManager()
{
}

int My::SceneManager::Initialize()
{
    int result =0;
    return result;
}

void My::SceneManager::Finalize()
{
}

void My::SceneManager::Tick()
{
}

void My::SceneManager::LoadOgexScene(const char *scene_file_name)
{
     AssetLoader asset_loader;
    string ogex_text = asset_loader.SyncOpenAndReadTextFileToString(scene_file_name);

    OgexParser ogex_parser;
    m_RootNode = ogex_parser.Parse(ogex_text);
}
