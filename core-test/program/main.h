#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include <core>
#include <chrono>
typedef core::vec4 vec4;
typedef core::matrixf matrixf;
typedef core::vec4s vec4s;
typedef core::matrixs matrixs;

//windows
#include<CommCtrl.h>

//core test
#include <window/renderWindow.h>
#include <window/mainWindow.h>
#include <model/mesh.h>
#include <model/simdMesh.h>
#include <render/view.h>
#include <render/render.h>
#include <timer/timer.h>

#include <program/coreTest.h>

