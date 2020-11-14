#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <Windows.h>
#include "fmod/inc/fmod.hpp"
#include "fmod/inc/fmod_errors.h"

#pragma comment(lib, "fmod/lib/fmod_vc.lib")

#include "utils.h"

// https://github.com/zzhouhe/AsioHookForOsu
void listDev()
{
    FMOD::System *fmodSystem;
	FMOD::Sound *sound;
	FMOD_RESULT initRet = FMOD::System_Create(&fmodSystem); 
	int num;
	fmodSystem->setOutput(FMOD_OUTPUTTYPE_ASIO);
	fmodSystem->getNumDrivers(&num);
	printf("find %d drivers\n", num);
	if (!num)
	{
		printf("no device support ASIO\n");
		system("pause");
		return;
	}
	char name[1024];
    int r, c;
    FMOD_SPEAKERMODE m;
	for (int i =0; i<num; i++)
	{
		fmodSystem->getDriverInfo(i, name, 1024, NULL, &r, &m, &c);
		printf(" %d: %s\n", i, name);
	}
	system("pause");
}
//

void help()
{
    std::cout << "usage: keyasio.exe [options] [-k keys] " << std::endl;
    std::cout << "example: keyasio.exe -d 0 -b 128 -s 44100 -k 90 88 67 86" << std::endl;
    std::cout << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "  -h              show this help message" << std::endl;
    std::cout << "  -l              list of devices" << std::endl;
    std::cout << "  -d deviceid     device id" << std::endl;
    std::cout << "  -b buffersize   DSP buffer size" << std::endl;
    std::cout << "  -s samplerate   DSP sample rate in Hz" << std::endl;
    std::cout << "keys:" << std::endl;
    std::cout << "  -k k1 k2 [k3 k4]" << std::endl;
	std::cout << "  For custom key bindings, refer keycodes on site:" << std::endl <<
		"    https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes" << std::endl << std::endl;
    std::cout << std::endl;
	system("pause");
}

int readArgs(int argc, char* argv[], int& device_id, int& buffer_size, int& sample_rate, int& k1, int& k2, int& k3, int& k4)
{
    buffer_size = 128;
    device_id = 0;
    sample_rate = 44100;
    int flags = 0;
    if (argc >= 2)
    {
        for (int a = 1; a < argc; ++a)
        {
            if (strnlen(argv[a], 10) == 1)
            {
                flags |= Flags::UNKNOWN;
                break;
            }

            if (argv[a][0] == '-')
            {
                switch (argv[a][1])
                {
                case 'h':
                    flags |= Flags::HELP;
                    break;

                case 'l':
                    flags |= Flags::LIST;
                    break;

                case 'd':
                    if (a + 1 < argc && strnlen(argv[a + 1], 10) > 0 && argv[a + 1][0] != '-')
                    {
                        int tmp = atoi(argv[a + 1]);
                        if (tmp >= 0) device_id = tmp;
                        a++;
                    }
                    else flags |= Flags::UNKNOWN;
                    break;

                case 'b':
                    if (a + 1 < argc && strnlen(argv[a + 1], 10) > 0 && argv[a + 1][0] != '-')
                    {
                        int tmp = atoi(argv[a + 1]);
                        if (tmp > 0) buffer_size = tmp;
                        a++;
                    }
                    else flags |= Flags::UNKNOWN;
                    break;

                case 's':
                    if (a + 1 < argc && strnlen(argv[a + 1], 10) > 0 && argv[a + 1][0] != '-')
                    {
                        int tmp = atoi(argv[a + 1]);
                        if (tmp > 0) sample_rate = tmp;
                        a++;
                    }
                    else flags |= Flags::UNKNOWN;
                    break;

                case 'k':
                    if (a + 2 < argc && strnlen(argv[a + 1], 10) > 0 && argv[a + 1][0] != '-' && strnlen(argv[a + 2], 10) > 0 && argv[a + 2][0] != '-')
                    {
                        int tmp;
                        tmp = atoi(argv[a + 1]);
                        if (tmp > 0) k1 = tmp;
                        tmp = atoi(argv[a + 2]);
                        if (tmp > 0) k2 = tmp;
                        a += 2;

                        if (a + 2 < argc && strnlen(argv[a + 1], 10) > 0 && argv[a + 1][0] != '-' && strnlen(argv[a + 2], 10) > 0 && argv[a + 2][0] != '-')
                        {
                            int tmp;
                            tmp = atoi(argv[a + 1]);
                            if (tmp > 0) k3 = tmp;
                            tmp = atoi(argv[a + 2]);
                            if (tmp > 0) k4 = tmp;
                            a += 2;
                        }
                    }
                    else flags |= Flags::UNKNOWN;
                    break;

                default:
                    flags |= Flags::UNKNOWN;
                    a = argc;
                    break;
                }
            }
        }
    }
    return flags;
}