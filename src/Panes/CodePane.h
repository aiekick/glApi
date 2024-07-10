#pragma once

#include <ImGuiPack.h>
#include <MainFrame/CodeEditor.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class ProjectFile;
class CodePane : public AbstractPane {
private:
    struct CodeSheet {
        CodeEditor codeEditor;
        std::string filepathName;
        std::string title;
        bool wasModified = false;
        bool opened = false;
    };
    std::vector<CodeSheet> m_CodeSheets;

public:
    bool Init() final;
    void Unit() final;
    bool DrawWidgets(const uint32_t& vCurrentFrame, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) final;
    bool DrawOverlays(const uint32_t& vCurrentFrame, const ImRect& vRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) final;
    bool DrawPanes(const uint32_t& vCurrentFrame, bool* vOpened = nullptr, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) final;
    bool DrawDialogsAndPopups(const uint32_t& vCurrentFrame, const ImRect& vMaxRect, ImGuiContext* vContextPtr = nullptr, void* vUserDatas = nullptr) final;
    void OpenFile(const std::string& vFilePathName, const size_t& vErrorLine, const std::string& vErrorMsg);

public:  // singleton
    static std::shared_ptr<CodePane> Instance() {
        static std::shared_ptr<CodePane> _instance = std::make_shared<CodePane>();
        return _instance;
    }

public:
    CodePane();                              // Prevent construction
    CodePane(const CodePane&) = default;  // Prevent construction by copying
    CodePane& operator=(const CodePane&) {
        return *this;
    };                       // Prevent assignment
    virtual ~CodePane();  // Prevent unwanted destruction};
};
