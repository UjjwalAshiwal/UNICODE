#include "framework.h"
#include "engineering design-ii.h"
#include <windowsx.h>
#include <commdlg.h> // common dialogs
#include <string>
#include <fstream>
#include <windows.h>
#include <shlobj.h> // required for shgetfolderpath function
#include <richedit.h>
#include <vector>
#include <wchar.h>
#include <stdio.h>
#include <locale>
#include<shellapi.h>
#include<windows.ui.shell.h>
//#include <Windows.h>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <iostream>
//#include <codecvt>
//#include <commctrl.h>
//#include <sstream>
//#include <cstdlib> // for system() function
//#include <cstdio> // for file operations


#define MAX_LOADSTRING 100
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hEdit;                                     // Handle to the edit control
int nScreenHeight;                              // Screen Height
int nScreenWidth;                               // Screen Width
HWND hwndDialog;                                // for dialog box
HFONT hFont = nullptr;
COLORREF textColor = RGB(255, 255, 255);        // Default text color
COLORREF bgColor = RGB(30, 30, 30);
HWND hLineNumbers = nullptr;
HWND hFindDlg = nullptr;
WCHAR szFindText[256];
WCHAR szReplaceText[256];
// Global variable to store the currently opened file path
std::wstring g_CurrentOpenedFilePath;
std::wstring filePath;

// Define color constants
const COLORREF TEXT_COLOR = RGB(0, 0, 0);      // Black
const COLORREF BG_COLOR = RGB(255, 255, 255);  // White
const COLORREF LINE_NUMBER_COLOR = RGB(0, 0, 0); // Gray

// Structure for character formatting
CHARFORMAT2 charFormat;

// Constant for setting text color
#define CFM_COLOR               0x40000000

// Define your keywords here
std::vector<std::wstring> keywords = { L"int", L"float", L"double", L"char", L"void", L"if", L"else", L"for", L"while", L"do", L"switch", L"case", L"break", L"continue", L"return" };

// Define the color for keywords
COLORREF keywordColor = RGB(0, 0, 255); // Blue color

