// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <Panes/CodePane.h>
#include <cinttypes>  // printf zu

#include <ctools/FileHelper.h>
#include <ctools/Logger.h>

CodePane::CodePane() = default;
CodePane::~CodePane() {
    Unit();
}

bool CodePane::Init() {
    return true;
}

void CodePane::Unit() {
    m_CodeSheets.clear();
}

///////////////////////////////////////////////////////////////////////////////////
//// IMGUI PANE ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

bool CodePane::DrawPanes(const uint32_t& /*vCurrentFrame*/, bool* vOpened, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    bool change = false;
    if (vOpened != nullptr && *vOpened) {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus  | ImGuiWindowFlags_MenuBar;
        if (ImGui::Begin(GetName().c_str(), vOpened, flags)) {
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
            auto win = ImGui::GetCurrentWindowRead();
            if (win->Viewport->Idx != 0)
                flags |= ImGuiWindowFlags_NoResize;  // | ImGuiWindowFlags_NoTitleBar;
            else
                flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus  | ImGuiWindowFlags_MenuBar;
#endif
            if (ImGui::BeginTabBar("CodePane")) {
                for (auto& sheet : m_CodeSheets) {
                    ImGui::PushID(sheet.filepathName.c_str());
                    if (ImGui::BeginTabItem(sheet.title.c_str(), &sheet.opened)) {
                        sheet.codeEditor.OnImGui();
                        ImGui::EndTabItem();
                    }
                    ImGui::PopID();
                }
                ImGui::EndTabBar();
            }
        }

        ImGui::End();
    }
    return change;
}

bool CodePane::DrawOverlays(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vRect*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

bool CodePane::DrawDialogsAndPopups(const uint32_t& /*vCurrentFrame*/, const ImRect& /*vMaxRect*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

bool CodePane::DrawWidgets(const uint32_t& /*vCurrentFrame*/, ImGuiContext* vContextPtr, void* /*vUserDatas*/) {
    ImGui::SetCurrentContext(vContextPtr);
    return false;
}

void CodePane::OpenFile(const std::string& vFilePathName, const size_t& vErrorLine, const std::string& vErrorMsg) {
    CodeSheet* existing_code_sheet_ptr = nullptr;
    for (size_t idx = 0; idx < m_CodeSheets.size(); ++idx) {
        const auto& sheet = m_CodeSheets.at(idx);
        if (sheet.filepathName == vFilePathName) {
            existing_code_sheet_ptr = &m_CodeSheets.at(idx);
            break;
        }
    }

    auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
    if (ps.isOk) {
        const auto code = FileHelper::Instance()->LoadFileToString(vFilePathName);
        TextEditor::LanguageDefinition type = TextEditor::LanguageDefinition::C();
        if (ps.ext == "cpp" || ps.ext == "hpp") {
            type = TextEditor::LanguageDefinition::Cpp();
        } else if (ps.ext == "c" || ps.ext == "h") {
            type = TextEditor::LanguageDefinition::C();
        } else if (ps.ext == "lua") {
            type = TextEditor::LanguageDefinition::Lua();
        }
        if (existing_code_sheet_ptr != nullptr) {
            existing_code_sheet_ptr->wasModified = false;
            existing_code_sheet_ptr->codeEditor.AddErrorMarker(vErrorLine, vErrorMsg);
        } else {
            auto& sheet = m_CodeSheets.emplace_back();
            sheet.codeEditor.init();
            sheet.filepathName = vFilePathName;
            sheet.opened = true;
            sheet.wasModified = false;
            sheet.title = ps.name + "." + ps.ext;
            sheet.codeEditor.SetCode(code, type);
            sheet.codeEditor.AddErrorMarker(vErrorLine, vErrorMsg);
        }
    }
}
