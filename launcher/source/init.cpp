#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "video.h"
#include "audio.h"
#include "input.h"

#include "haxx.h"
#include "installer.h"
#include "wdvd.h"

extern "C" void Init_DebugConsole(int use_net);

bool PressA()
{
	printf("\tPress A to continue or press Home to exit.\n");
	while (true) {
		WPAD_ScanPads();

		int down = WPAD_ButtonsDown(0) |
			WPAD_ButtonsDown(1) |
			WPAD_ButtonsDown(2) |
			WPAD_ButtonsDown(3);
		if (down & WPAD_BUTTON_A)
			return true;
		if (down & WPAD_BUTTON_HOME)
			return false;
	}
}

void PressHome()
{
	printf("\tPress Home to exit.\n");
	do {
		WPAD_ScanPads();
	} while (!((WPAD_ButtonsDown(0) |
		WPAD_ButtonsDown(1) |
		WPAD_ButtonsDown(2) |
		WPAD_ButtonsDown(3)) & WPAD_BUTTON_HOME));
}

static volatile int ShutdownParam = 0;

static void CallbackReset()
{
	ShutdownParam = SYS_RETURNTOMENU;
}

static void CallbackPoweroffWiimote(int chan)
{
	ShutdownParam = SYS_POWEROFF;
}

static void CallbackPoweroff()
{
	CallbackPoweroffWiimote(0);
}

void CheckShutdown()
{
	switch (ShutdownParam) {
		case SYS_POWEROFF:
			// stop the disc
			WDVD_StopMotor(false, false);
		case SYS_RETURNTOMENU:
			SYS_ResetSystem(ShutdownParam, 0, 0);
	}
}

u32 MALLOC_MEM2 = 1;
enum { INSTALL_APPROACH_NOTHING = 0, INSTALL_APPROACH_UPDATE, INSTALL_APPROACH_DOWNGRADE };
void Initialise()
{
	InitVideo();

	if (Haxx_Init() < 0) {
		int approach = 0;
		WPAD_Init();
		printf("\n\n");
		if (IOS_GetVersion() != (u32)HAXX_IOS) {
			printf("\tIOS%d does not seem to be installed on your system.\n\tIt's perfectly safe to install it; do you want to do so now?\n", (u32)HAXX_IOS);
			if (!PressA())
				exit(0);
			approach = INSTALL_APPROACH_UPDATE;
		} else if (IOS_GetRevision() < HAXX_IOS_MINIMUM) {
			printf("\tIOS37 must be updated to continue.\n\tIt's perfectly safe to update it; do you want to do so now?\n");
			if (!PressA())
				exit(0);
			approach = INSTALL_APPROACH_UPDATE;
		} else if (IOS_GetRevision() > HAXX_IOS_MAXIMUM) {
			// Either cIOScrap (which will make the downgrade exploit fail) or a future update
			printf("\tIOS37 must be downgraded to continue. Do you want to attempt this now?\n");
			if (!PressA())
				exit(0);
			approach = INSTALL_APPROACH_DOWNGRADE;
		} else {
			// Proper version, but a patch failed. RawkSD patcher or DOP-IOS or something.
			printf("\tIOS37 must be reinstalled to continue.\n\tThis is a perfectly safe to do; do you want to reinstall it now?\n");
			if (!PressA())
				exit(0);
			approach = INSTALL_APPROACH_UPDATE;
		}

		int ret = 0;
		Installer_Initialize();
		switch (approach) {
			case INSTALL_APPROACH_UPDATE:
				ret = Install(HAXX_IOS, HAXX_IOS_MAXIMUM, false);
				break;
			case INSTALL_APPROACH_DOWNGRADE:
				ret = Install(HAXX_IOS, HAXX_IOS_MAXIMUM, true);
				break;
			default:
				exit(0);
		}
		Installer_Deinitialize();

		if (ret < 0) {
			printf("\tThe installation did not complete successfully.\n");
			PressHome();
			exit(0);
		} else {
			printf("\tThe installation was completed successfully!\n");
			WPAD_Shutdown();
			Haxx_Init();
		}
	}

	// uncomment to redirect stdout/stderr over wifi
	//Init_DebugConsole(1);
	Init_DebugConsole(0);

	SetupPads();
	InitAudio();

	SYS_SetResetCallback(CallbackReset);
	SYS_SetPowerCallback(CallbackPoweroff);
	WPAD_SetPowerButtonCallback(CallbackPoweroffWiimote);
}