// Constants for text selection and formatting
#define EM_SETCHARFORMAT        (WM_USER + 68)
#define SCF_SELECTION           0x0001


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                OnFileOpen(HWND hWnd);
void                OnFileNew(HWND hWnd);
void                OnFileSaveAs(HWND hWnd);
void                OnFileSaveAll(HWND hWnd);
void                OnEditCut(HWND hWnd);
void                OnEditCopy(HWND hWnd);
void                OnEditPaste(HWND hWnd);
void                OnCreate(HWND hWnd);
void                OnSize(HWND hWnd, UINT state, int cx, int cy);
void                OnUndo(HWND hWnd);
void                OnRedo(HWND hWnd);
INT_PTR CALLBACK    FindDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
std::wstring        GetFilePathFromUser(HWND hWnd);
void                compileAndRun();
std::wstring        get_current_directory();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ENGINEERINGDESIGNII, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        MessageBox(nullptr, L"InitInstance failed", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENGINEERINGDESIGNII));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENGINEERINGDESIGNII));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(30, 30, 30)); // Dark color
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ENGINEERINGDESIGNII);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    // Get the width and height of the screen
    nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Initialize the window
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, nScreenWidth / 2, nScreenHeight / 2, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    // Create the edit control
    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        0, 0, 0, 0, hWnd, nullptr, hInstance, nullptr);

    if (!hEdit)
    {
        return FALSE;
    }

    // Create the line numbers control
    hLineNumbers = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        0, 0, 0, 0, hWnd, nullptr, hInstance, nullptr);

    if (!hLineNumbers)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_FILE_OPEN:
            OnFileOpen(hWnd);
            break;
        case IDM_FILE_NEW:
            OnFileNew(hWnd);
            break;
        case IDM_FILE_SAVEAS:
            OnFileSaveAs(hWnd);
            break;
        case IDM_FILE_SAVEALL:
            OnFileSaveAll(hWnd);
            break;
        case IDM_EDIT_CUT:
            OnEditCut(hWnd);
            break;
        case IDM_EDIT_COPY:
            OnEditCopy(hWnd);
            break;
        case IDM_EDIT_PASTE:
            OnEditPaste(hWnd);
            break;
        case IDM_EDIT_UNDO:
            OnUndo(hWnd); 
            break;
        case IDM_EDIT_REDO:
            OnRedo(hWnd);
            break;
        case IDM_COMPILE_RUN:
            compileAndRun();
            break;
   
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case IDM_SEARCH_FIND:
        // Create and show the Find dialog box
        hFindDlg = CreateDialogParam(hInst, MAKEINTRESOURCEW(IDD_FIND_DIALOG), hWnd, FindDialogProc, 0);
        ShowWindow(hFindDlg, SW_SHOW);
        break;

    case WM_NOTIFY:
    {
        switch (((LPNMHDR)lParam)->code)
        {
        case EN_CHANGE:
            // Syntax highlighting logic here
            
            break;
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hLineNumbers, &ps);

        // Set line numbers font
        SelectObject(hdc, hFont);

        // Set text color
        SetTextColor(hdc, LINE_NUMBER_COLOR);

        // Set background color
        SetBkColor(hdc, BG_COLOR);

        // Calculate the number of lines in the edit control
        RECT rect;
        GetClientRect(hEdit, &rect);
        int nLines = SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);

        // Calculate the height of a single line
        RECT rcLine;
        SendMessage(hEdit, EM_GETRECT, 0, (LPARAM)&rcLine);
        int nLineHeight = rcLine.bottom - rcLine.top;

        // Draw line numbers
        wchar_t szLineNum[10];
        for (int i = 0; i < nLines; ++i)
        {
            swprintf(szLineNum, 10, L"%d", i + 1);
            TextOut(hdc, 0, i * nLineHeight, szLineNum, wcslen(szLineNum));
        }

        EndPaint(hLineNumbers, &ps);
    }
    break;

    case WM_SIZE:
    {
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        // Resize the edit control to fill the client area
        MoveWindow(hEdit, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE);
    }
    break;

    case WM_CREATE:
    {
        // Add this code here to enable syntax highlighting for C and C++ files
        SendMessage(hEdit, EM_SETTEXTMODE, TM_PLAINTEXT | TM_MULTILEVELUNDO | TM_RICHTEXT, 0);
        SendMessage(hEdit, EM_SETEVENTMASK, 0, ENM_CHANGE);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK FindDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            // Retrieve the text entered by the user
            GetDlgItemText(hDlg, IDC_FIND_TEXT, szFindText, 256);
            EndDialog(hDlg,  IDOK);
            return (INT_PTR)TRUE;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void OnFileOpen(HWND hWnd)
{
    OPENFILENAME ofn;
    WCHAR szFile[MAX_PATH];

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"C/C++ Files\0*.c;*.cpp\0JAVA/JavaScript Files\0*.java;*.js\0PYTHON Files\0*.py\0Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // Store the file path in the global variable
        g_CurrentOpenedFilePath = ofn.lpstrFile;

        std::wifstream file(g_CurrentOpenedFilePath);

        if (file.is_open())
        {
            std::wstring line;
            std::wstring content;

            // Read each line from the file and append it to the content string
            while (std::getline(file, line))
            {
                content += line + L"\r\n"; // Append each line with a carriage return and line feed
            }

            file.close();

            // Set the text in the edit control
            SetWindowText(hEdit, content.c_str());

         
        }
        else
        {
            MessageBox(hWnd, L"Failed to open file.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

void OnFileNew(HWND hWnd)
{
    // Clear the text in the edit control
    SetWindowText(hEdit, L"");
}

void OnFileSaveAs(HWND hWnd)
{
    OPENFILENAME ofn;
    WCHAR szFile[MAX_PATH];

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"C/C++ Files\0*.c;*.cpp\0JAVA/JavaScript Files\0*.java;*.js\0PYTHON Files\0*.py\0Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        std::wofstream file(ofn.lpstrFile);
        if (file.is_open())
        {
            // Get the text from the edit control
            int length = GetWindowTextLength(hEdit);
            std::wstring content(length + 1, L'\0');
            GetWindowText(hEdit, &content[0], length + 1);

            // Write the content to the file
            file << content;
            file.close();
        }
        else
        {
            MessageBox(hWnd, L"Failed to save file.", L"Error", MB_OK | MB_ICONERROR);
        }
    }
}

void OnFileSaveAll(HWND hWnd)
{
    if (IsWindow(hEdit))
    {
        // Check if the file has been created
        int length = GetWindowTextLength(hEdit);
        if (length > 0)
        {
            // The file has been created, save its content automatically
            OnFileSaveAs(hWnd);
        }
        else
        {
            // The file has not been created, prompt the user to save the file to the desktop
            WCHAR szDesktopPath[MAX_PATH];
            if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, szDesktopPath)))
            {
                // Successfully retrieved the desktop path
                std::wstring defaultFileName = L"Untitled.txt";
                std::wstring filePath = std::wstring(szDesktopPath) + L"\\" + defaultFileName;

                OPENFILENAME ofn;
                WCHAR szFile[MAX_PATH];

                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = szFile;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"C/C++ Files\0*.c;*.cpp\0JAVA/JavaScript Files\0*.java;*.js\0PYTHON Files\0*.py\0Text Files\0*.txt\0All Files\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = szDesktopPath;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                if (GetSaveFileName(&ofn) == TRUE)
                {
                    std::wofstream file(ofn.lpstrFile);
                    if (file.is_open())
                    {
                        // Get the text from the edit control
                        length = GetWindowTextLength(hEdit);
                        std::wstring content(length + 1, L'\0');
                        GetWindowText(hEdit, &content[0], length + 1);

                        // Write the content to the file
                        file << content;
                        file.close();
                    }
                    else
                    {
                        MessageBox(hWnd, L"Failed to save file.", L"Error", MB_OK | MB_ICONERROR);
                    }
                }
            }
            else
            {
                MessageBox(hWnd, L"Failed to retrieve desktop path.", L"Error", MB_OK | MB_ICONERROR);
            }
        }
    }
    else
    {
        MessageBox(hWnd, L"No file has been created.", L"Information", MB_OK | MB_ICONINFORMATION);
    }
}

