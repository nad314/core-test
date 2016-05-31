#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE

#include <core>
#include <chrono>
typedef core::vec4 vec4;
typedef core::matrixf matrixf;
typedef core::vec4s vec4s;
typedef core::matrixs matrixs;

//windows
#include<CommCtrl.h>

//core parts
#include <timer/timer.h>

//core test
#include <window/renderWindow.h>
#include <window/mainWindow.h>
#include <model/mesh.h>
#include <model/simdMesh.h>
#include <render/view.h>
#include <render/render.h>

#include <program/coreTest.h>

