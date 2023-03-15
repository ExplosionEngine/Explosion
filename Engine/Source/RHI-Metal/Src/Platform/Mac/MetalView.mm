//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/MetalView.h>

@implementation MetalView

- (id)initWithFrame: (NSWindow*)nativeWindow
             device: (id<MTLDevice>)device
{
    if (self = [super initWithFrame: [nativeWindow frame]]) {
        self.autoresizingMask = (NSViewWidthSizable | NSViewHeightSizable);
        self.autoresizesSubviews = TRUE;
        self.wantsLayer = YES;
        self.layer = self.metalLayer = [CAMetalLayer layer];
        
        self.metalLayer.device = device;
        self.metalLayer.opaque = YES;
        self.metalLayer.framebufferOnly = TRUE;
        self.metalLayer.drawsAsynchronously = TRUE;
        self.metalLayer.presentsWithTransaction = FALSE;
        self.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    }
    return self;
}

- (void)setFrameSize: (CGSize) size
{
    [super setFrameSize: size];
    [self.metalLayer setDrawableSize: size];
}

+ (id)layerClass
{
    return [CAMetalLayer class];
}

@end
