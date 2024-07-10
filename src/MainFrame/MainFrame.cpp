#include "MainFrame.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ctools/Logger.h>
#include <ctools/FileHelper.h>

#include <Panes/CodePane.h>
#include <Panes/ConsolePane.h>
#include <Panes/ViewPane.h>

#define IMGUI_IMPL_API
#include <3rdparty/imgui_docking/backends/imgui_impl_opengl3.h>
#include <3rdparty/imgui_docking/backends/imgui_impl_glfw.h>
#include <Res/Cousine_Regular.cpp>
#include <Res/Roboto_Medium.cpp>
#include <Res/fontIcons.cpp>

#define WINDOW_TITLE "CuiCuiTools"
#define SET_DATAS_DIR_DIALOG_KEY "SET_DATAS_DIR"
#define SET_PROJECT_DIR_DIALOG_KEY "SET_PROJECT_DIR"
#define MESSAGING_CODE_INFOS 0
#define MESSAGING_LABEL_INFOS "Infos"
#define MESSAGING_CODE_WARNINGS 1
#define MESSAGING_LABEL_WARNINGS "Warnings"
#define MESSAGING_CODE_ERRORS 2
#define MESSAGING_LABEL_ERRORS "Errors"
#define MESSAGING_CODE_DEBUG 3
#define MESSAGING_LABEL_DEBUG "Debug"

static void glfw_error_callback(int error, const char *description) {
    LogVarError("glfw error %i : %s", error, description);
}

bool MainFrame::init() {  // Setup Dear ImGui context
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return false;
    }

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_MainWindowPtr = glfwCreateWindow(1280, 720, WINDOW_TITLE, nullptr, nullptr);
    if (m_MainWindowPtr == nullptr) {
        return false;
    }

    glfwMakeContextCurrent(m_MainWindowPtr);
    glfwSwapInterval(1);

    if (gladLoadGL() == 0) {
        LogVarError("Failed to initialize OpenGL loader");
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Viewport
    io.FontAllowUserScaling = true;                      // activate zoom feature with ctrl + mousewheel
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
    io.ConfigViewportsNoDecoration = false;  // toujours mettre une frame aux fenetres enfants
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // fonts
    {
        {  // main font
            if (ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(RM_compressed_data_base85, 15.0f) == nullptr) {
                LogVarError("Failed to load font Roboto");
                return false;
            }
        }
        {  // icon font
            static const ImWchar icons_ranges[] = {ICON_MIN_STRO, ICON_MAX_STRO, 0};
            ImFontConfig icons_config;
            icons_config.MergeMode = true;
            icons_config.PixelSnapH = true;
            if (ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_STRO, 15.0f, &icons_config, icons_ranges) == nullptr) {
                LogVarError("Failed to load font Icons");
                return false;
            }
        }
        {  // code font
            if (ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(CR_compressed_data_base85, 20.0f) == nullptr) {
                LogVarError("Failed to load font Cousine-Regular");
                return false;
            }
        }
    }

    ImGui::CustomStyle::Init();
    m_BuildThemes();

    Messaging::Instance()->AddCategory(MESSAGING_CODE_INFOS, "Infos(s)", MESSAGING_LABEL_INFOS, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
    Messaging::Instance()->AddCategory(MESSAGING_CODE_WARNINGS, "Warnings(s)", MESSAGING_LABEL_WARNINGS, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
    Messaging::Instance()->AddCategory(MESSAGING_CODE_ERRORS, "Errors(s)", MESSAGING_LABEL_ERRORS, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
#ifdef _DEBUG
    Messaging::Instance()->AddCategory(MESSAGING_CODE_DEBUG, "Debug(s)", MESSAGING_LABEL_DEBUG, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
#endif
    Messaging::Instance()->SetLayoutManager(LayoutManager::Instance());
    Logger::sStandardLogFunction = [](const int &vType, const std::string &vMessage) {
        MessageData msg_datas;
        const auto &type = vType;
        Messaging::Instance()->AddMessage(vMessage, type, false, msg_datas, {});
    };

    LayoutManager::Instance()->Init("Layouts", "Default Layout");

    LayoutManager::Instance()->SetPaneDisposalRatio("LEFT", 0.3f);
    LayoutManager::Instance()->SetPaneDisposalRatio("RIGHT", 0.3f);
    LayoutManager::Instance()->SetPaneDisposalRatio("BOTTOM", 0.3f);

    LayoutManager::Instance()->AddPane(CodePane::Instance(), "Code", "Views", "CENTRAL", 0.0f, true, false);
    LayoutManager::Instance()->AddPane(ViewPane::Instance(), "View", "Views", "LEFT", 0.4f, true, true);
    LayoutManager::Instance()->AddPane(ConsolePane::Instance(), "Console", "Views", "BOTTOM", 0.3f, false, false);

    LayoutManager::Instance()->InitPanes();

    Messaging::sMessagePaneId = static_cast<int64_t>(ConsolePane::Instance()->GetFlag());

    m_setAppTitle("", "");

    LoadConfigFile("config.xml", "app");

    return                                                      //
        ImGui_ImplGlfw_InitForOpenGL(m_MainWindowPtr, true) &&  //
        ImGui_ImplOpenGL3_Init(glsl_version);
}

void MainFrame::unit() {
    SaveConfigFile("config.xml", "app");
    LayoutManager::Instance()->UnitPanes();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_MainWindowPtr);
    glfwTerminate();
}

bool MainFrame::m_beginFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::CustomStyle::ResetCustomId();
    glfwGetFramebufferSize(m_MainWindowPtr, &m_DisplayWidth, &m_DisplayHeight);
    m_DisplaySize.x = (float)m_DisplayWidth;
    m_DisplaySize.y = (float)m_DisplayHeight;
    return m_WantToContinue;
}

void MainFrame::m_endFrame() {
    ImGui::Render();
    glViewport(0, 0, m_DisplayWidth, m_DisplayHeight);
    glClearColor(m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        auto *backup_current_context_ptr = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context_ptr);
    }
    glfwSwapBuffers(m_MainWindowPtr);
}

