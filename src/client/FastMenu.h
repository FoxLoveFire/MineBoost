#ifndef EMPTY_ENVIRONMENT_H
#define EMPTY_ENVIRONMENT_H

#include <irrlicht.h>
#include "gui/modalMenu.h"
#include <vector>
#include "script/scripting_client.h"
#include "client/client.h"
#include "client/fontengine.h"
#include <codecvt> 
#include <locale> 

using namespace irr;
using namespace gui;

#define GET_SCRIPT_POINTER                                                     \
    ClientScripting *script = m_client->getScript();                         \
    if (!script || !script->m_function_loaded)                                 \
        return;

#define GET_SCRIPT_POINTER_BOOL                                                     \
    ClientScripting *script = m_client->getScript();                         \
    if (!script || !script->m_function_loaded)                                 \
        return true;     
        
class FastMenu: public IGUIElement {
public:
    FastMenu(gui::IGUIEnvironment* env, gui::IGUIElement* parent, s32 id, IMenuManager* menumgr, Client *client);
    ~FastMenu();

    void create();
    void close();

    virtual bool OnEvent(const irr::SEvent& event);
    virtual void draw() override;

    bool isOpen() { return m_is_open; }

private:
    core::rect<s32> createRect(s32 x, s32 y);
    std::vector<core::rect<s32>> categoryRects;
    core::vector2d<s32> offset; 
    IMenuManager* m_menumgr; 
    bool isDragging;
    core::vector2d<s32> rectPosition; 
    const int rectWidth = 200; 
    const int rectHeight = 40;
    bool m_is_open = false; 
    int draggedRectIndex;
    std::vector<bool> selectedCategory;
    std::vector<std::vector<core::rect<s32>>> subCategoryRects;
    std::vector<std::vector<video::SColor>> subCategoryColors;
    Client* m_client;
    bool m_rectsCreated = false;
    

    video::SColor outlineColor = video::SColor(255, 255, 255, 255);
};

#endif // EMPTY_ENVIRONMENT_H