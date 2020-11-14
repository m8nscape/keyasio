#include "fmod/inc/fmod.hpp"
#include "fmod/inc/fmod_errors.h"
#include <iostream>
#define NOMINMAX
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "fmod/lib/fmod_vc.lib")
#include <thread>
#include <string>

#include "utils.h"

#if _DEBUG
#define SILENCE
#endif

std::string path[6]{
"soundk1.wav",
"soundk2.wav",
"soundk3.wav",
"soundk4.wav",
"soundm1.wav",
"soundm2.wav",
};
const int RATE = 1;                 // 1ms
int VK_K1 = 'Z';                    // K1
int VK_K2 = 'X';                    // K2
int VK_K3 = 0;               
int VK_K4 = 0;              
int VK_M1 = VK_LBUTTON;             // M1
int VK_M2 = VK_RBUTTON;             // M2

FMOD::System *fmodSystem = nullptr;
FMOD::Sound *sound[6]{ nullptr };
HANDLE handle;
bool running = false;

enum KeyState
{
//                   STATE bit || STROKE bit
    OFF,                    // 00
    RELEASE,                // 01
    PRESS,                  // 10
    HOLD,                   // 11
};
KeyState K1, K2, K3, K4, M1, M2;
KeyState* keys[]{&K1, &K2, &K3, &K4, &M1, &M2};

inline KeyState eGetAsyncKeyState(KeyState prev, int vkey)
{
    if (GetAsyncKeyState(vkey) >> (sizeof(SHORT) - 1))
    {
        return prev & 2 ? HOLD : PRESS;
    }
    else
    {
        return prev & 2 ? RELEASE : OFF;
    }
}

VOID CALLBACK WaitOrTimerCallback(_In_ PVOID lpParameter, _In_ BOOLEAN TimerOrWaitFired)
{
    K1 = eGetAsyncKeyState(K1, VK_K1);
    K2 = eGetAsyncKeyState(K2, VK_K2);
	if (VK_K3 != 0)
	{
		K3 = eGetAsyncKeyState(K3, VK_K3);
		K4 = eGetAsyncKeyState(K4, VK_K4);
	}
    M1 = eGetAsyncKeyState(M1, VK_M1);
    M2 = eGetAsyncKeyState(M2, VK_M2);
    for (size_t i = 0; i < 6; ++i)
    {
        if (*keys[i] == PRESS)
        {
#if _DEBUG
            std::cout << "HIT\n";
#endif
#ifndef SILENCE
            fmodSystem->playSound(sound[i], 0, false, 0);
#endif
        }
    }
    fmodSystem->update();
}

int main(int argc, char* argv[])
{
    std::cout << "FMOD Studio Low Level API (C) Firelight Technologies Pty Ltd." << std::endl;
    std::cout << "use \"keyasio.exe -h\" to show help message." << std::endl;

    int buffer;
    int deviceid;
    int samplerate;
    int flags = readArgs(argc, argv, deviceid, buffer, samplerate, VK_K1, VK_K2, VK_K3, VK_K4);

    if (flags & Flags::UNKNOWN)
    {
        std::cout << "Unknown usage" << std::endl;
        flags |= Flags::HELP;
    }

    if (flags & Flags::HELP)
    {
        help();
        return 0;
    }

    if (flags & Flags::LIST)
    {
        listDev();
        return 0;
    }

    if (VK_K3)
        std::cout << "* Keys: " << VK_K1 << '/' << VK_K2 << '/' << VK_K3 << '/' << VK_K4 << "/M1/M2 *\n\n";
    else 
        std::cout << "* Keys: " << VK_K1 << '/' << VK_K2 << "/M1/M2 *\n\n";


    FMOD_RESULT initRet = FMOD::System_Create(&fmodSystem);      // Create the main system object.
    if (initRet != FMOD_OK)
    {
        std::cerr << "Create FMOD System Failed: " << FMOD_ErrorString((FMOD_RESULT)initRet) << std::endl;
        return initRet;
    }

    fmodSystem->setOutput(FMOD_OUTPUTTYPE_ASIO);
    fmodSystem->setDriver(deviceid);
    fmodSystem->setSoftwareFormat(samplerate, FMOD_SPEAKERMODE_DEFAULT, 0);
    fmodSystem->setDSPBufferSize(buffer, 2);

    initRet = fmodSystem->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (initRet != FMOD_OK)
    {
        std::cerr << "FMOD System Initialize Failed (" << initRet << "): " << FMOD_ErrorString((FMOD_RESULT)initRet) << std::endl;
        return initRet;
    }

    std::cout << "FMOD System Initialize Finished." << std::endl;


    int driverId;
    fmodSystem->getDriver(&driverId);

    char name[256];
    int systemRate;
    int speakerChannels;
    fmodSystem->getDriverInfo(driverId, name, 255, 0, &systemRate, 0, &speakerChannels);

    unsigned bufLen;
    int bufNum;
    fmodSystem->getDSPBufferSize(&bufLen, &bufNum);

    std::cout << "[FMOD] Device Name: " << name << std::endl;
    std::cout << "[FMOD] Device Sample Rate: " << systemRate << std::endl;
    std::cout << "[FMOD] Device Channels: " << speakerChannels << std::endl;
    std::cout << "[FMOD] DSP buffer size: " << bufLen << " * " << bufNum << std::endl;
    std::cout << "[FMOD] Estimated latency: " << bufLen * bufNum * 1000.0 / systemRate  << "ms" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Press Ctrl+C to exit" << std::endl << std::endl;

#ifndef SILENCE
    for (size_t i = 0; i < 6; ++i)
    {
		if (i == 2 && VK_K3 == 0) continue;
		if (i == 3 && VK_K4 == 0) continue;
        if (path[i].empty())
        {
            std::cerr << "[FMOD] Sample path empty" << std::endl;
            Sleep(2000);
            return 1;
        }
        FMOD_RESULT r = fmodSystem->createSound(path[i].c_str(), FMOD_UNIQUE, 0, &sound[i]);

        if (r != FMOD_OK)
        {
            std::cerr << "[FMOD] Loading Sample (" + path[i] + ") Error: " << r << ", " << FMOD_ErrorString(r) << std::endl;
            //Sleep(2000);
            //return 1;
        }
    }
#endif
    
    timeBeginPeriod(1);
    CreateTimerQueueTimer(
        &handle,
        NULL, WaitOrTimerCallback,
        nullptr,
        0,
        RATE,
        WT_EXECUTEDEFAULT
    );
    running = true;
    SetConsoleCtrlHandler([](_In_ DWORD dwCtrlType) { running = false; return TRUE; }, TRUE);

    while (running)
    {
        Sleep(500);
    }

    DeleteTimerQueueTimer(NULL, handle, NULL);
    timeEndPeriod(1);

    if (initRet == FMOD_OK && fmodSystem != nullptr)
    {
        fmodSystem->release();
        std::cout << "FMOD System released." << std::endl;
    }
    
    return 0;
}