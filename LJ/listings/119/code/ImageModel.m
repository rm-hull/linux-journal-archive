#import "ImageModel.h"

@implementation ImageModel

/* This method will be automatically invoked when creating a new
   instance of the ImageModel class.
 */
- (id) init
{
  self = [super init];
  image = nil;
  return self;
}


/* This method is automatically called when the retain count
   of an object of this class turns zero. It releases the
   memory uses by our NSImage instance.
 */
- (void) dealloc
{
  [image release];
  [super dealloc];
}


/* This method returns the current NSImage instance of the model. */
- (NSImage *) image
{
  return image;
}


/* This method is used to set the current NSImage instance of the model. */
- (void) setImage: (NSImage *) theImage
{
  [theImage retain];
  [image release];
  image = theImage;
}

@end
