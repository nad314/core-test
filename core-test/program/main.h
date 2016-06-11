#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE

#include <core>
#include <core-forms>
using namespace core::opengl;
typedef core::vec4 vec4;
typedef core::matrixf matrixf;
typedef core::vec4s vec4s;
typedef core::matrixs matrixs;
typedef core::Color Color;
typedef core::Rect Rect;

//core components
#include <model/simdMesh.h>
#include <render/view.h>
#include <render/render.h>

//CoreTest components
#include <renderWindow/renderWindow.h>
#include <mainWindow/mainWindow.h>
#include <program/coreTest.h>

