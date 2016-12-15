#ifdef _MBCS
#undef _MBCS
#endif
#define _UNICODE

#include <thread>
#include <mutex>

#include <core>
#include <core-forms>
#include <core-tracing>
using namespace coreTypes;
using namespace core::opengl;

//core components
#include <render/renderTask.h>
#include <render/msRenderTask.h>
#include <render/subRenderTask.h>

//CoreTest components
#include <storage/storage.h>
#include <controller/controller.h>
#include <renderWindow/renderWindow.h>
#include <mainWindow/mainWindow.h>
#include <program/coreTest.h>

