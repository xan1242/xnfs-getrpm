// GetRPM. (GUI Edition) (Carbon)
// Currently using the IUP library.
// Sorry for clutter! 
// This could be done without the (huge) FreeType library, however it is necessary for IupGauge to function.
// IupProgressBar works too. (without the bar colors)

#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include <stdlib.h>
#include <iup.h>
#include <iupcontrols.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "iup.lib")
#pragma comment(lib, "iupcontrols.lib")
#pragma comment(lib, "iupcd.lib")
#pragma comment(lib, "cd.lib")
#pragma comment(lib, "freetype6.lib")
#pragma comment(lib, "zlib1.lib")

#include "includes\IniReader.h"
#include "DALVehicle_Carbon.h"

int bDarkMode = 1;
int bRemoveSleep = 0;

void *DALVehiclePointer;
unsigned int *gTheDALManager;

bool bTerminateMainThread = 0;

float GetRpmValue = 0;
float MaxRPM = 10000.0;
float RedLine = 8000.0;
float GetTurboValue = 0;
float SpeedMpsValue = 0;
float NosValue = 0;
float SpeedBreakerValue = 0;
int GearValue = 0;
int CarIDHash = 0;
int OldCarIDHash = 0;
char* GearChar;

Ihandle *dlg, *label, *vbox, *speedlabel, *turbolabel, *rpmlabel, *gearlabel, *rpmbar, *noslabel, *nosbar, *speedbreakerlabel, *speedbreakerbar, *curcaridhashlabel;

DWORD WINAPI IUPThread()												// The GUI thread
{

	IupOpen(NULL, NULL);													// Init IUP lib
	IupControlsOpen();														// Init IUP controls lib

	label = IupLabel("GetRPM not initialized.");							// Setup main label strings.
	curcaridhashlabel = IupLabel("CarIDHash: ");
	gearlabel = IupLabel("Gear: ");
	rpmlabel = IupLabel("RPM: ");
	turbolabel = IupLabel("Turbo: ");
	speedlabel = IupLabel("Speed: ");
	noslabel = IupLabel("NOS: ");
	speedbreakerlabel = IupLabel("SpeedBreaker: ");

	rpmbar = IupGauge();													// Setup the bars.
	nosbar = IupGauge();
	speedbreakerbar = IupGauge();
	
	vbox = IupVbox(label, curcaridhashlabel, gearlabel, rpmlabel, rpmbar, speedlabel, turbolabel, noslabel, nosbar, speedbreakerlabel, speedbreakerbar, NULL);  // Set dialog box elements here.

	dlg = IupDialog(vbox);
	IupSetAttribute(dlg, "TITLE", "GetRPM (CB)");
	IupSetAttribute(dlg, "ICON", "IDI_ICON1");
	IupSetAttribute(dlg, "MAXBOX", "NO");
	IupSetAttribute(dlg, "RESIZE", "NO");
	IupSetAttribute(dlg, "MINSIZE", "260x196");								// Dialog box size.
	IupSetAttribute(dlg, "MAXSIZE", "260x196");
	if (bDarkMode)															// Dark mode using Visual Studio's dark color scheme.
	{
		IupSetAttribute(dlg, "BGCOLOR", "#2d2d30");
		IupSetAttribute(label, "FGCOLOR", "255 255 255");
		IupSetAttribute(curcaridhashlabel, "FGCOLOR", "255 255 255");
		IupSetAttribute(gearlabel, "FGCOLOR", "255 255 255");
		IupSetAttribute(rpmlabel, "FGCOLOR", "255 255 255");
		IupSetAttribute(turbolabel, "FGCOLOR", "255 255 255");
		IupSetAttribute(speedlabel, "FGCOLOR", "255 255 255");
		IupSetAttribute(noslabel, "FGCOLOR", "255 255 255");
		IupSetAttribute(speedbreakerlabel, "FGCOLOR", "255 255 255");
	}

	IupSetAttribute(rpmbar, "MAX", "10000");
	IupSetAttribute(rpmbar, "RASTERSIZE", "253x13");
	IupSetAttribute(rpmbar, "SHOWTEXT", "NO");
	IupSetAttribute(rpmbar, "FGCOLOR", "#81adad");
	IupSetAttribute(rpmbar, "FLAT", "YES");
	IupSetAttribute(rpmbar, "FLATCOLOR", "#81adad");

	IupSetAttribute(nosbar, "RASTERSIZE", "253x13");
	IupSetAttribute(nosbar, "FGCOLOR", "#57a64a");
//	IupSetAttribute(nosbar, "BGCOLOR", "#305829");
	IupSetAttribute(nosbar, "FLAT", "YES");
	IupSetAttribute(nosbar, "FLATCOLOR", "#57a64a");

	IupSetAttribute(speedbreakerbar, "RASTERSIZE", "253x13");
	IupSetAttribute(speedbreakerbar, "FGCOLOR", "#007ACC");
//	IupSetAttribute(speedbreakerbar, "BGCOLOR", "#024069");
	IupSetAttribute(speedbreakerbar, "FLAT", "YES");
	IupSetAttribute(speedbreakerbar, "FLATCOLOR", "#007ACC");

	IupShowXY(dlg, IUP_LEFT, IUP_TOP);

	IupMainLoop();

	IupClose();

	bTerminateMainThread = true;

	return EXIT_SUCCESS;
}

