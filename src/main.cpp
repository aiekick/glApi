#include <MainFrame/MainFrame.h>
#include <ctools/FileHelper.h>
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
#include <winuser.h> // ShowWindow
#define WIN32
#endif

// Main code
int main(int, char** vArgv) {
    FileHelper::Instance()->SetAppPath(vArgv[0]);
    FileHelper::Instance()->SetCurDirectory(FileHelper::Instance()->GetAppPath());
#ifdef WIN32
#ifdef _DEBUG
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
#endif
    MainFrame mf;
    if (mf.init()) {
        mf.doLoop();
        mf.unit();
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
