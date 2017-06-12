#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <Mfidl.h>
#include <iostream>
#include <mfapi.h>
#include <mferror.h>


#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#include <windows.media.mediaproperties.h>
#include <windows.networking.sockets.h>

#include <wrl\client.h>
#include <wrl\implements.h>
#include <wrl\ftm.h>
#include <wrl\event.h> 
#include <wrl\wrappers\corewrappers.h>



#include "LinkList.h"
#include "WRLEvents.h"
#include "WRLUtilities.h"
#include "BaseAttributes.h"