void MainFrame::doLoop() {
    while (!glfwWindowShouldClose(m_MainWindowPtr)) {
        if (m_beginFrame()) {
            m_doBackEnd();
            m_drawMainMenuBar();
            m_drawStatusBar();
            m_drawDialogs();
            if (LayoutManager::Instance()->BeginDockSpace(ImGuiDockNodeFlags_PassthruCentralNode)) {
                // overlay
                LayoutManager::Instance()->EndDockSpace();
            }
            if (LayoutManager::Instance()->DrawPanes(0, ImGui::GetCurrentContext(), {})) {
            }
            ImGuiThemeHelper::Instance()->Draw();
            LayoutManager::Instance()->InitAfterFirstDisplay(m_DisplaySize);
            if (m_ShowImGuiDemoWindow) {
                ImGui::ShowDemoWindow(&m_ShowImGuiDemoWindow);
            }
            m_endFrame();
        }
    }
}

std::string MainFrame::getXml(const std::string &vOffset, const std::string &vUserDatas) {
    std::string str;
    if (vUserDatas == "app") {
        str += vOffset + ct::toStr("<root_datas_path>%s</root_datas_path>\n", m_RootDatasPath.c_str());
        str += vOffset + ct::toStr("<root_project_path>%s</root_project_path>\n", m_RootProjectPath.c_str());
        str += ImGuiThemeHelper::Instance()->getXml(vOffset);
        str += LayoutManager::Instance()->getXml(vOffset, "app");
        // str += ViewPane::Instance()->getXml(vOffset, "app");
#ifdef USE_PLACES_FEATURE
        str += vOffset + "<places>" + ImGuiFileDialog::Instance()->SerializePlaces() + "</places>\n";
#endif
    }
    return str;
}

// return true for continue xml parsing of childs in this node or false for interrupt the child exploration (if we want explore child ourselves)
bool MainFrame::setFromXml(tinyxml2::XMLElement *vElem, tinyxml2::XMLElement *vParent, const std::string &vUserDatas) {
    // The value of this child identifies the name of this element
    std::string strValue;
    std::string strParentName;
    std::string strName = vElem->Value();
    if (vElem->GetText() != nullptr) {
        strValue = vElem->GetText();
    }
    if (vParent != nullptr) {
        strParentName = vParent->Value();
    }
    if (vUserDatas == "app") {
        ImGuiThemeHelper::Instance()->setFromXml(vElem, vParent);
        LayoutManager::Instance()->setFromXml(vElem, vParent, "app");
        // ViewPane::Instance()->setFromXml(vElem, vParent, "app");
        if (strName == "places") {
#ifdef USE_PLACES_FEATURE
            ImGuiFileDialog::Instance()->DeserializePlaces(strValue);
#endif
        } else if (strName == "root_datas_path") {
            m_setRootDatasPath(strValue);
            LogVarInfo("The Root datas path is %s", strValue.c_str());
        } else if (strName == "root_project_path") {
            m_setRootProjectPath(strValue);
            LogVarInfo("The Root project path is %s", strValue.c_str());
        }
        return true;
    }
    return false;
}

