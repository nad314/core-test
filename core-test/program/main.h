#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE

#include <thread>
#include <mutex>

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
#include <model/polyOctree.h>
#include <OBVH/OBVH.h>
#include <pointCloud/pointCloud.h>
#include <pointCloud/pointOctree.h>
#include <PBVH/PBVH.h>
#include <render/view.h>
#include <render/render.h>
#include <render/renderTask.h>
#include <render/msRenderTask.h>

//CoreTest components
#include <controller/controller.h>
#include <renderWindow/renderWindow.h>
#include <mainWindow/mainWindow.h>
#include <program/coreTest.h>

