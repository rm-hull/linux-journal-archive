#import "AppController.h"

#import <Renaissance/Renaissance.h>

@implementation AppController

/* This is the designed initialisation method. This method
   will be automatically invoked when creating an instance
   of this class.
*/
- (id) init
{
  self = [super init];
  model = [[ImageModel alloc] init];
  return self;
}

/* This method is automatically called when the retain count
   of an object of this class turns zero. This method will release
   the memory used by this object and also decrement the retain
   count of our ImageModel by one - so -dealloc of the ImageModel
   instance will be automatically called.
 */
- (void) dealloc
{
  [model release];
  [super dealloc];
}

/* This method is automatically called upon application's startup. 
   It's the last method called before the application can start
   receiving events.
 */
- (void) applicationDidFinishLaunching:
  (NSNotification *) theNotification
{
  [NSBundle loadGSMarkupNamed: @"TiffViewer"
	    owner: self];
}

/* This method is automatically called right before the NSApplication
   object is initialized.
 */
- (void) applicationWillFinishLaunching:
  (NSNotification *) theNotification
{
#ifdef GNUSTEP
  [NSBundle loadGSMarkupNamed: @"GNUstep-Menu"
	    owner: self];
#else
  [NSBundle loadGSMarkupNamed: @"Cocoa-Menu"
	    owner: self];
#endif
}

/* This method is invoked when the user clicks on the "Load Image"
   menu item.
 */
- (void) loadImage: (id)sender
{
  NSOpenPanel *oPanel;
  int result;
  
  /* We show our open panel and we get the result from the user */
  oPanel = [NSOpenPanel openPanel];
  [oPanel setAllowsMultipleSelection:YES];
  result = [oPanel runModalForDirectory: NSHomeDirectory()
		   file: nil
		   types: nil];
  
  /* If the user has clicked on the "OK" button... */
  if (result == NSOKButton)
    {
      NSEnumerator *filesToOpenEnumerator;
      NSString *theFilename;
      
      filesToOpenEnumerator = [[oPanel filenames] 
				objectEnumerator];
      theFilename = [filesToOpenEnumerator nextObject];
      
      /* We get the selected file, if any. */
      if ( theFilename )
	{
	  NSImage *image;
	  
	  /* We initialize an image object from the selected file name */
	  image = [[NSImage alloc] 
		    initWithContentsOfFile: theFilename];
	  
	  /* We set the image to our ImageModel instance */
	  [model setImage: image];
	  [image release];
	  
	  /* We finally set the title of our window to the image's path
	     and we also refresh our imageView object with the current
	     selected image.
	  */
	  [window setTitle: theFilename];
	  [imageView setImage: [model image]];  
	}
    }
}

@end

/* Entry point of the application */
int main(int argc, const char *argv[], char *env[])
{
  AppController *controller;
  NSAutoreleasePool *pool;

  /* We create our default autorelease pool and 
     our main controller object. */
  pool = [[NSAutoreleasePool alloc] init];
  controller = [[AppController alloc] init];

  [NSApplication sharedApplication];
  [NSApp setDelegate: controller];

  NSApplicationMain(argc, argv);

  [controller release];
  [pool release];

  return 0;
}