void MainFrame::m_drawDialogs() {
    const ImVec2 &maxSize = m_DisplaySize;
    const ImVec2 minSize = m_DisplaySize * 0.5f;
    if (ImGuiFileDialog::Instance()->Display(SET_DATAS_DIR_DIALOG_KEY, ImGuiWindowFlags_None, minSize, maxSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            m_setRootDatasPath(ImGuiFileDialog::Instance()->GetCurrentPath());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display(SET_PROJECT_DIR_DIALOG_KEY, ImGuiWindowFlags_None, minSize, maxSize)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            m_setRootProjectPath(ImGuiFileDialog::Instance()->GetCurrentPath());
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void MainFrame::m_drawMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Root")) {
            if (ImGui::MenuItem("Set datas dir")) {
                IGFD::FileDialogConfig config;
                config.path = m_RootDatasPath;
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog(SET_DATAS_DIR_DIALOG_KEY, "Set datas dir", nullptr, config);
            }
            if (ImGui::MenuItem("Set project dir")) {
                IGFD::FileDialogConfig config;
                config.path = m_RootProjectPath;
                config.flags = ImGuiFileDialogFlags_Modal;
                ImGuiFileDialog::Instance()->OpenDialog(SET_PROJECT_DIR_DIALOG_KEY, "Set project dir", nullptr, config);
            }
            ImGui::EndMenu();
        }

        LayoutManager::Instance()->DisplayMenu(m_DisplaySize);

        if (ImGui::BeginMenu("Styles")) {
            ImGuiThemeHelper::Instance()->DrawMenu();
            ImGui::Separator();
            ImGui::MenuItem("Show ImGui", "", &m_ShowImGuiDemoWindow);
            ImGui::EndMenu();
        }

        // ImGui Infos
        const auto label = ct::toStr("Dear ImGui %s (Docking)", ImGui::GetVersion());
        const auto size = ImGui::CalcTextSize(label.c_str());
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", label.c_str());
        ImGui::EndMainMenuBar();
    }
}

void MainFrame::m_drawStatusBar() {
    if (ImGui::BeginMainStatusBar()) {
        Messaging::Instance()->DrawStatusBar();

        //  ImGui Infos
        const auto &io = ImGui::GetIO();
        const auto fps = ct::toStr("%.1f ms/frame (%.1f fps)", 1000.0f / io.Framerate, io.Framerate);
        const auto size = ImGui::CalcTextSize(fps.c_str());
        ImGui::Spacing(ImGui::GetContentRegionAvail().x - size.x - ImGui::GetStyle().FramePadding.x * 2.0f);
        ImGui::Text("%s", fps.c_str());
        ImGui::EndMainStatusBar();
    }
}

void MainFrame::m_doBackEnd() {
    // ViewPane::Instance()->doBackend();
}

void MainFrame::m_setRootDatasPath(const std::string &vRootPath) {
    m_RootDatasPath = vRootPath;
    m_setAppTitle(m_RootProjectPath, m_RootDatasPath);
    // ViewPane::Instance()->setRootDatasPath(m_RootDatasPath);
}

void MainFrame::m_setRootProjectPath(const std::string &vRootPath) {
    m_RootProjectPath = vRootPath;
    m_setAppTitle(m_RootProjectPath, m_RootDatasPath);
    // ViewPane::Instance()->setRootProjectPath(m_RootProjectPath);
}

void MainFrame::m_setAppTitle(const std::string &vRootProjectPath, const std::string &vRootDatasPath) {
    if (!vRootDatasPath.empty() || !vRootProjectPath.empty()) {
        const auto title = ct::toStr(  //
            "CuiCuiTools %s - Project path : %s - Datas path : %s",
            sizeof(size_t) == 8U ? "x64" : "x32",
            vRootProjectPath.c_str(),
            vRootDatasPath.c_str());
        glfwSetWindowTitle(m_MainWindowPtr, title.c_str());
    } else {
        glfwSetWindowTitle(m_MainWindowPtr, "CuiCuiTools");
    }
}
