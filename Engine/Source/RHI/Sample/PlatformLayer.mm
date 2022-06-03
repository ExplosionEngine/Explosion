
#if PLATFORM_MACOS
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include "PlatformLayer.h"
#include <GLFW/glfw3native.h>
#import <AppKit/AppKit.h>

void CreateLayer(void* window)
{
    auto nsWin = static_cast<NSWindow*>(window);
//    NSBundle* bundle = [NSBundle bundleWithPath: @"/System/Library/Frameworks/QuartzCore.framework"];
//    CALayer* layer = [[bundle classNamed: @"CAMetalLayer"] layer];
//    NSView* view = nsWin.contentView;
//    [view setLayer: layer];
//    [view setWantsLayer: YES];
}
#endif