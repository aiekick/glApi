#pragma once

#include <ImGuiPack.h>
#include <ctools/ConfigAbstract.h>

struct GLFWwindow;

class MainFrame : public conf::ConfigAbstract {
private:
    ImVec4 m_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    GLFWwindow* m_MainWindowPtr = nullptr;
    bool m_WantToContinue = true;
    bool m_ShowImGuiDemoWindow = false;
    int m_DisplayWidth = 0;
    int m_DisplayHeight = 0;
    ImVec2 m_DisplaySize;
    std::string m_RootDatasPath;
    std::string m_RootProjectPath;

public:
    bool init();
    void unit();
    void doLoop();

protected:
    std::string getXml(const std::string& vOffset, const std::string& vUserDatas) final;
    // return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
    bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) final;

private:
    bool m_beginFrame();
    void m_endFrame();

    // Frontend
    void m_drawDialogs();
    void m_drawMainMenuBar();
    void m_drawStatusBar();
    bool m_BuildThemes();

    // Backend
    void m_doBackEnd();
    void m_setRootDatasPath(const std::string& vRootPath);
    void m_setRootProjectPath(const std::string& vRootPath);
    void m_setAppTitle(const std::string &vRootProjectPath, const std::string &vRootDatasPath);
};