DWORD WINAPI MainThread()
{

	while (!bTerminateMainThread)
	{
//		Sleep(1);							// Check TheGameFlowManager state to initiate the loop.
		if (*(int*)THEGAMEFLOWMANAGER_ADDRESS == 3)
		{
			gTheDALManager = *(unsigned int**)THEDALMANAGERADDRESS;
			DALVehiclePointer = (void*)gTheDALManager[4];
			IupSetAttribute(label, "TITLE", "GetRPM standing by. (in FrontEnd)");
			IupRefresh(dlg);
		}
		if (*(int*)THEGAMEFLOWMANAGER_ADDRESS == 5)											// Initiate the loop right before entering ingame.
		{
			IupSetAttribute(label, "TITLE", "GetRPM running.");				// Update visual status.
			while (1)
			{
				if (!bRemoveSleep)											// To remove the 1ms delay (for more realtime readouts)
					Sleep(1);

				OldCarIDHash = CarIDHash;									// Monitor if car has changed to update the MaxRPM and the redline.
				DALVehicle_GetCarIDHash(DALVehiclePointer, &CarIDHash, 0);
				if (CarIDHash != OldCarIDHash)
				{
					// Start getting all necessary values.
					DALVehicle_GetMaxRPM(DALVehiclePointer, &MaxRPM, 0);
					DALVehicle_GetRedLine(DALVehiclePointer, &RedLine, 0);
					IupSetfAttribute(rpmbar, "MAX", "%.3f", MaxRPM);
				}

				DALVehicle_GetSpeedBreaker(DALVehiclePointer, &SpeedBreakerValue, 0);
				DALVehicle_GetNos(DALVehiclePointer, &NosValue, 0);
				DALVehicle_GetSpeedMps(DALVehiclePointer, &SpeedMpsValue, 0);
				DALVehicle_GetTurbo(DALVehiclePointer, &GetTurboValue, 0);
				DALVehicle_GetRPM(DALVehiclePointer, &GetRpmValue, 0);
				DALVehicle_GetGear(DALVehiclePointer, &GearValue, 0);

				if (GearValue == 1)														// Do the current gear check to update the chars.
				{
					GearChar = "N";
					IupSetfAttribute(gearlabel, "TITLE", "Gear: %s", GearChar);
				}
				else if (GearValue <= 0)
				{
					GearChar = "R";
					IupSetfAttribute(gearlabel, "TITLE", "Gear: %s", GearChar);
				}
				else
				{
					IupSetfAttribute(gearlabel, "TITLE", "Gear: %d", GearValue - 1);	// If it's not a neutral or reverse gear.
				}
				IupSetfAttribute(rpmlabel, "TITLE", "RPM: %.3f", GetRpmValue);
				if (GetRpmValue >= RedLine)												// Check if RPM crossed the redline and update the color.
				{
					IupSetAttribute(rpmbar, "FGCOLOR", "255 0 0");
					IupSetAttribute(rpmbar, "FLATCOLOR", "255 0 0");
				}
				else
				{
					if (bDarkMode)
					{
						IupSetAttribute(rpmbar, "FGCOLOR", "#4eeae9");
						IupSetAttribute(rpmbar, "BGCOLOR", "#1b4949");
						IupSetAttribute(rpmbar, "FLATCOLOR", "#4eeae9");
					}
					else
					{
						IupSetAttribute(rpmbar, "FGCOLOR", "0 0 255");
						IupSetAttribute(rpmbar, "FLATCOLOR", "0 0 255");
					}
				}
				IupSetfAttribute(rpmbar, "VALUE", "%.3f", GetRpmValue);
				IupSetfAttribute(turbolabel, "TITLE", "Turbo: %.3f", GetTurboValue);
				IupSetfAttribute(speedlabel, "TITLE", "Speed: %.3f km/h (%.3f m/s)", SpeedMpsValue*3.6, SpeedMpsValue);
				
				IupSetfAttribute(nosbar, "VALUE", "%.3f", NosValue);

				IupSetfAttribute(speedbreakerbar, "VALUE", "%.3f", SpeedBreakerValue);

				IupSetfAttribute(curcaridhashlabel, "TITLE", "CarIDHash: %x", CarIDHash);

				IupRefresh(dlg);														// Added to avoid glitches.
			}
		}

	}
	return 0;
}

int Init()
{
	CIniReader inireader("");

	bDarkMode = inireader.ReadInteger("GetRPM", "DarkMode", 1);
	bRemoveSleep = inireader.ReadInteger("GetRPM", "RemoveSleep", 0);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&MainThread, NULL, 0, NULL);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&IUPThread, NULL, 0, NULL);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Init();
	}
	return TRUE;
}
int main()		// Included due to IUP library. Never called.
{
	return 0;
}

