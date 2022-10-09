//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

@interface MetalView : NSView

@property  (nonatomic, assign )CAMetalLayer *metalLayer;
+ (id)layerClass;

- (id)initWithFrame: (NSWindow*)nativeWindow
             device: (id<MTLDevice>)device;
- (void)setFrameSize: (CGSize) size;
@end
