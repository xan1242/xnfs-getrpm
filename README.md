# XNFS-GetRPM
Small addon for NFS to read out some vehicle status

## Games supported
This will only support games that implement the DALManager (Carbon and newer)

Currently this only works with Carbon, but expected games to work are:
- Need for Speed: Carbon
- Need for Speed: ProStreet
- Need for Speed: Undercover
- Need for Speed: World

## Building
The included solution is made from VS2015, however, you should be able to build this with any Visual Studio or Dev C++.

### Requirements
- IUP library: https://sourceforge.net/projects/iup/
- Canvas Draw library: https://sourceforge.net/projects/canvasdraw/

### Build process
- Open GetRPM.sln
- Select the configuration that you desire.
- Set the include and library directories in VC++ Directories in the solution properties settings (temporary, will not be necessary later)
- Output file will be in the configuration folder name.
- Copy GetRPM.asi to the scripts folder
- Copy GetRPM.ini to the scripts folder
