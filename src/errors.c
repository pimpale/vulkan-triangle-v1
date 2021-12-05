/* Copyright 2019 Govind Pimpale
 *
 * error_handle.c
 *
 *  Created on: Aug 7, 2018
 *      Author: gpi
 */

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>

#include "constants.h"

#include "errors.h"

char *vkstrerror(VkResult err) {
    char* errmsg;
    switch(err) {
    case VK_SUCCESS: { errmsg ="VK_SUCCESS"; break; }
    case VK_NOT_READY: { errmsg ="VK_NOT_READY"; break; }
    case VK_TIMEOUT: { errmsg ="VK_TIMEOUT"; break; }
    case VK_EVENT_SET: { errmsg ="VK_EVENT_SET"; break; }
    case VK_EVENT_RESET: { errmsg ="VK_EVENT_RESET"; break; }
    case VK_INCOMPLETE: { errmsg ="VK_INCOMPLETE"; break; }
    case VK_ERROR_OUT_OF_HOST_MEMORY: { errmsg ="VK_ERROR_OUT_OF_HOST_MEMORY"; break; }
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: { errmsg ="VK_ERROR_OUT_OF_DEVICE_MEMORY"; break; }
    case VK_ERROR_INITIALIZATION_FAILED: { errmsg ="VK_ERROR_INITIALIZATION_FAILED"; break; }
    case VK_ERROR_DEVICE_LOST: { errmsg ="VK_ERROR_DEVICE_LOST"; break; }
    case VK_ERROR_MEMORY_MAP_FAILED: { errmsg ="VK_ERROR_MEMORY_MAP_FAILED"; break; }
    case VK_ERROR_LAYER_NOT_PRESENT: { errmsg ="VK_ERROR_LAYER_NOT_PRESENT"; break; }
    case VK_ERROR_EXTENSION_NOT_PRESENT: { errmsg ="VK_ERROR_EXTENSION_NOT_PRESENT"; break; }
    case VK_ERROR_FEATURE_NOT_PRESENT: { errmsg ="VK_ERROR_FEATURE_NOT_PRESENT"; break; }
    case VK_ERROR_INCOMPATIBLE_DRIVER: { errmsg ="VK_ERROR_INCOMPATIBLE_DRIVER"; break; }
    case VK_ERROR_TOO_MANY_OBJECTS: { errmsg ="VK_ERROR_TOO_MANY_OBJECTS"; break; }
    case VK_ERROR_FORMAT_NOT_SUPPORTED: { errmsg ="VK_ERROR_FORMAT_NOT_SUPPORTED"; break; }
    case VK_ERROR_FRAGMENTED_POOL: { errmsg ="VK_ERROR_FRAGMENTED_POOL"; break; }
    case VK_ERROR_UNKNOWN: { errmsg ="VK_ERROR_UNKNOWN"; break; }
    case VK_ERROR_OUT_OF_POOL_MEMORY: { errmsg ="VK_ERROR_OUT_OF_POOL_MEMORY"; break; }
    case VK_ERROR_INVALID_EXTERNAL_HANDLE: { errmsg ="VK_ERROR_INVALID_EXTERNAL_HANDLE"; break; }
    case VK_ERROR_FRAGMENTATION: { errmsg ="VK_ERROR_FRAGMENTATION"; break; }
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: { errmsg ="VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"; break; }
    case VK_ERROR_SURFACE_LOST_KHR: { errmsg ="VK_ERROR_SURFACE_LOST_KHR"; break; }
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: { errmsg ="VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; break; }
    case VK_SUBOPTIMAL_KHR: { errmsg ="VK_SUBOPTIMAL_KHR"; break; }
    case VK_ERROR_OUT_OF_DATE_KHR: { errmsg ="VK_ERROR_OUT_OF_DATE_KHR"; break; }
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: { errmsg ="VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; break; }
    case VK_ERROR_VALIDATION_FAILED_EXT: { errmsg ="VK_ERROR_VALIDATION_FAILED_EXT"; break; }
    case VK_ERROR_INVALID_SHADER_NV: { errmsg ="VK_ERROR_INVALID_SHADER_NV"; break; }
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: { errmsg ="VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"; break; }
    case VK_ERROR_NOT_PERMITTED_EXT: { errmsg ="VK_ERROR_NOT_PERMITTED_EXT"; break; }
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: { errmsg ="VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"; break; }
    case VK_THREAD_IDLE_KHR: { errmsg ="VK_THREAD_IDLE_KHR"; break; }
    case VK_THREAD_DONE_KHR: { errmsg ="VK_THREAD_DONE_KHR"; break; }
    case VK_OPERATION_DEFERRED_KHR: { errmsg ="VK_OPERATION_DEFERRED_KHR"; break; }
    case VK_OPERATION_NOT_DEFERRED_KHR: { errmsg ="VK_OPERATION_NOT_DEFERRED_KHR"; break; }
    case VK_PIPELINE_COMPILE_REQUIRED_EXT: { errmsg ="VK_PIPELINE_COMPILE_REQUIRED_EXT"; break; }
    default: { errmsg = "UNKNOWN_ERROR"; break; }
    }
    return errmsg;
}

char *levelstrerror(ErrSeverity level) {
  char *str = "unknown";

  switch (level) {
  case ERR_LEVEL_DEBUG: {
    str = "debug";
    break;
  }
  case ERR_LEVEL_INFO: {
    str = "info";
    break;
  }
  case ERR_LEVEL_WARN: {
    str = "warn";
    break;
  }
  case ERR_LEVEL_ERROR: {
    str = "error";
    break;
  }
  case ERR_LEVEL_FATAL: {
    str = "fatal";
    break;
  }
  case ERR_LEVEL_UNKNOWN: {
    str = "unknown";
    break;
  }
  }

  return (str);
}
