// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef PLATFORM_API_ENUMS_H
#define PLATFORM_API_ENUMS_H

#ifdef __cplusplus
extern "C" {
#endif

// Renderer type selection
typedef enum PlatformRendererType {
  PLATFORM_RENDERER_VULKAN,
  PLATFORM_RENDERER_METAL,
  PLATFORM_RENDERER_OPENGL
} PlatformRendererType;

// Add more enums as needed:
// typedef enum PlatformKeyCode { ... } PlatformKeyCode;
// typedef enum PlatformMouseButton { ... } PlatformMouseButton;
// typedef enum PlatformLogLevel { ... } PlatformLogLevel;

#ifdef __cplusplus
}
#endif

#endif
