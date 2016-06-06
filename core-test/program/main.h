#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE

#include <core>
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
#include <control/control.h>
#include <form/form.h>
#include <control/buttonObject.h>
#include <control/button.h>
#include <control/imageButton.h>


//CoreTest components
#include <theme/theme.h>
#include <renderWindow/renderWindow.h>
#include <mainWindow/mainWindow.h>
#include <program/coreTest.h>