void OnEditCut(HWND hWnd)
{
    // Cut the selected text in the edit control
    SendMessage(hEdit, WM_CUT, 0, 0);
}

void OnEditCopy(HWND hWnd)
{
    // Copy the selected text in the edit control
    SendMessage(hEdit, WM_COPY, 0, 0);
}

void OnEditPaste(HWND hWnd)
{
    // Paste the text from the clipboard into the edit control
    SendMessage(hEdit, WM_PASTE, 0, 0);
}

// Function to handle WM_CREATE message
void OnCreate(HWND hWnd) {
    // Create the text editor
    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        50, 0, nScreenWidth / 2, nScreenHeight / 2, hWnd, nullptr, hInst, nullptr);
    if (!hEdit) {
        MessageBox(hWnd, L"Failed to create text editor.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Create the line numbers control
    hLineNumbers = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        0, 0, 50, nScreenHeight / 2, hWnd, nullptr, hInst, nullptr);
    if (!hLineNumbers) {
        MessageBox(hWnd, L"Failed to create line numbers control.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Set line numbers font and color
    SendMessage(hLineNumbers, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(hLineNumbers, EM_SETBKGNDCOLOR, FALSE, BG_COLOR);
    SendMessage(hLineNumbers, EM_SETBKGNDCOLOR, TRUE, LINE_NUMBER_COLOR);
}

// Function to handle WM_SIZE message
void OnSize(HWND hWnd, UINT state, int cx, int cy) {
    // Resize the text editor
    MoveWindow(hEdit, 50, 0, cx - 50, cy, TRUE);

    // Resize the line numbers control
    MoveWindow(hLineNumbers, 0, 0, 50, cy, TRUE);

    
}

void OnUndo(HWND hWnd)
{
    SendMessage(hEdit, EM_UNDO, 0, 0);
}

void OnRedo(HWND hWnd)
{
    SendMessage(hEdit, EM_REDO, 0, 0);
}

void ShowErrorMessage(HWND hWnd, const wchar_t* message) {
    MessageBox(hWnd, message, L"Error", MB_OK | MB_ICONERROR);
}


std::wstring GetFilePathFromUser(HWND hWnd)
{
    OPENFILENAME ofn;
    WCHAR szFile[MAX_PATH] = L"";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"C / C++ Files\0 * .c; *.cpp\0JAVA Files\0 * .java; *.js\0PYTHON Files\0 * .py\0Text Files\0 * .txt\0All Files\0 * .*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    else
    {
        return L"";
    }
}


void compileAndRun() {
    // Check if there's content in the edit control
    int length = GetWindowTextLengthW(hEdit);
    if (length == 0) {
        MessageBoxW(nullptr, L"No code to compile.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Get the text from the edit control
    std::wstring code;
    code.resize(length + 1);
    GetWindowTextW(hEdit, &code[0], length + 1);

    // Determine the file extension based on the code content
    std::wstring fileExtension;
    if (code.find(L"#include") != std::wstring::npos) {
        fileExtension = L"cpp";
    }
    else {
        fileExtension = L"c";
    }

    // Save the code to a temporary file with the appropriate extension
    std::wstring tempFileName = L"temp." + fileExtension;
    std::wofstream tempFile(tempFileName);
    tempFile << code;
    tempFile.close();

    // Compile and run based on file extension
    if (fileExtension == L"cpp" || fileExtension == L"c") {
        // Compile C/C++ code using gcc/g++
        std::wstring compileCommand;
        if (fileExtension == L"cpp") {
            compileCommand = L"g++ -o temp.exe temp." + fileExtension;
        }
        else {
            compileCommand = L"gcc -o temp.exe temp." + fileExtension;
        }

        int compileResult = _wsystem(compileCommand.c_str());
        if (compileResult != 0) {
            MessageBoxW(nullptr, L"Compilation failed.", L"Error", MB_OK | MB_ICONERROR);
            return;
        }

        // Execute the compiled program
        int executeResult = _wsystem(L"temp.exe");
        if (executeResult != 0) {
            MessageBoxW(nullptr, L"Execution failed.", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
    }

    // Wait for a key press before closing the command prompt window
    MessageBoxW(nullptr, L"Press any key to continue...", L"Info", MB_OK);
}



 //FUNCTION: get_current_directory()
 //PURPOSE: Retrieves the current working directory.
//std::wstring get_current_directory()
//{
  //  WCHAR buffer[MAX_PATH];
    //GetCurrentDirectory(MAX_PATH, buffer);
   // return std::wstring(buffer);
//}