#import <AppKit/AppKit.h>

@interface ImageModel : NSObject
{
  NSImage *image;
}

- (NSImage *) image;
- (void) setImage: (NSImage *) theImage;

@end
