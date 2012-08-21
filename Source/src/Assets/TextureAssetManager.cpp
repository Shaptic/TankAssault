#include "Assets/TextureAssetManager.h"

using asset::GL_AssetManager;

GL_AssetManager& GL_AssetManager::GetInstance()
{
    static GL_AssetManager g_GLAssets;
    return g_GLAssets;
}

asset::asset_id GL_AssetManager::LoadEntityFromFile(const char* p_filename)
{
    GL_asset_iterator finder;

    // Figure out if the requested asset exists in the asset pool.
    for(finder = mp_assetPool.begin();
        finder != mp_assetPool.end();
        ++finder)
    {
        if(finder->second->GetFilename() == p_filename)
        {
            // If it exists, make a copy, add to all assets,
            // and return it.
            GL_Entity* p_Tmp = new GL_Entity;
            p_Tmp->LoadFromEntity(finder->second);
            mp_allAssets[p_Tmp->GetID()] = p_Tmp;
            m_assetcount++;
            return p_Tmp->GetID();
        }
    }

    // If if doesn't exist, add it to the pool, 
    // add it to all assets, and return it.

    // Do some logging operations.
    g_Log.Flush();
    g_Log << "[INFO] Loading texture asset: " << p_filename << ".\n";
    
    GL_Entity* p_ToPool = new GL_Entity;
    if(!p_ToPool->LoadFromFile(p_filename))
    {
        g_Log.Flush();
        g_Log << "[ERROR] Failed to load texture asset: ";
        g_Log << p_filename << ".\n";
        g_Log << "[ERROR] OpenGL error: " << gluGetString(glGetError());
        g_Log << "\n[ERROR] SDL error: " << SDL_GetError() << ".\n";
        gk::handle_error(g_Log.GetLastLog().c_str());
    }

    mp_assetPool[p_ToPool->GetID()] = p_ToPool;

    GL_Entity* p_ToAll = new GL_Entity;
    if(!p_ToAll->LoadFromEntity(p_ToPool))
    {
        g_Log.Flush();
        g_Log << "[ERROR] Failed to create texture from pixel data!\n";
        g_Log << "[ERROR] SDL error: " << SDL_GetError() << ".\n";
        gk::handle_error(g_Log.GetLastLog().c_str());
    }
    mp_allAssets[p_ToAll->GetID()] = p_ToAll;
    m_assetcount++;
    return p_ToAll->GetID();
}

asset::asset_id GL_AssetManager::LoadEntityFromSurface(SDL_Surface* const p_Surface)
{
    g_Log.Flush();
    g_Log << "[INFO] Creating texture asset from pixel data.\n";

    GL_Entity* p_ToAll = new GL_Entity;
    if(!p_ToAll->LoadFromSDLSurface(p_Surface))
    {
        g_Log.Flush();
        g_Log << "[ERROR] Failed to create texture from pixel data!\n";
        g_Log << "[ERROR] SDL error: " << SDL_GetError() << ".\n";
        gk::handle_error(g_Log.GetLastLog().c_str());
    }

    mp_allAssets[p_ToAll->GetID()] = p_ToAll;
    m_assetcount++;
    return p_ToAll->GetID();
}

asset::GL_Entity* GL_AssetManager::GetEntityByID(const asset::asset_id id)
{
    asset::GL_asset_iterator i = mp_allAssets.find(id);
    if(i == mp_allAssets.end())
        return NULL;
    else
        return i->second;
}

u_int GL_AssetManager::GetEntityCount() const
{
    return m_assetcount;
}

bool GL_AssetManager::UnloadEntityByID(const asset::asset_id id)
{
    asset::GL_asset_iterator i = mp_allAssets.find(id);
    if(i == mp_allAssets.end())
        return false;

    mp_allAssets.erase(i);
    return true;
}

asset::GL_AssetManager::~GL_AssetManager()
{
    asset::GL_asset_iterator i;
    for(i = mp_assetPool.begin();
        i != mp_assetPool.end();
        /* no third */)
    {
        delete i->second;
        i = mp_assetPool.erase(i);
    }

    mp_assetPool.clear();
    mp_allAssets.clear();
}
