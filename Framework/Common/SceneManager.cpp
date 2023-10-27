#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "OGEX.hpp"

using namespace My;
using namespace std;
SceneManager::~SceneManager()
{
}

int SceneManager::Initialize()
{
    int result =0;
    return result;
}

void SceneManager::Finalize()
{
}

void SceneManager::Tick()
{
}

bool SceneManager::LoadOgexScene(const char *scene_file_name)
{
    string ogex_text = g_pAssetLoader->SyncOpenAndReadTextFileToString(scene_file_name);

    if (ogex_text.empty()) {
        return false;
    }

    OgexParser ogex_parser;
    m_pScene = ogex_parser.Parse(ogex_text);

    if (!m_pScene) {
        return false;
    }

    return true;
}

int SceneManager::LoadScene(const char *scene_file_name)
{
     if(LoadOgexScene(scene_file_name)) {
        return 0;
    }
    else {
        return -1;
    }
}

const Scene &SceneManager::GetSceneForRendering()
{
    return *m_pScene;
    // TODO: insert return statement here
}
