//
// Created by Zach Lee on 2022/10/1.
//

#include <Metal/MetalView.h>

@implementation MetalView

- (id)initWithFrame: (NSWindow*)nativeWindow
             device: (id<MTLDevice>)device
{
    self = [super initWithFrame: [nativeWindow frame]];
    self.autoresizingMask =(NSViewWidthSizable | NSViewHeightSizable);
    self.autoresizesSubviews = TRUE;
    
    self.metalLayer = [CAMetalLayer layer];
    self.metalLayer.device = device;
    self.metalLayer.opaque = YES;
    self.metalLayer.framebufferOnly = TRUE;
    self.metalLayer.drawsAsynchronously = TRUE;
    self.metalLayer.presentsWithTransaction = FALSE;
    self.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    self.layer = self.metalLayer;

    return self;
}

- (void)setFrameSize: (CGSize) size
{
    [super setFrameSize: size];
}

@end
