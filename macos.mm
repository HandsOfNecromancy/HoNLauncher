#import <Cocoa/Cocoa.h>
#include "strings.h"
#include "launcher.h"

@interface MyView : NSView
@end

@implementation MyView

- (void)resetCursorRects
{
	[self addCursorRect:[self bounds] cursor:[NSCursor arrowCursor]];
}

@end


@interface AppDelegate : NSObject <NSApplicationDelegate, NSComboBoxDataSource, NSComboBoxDelegate>
@property (strong, nonatomic) NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSString *appPath1 = [[NSBundle mainBundle] bundlePath];
	NSString *appPath2 = @"/Contents/MacOS";
	NSString *appPath = [appPath stringByAppendingString:appPath2];
	[fileManager changeCurrentDirectoryPath:appPath];

	//printf ("%s\n", [appPath UTF8String]);

	// Create the window
	self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 640, 480)
				styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
				backing:NSBackingStoreBuffered
				defer:NO];
	[self.window center];

	MyView *contentView = [[MyView alloc] initWithFrame:[self.window.contentView bounds]];
	[self.window setContentView:contentView];

	// Set the background image
	NSString *imagePath = [appPath stringByAppendingPathComponent:@"startup.png"];
	NSURL *imageURL = [NSURL fileURLWithPath:imagePath];
	NSImage *image = [[NSImage alloc] initWithContentsOfURL:imageURL];

	NSImageView *backgroundView = [[NSImageView alloc] initWithFrame:self.window.contentView.bounds];
	backgroundView.imageScaling = NSImageScaleAxesIndependently;
	backgroundView.image = image;
	[self.window.contentView addSubview:backgroundView positioned:NSWindowBelow relativeTo:nil];
	
	// Create the combo box
	NSComboBox *comboBox = [[NSComboBox alloc] initWithFrame:NSMakeRect(14, 440, 200, 24)];
	//[comboBox addItemsWithObjectValues:@[@VIDMODE1, @VIDMODE2, @VIDMODE3, @VIDMODE4]];
	comboBox.usesDataSource = YES;
	comboBox.dataSource = self; // assuming AppDelegate implements the NSComboBoxDataSource protocol
	comboBox.delegate = self; // assuming AppDelegate implements the NSComboBoxDelegate protocol
	[comboBox selectItemAtIndex:vidmode];
	[self.window.contentView addSubview:comboBox];
	
	// Create the buttons
	NSButton *button1 = [[NSButton alloc] initWithFrame:NSMakeRect(398, 440, 100, 24)];
	[button1 setTitle:@BTPLAY];
	button1.target = self;
	button1.action = @selector(button1Clicked:);
	[self.window.contentView addSubview:button1];
	
	NSButton *button2 = [[NSButton alloc] initWithFrame:NSMakeRect(526, 440, 100, 24)];
	[button2 setTitle:@BTQUIT];
	button2.target = self;
	button2.action = @selector(button2Clicked:);
	[self.window.contentView addSubview:button2];
	
	// Create the radio buttons
	NSButton *radioButton1 = [[NSButton alloc] initWithFrame:NSMakeRect(10, 160, 100, 24)];
	[radioButton1 setButtonType:NSButtonTypeRadio];
	[radioButton1 setTitle:@LANGEN];
	if (userlang == 0)
		[radioButton1 setState:NSControlStateValueOn];
	radioButton1.target = self;
	radioButton1.action = @selector(radioButton1Clicked:);
	[self.window.contentView addSubview:radioButton1];
	
	NSButton *radioButton2 = [[NSButton alloc] initWithFrame:NSMakeRect(10, 130, 100, 24)];
	[radioButton2 setButtonType:NSButtonTypeRadio];
	[radioButton2 setTitle:@LANGFR];
	if (userlang == 1)
		[radioButton2 setState:NSControlStateValueOn];
	radioButton2.target = self;
	radioButton2.action = @selector(radioButton2Clicked:);
	[self.window.contentView addSubview:radioButton2];
	
	NSButton *radioButton3 = [[NSButton alloc] initWithFrame:NSMakeRect(10, 100, 100, 24)];
	[radioButton3 setButtonType:NSButtonTypeRadio];
	[radioButton3 setTitle:@LANGDE];
	if (userlang == 2)
		[radioButton3 setState:NSControlStateValueOn];
	radioButton3.target = self;
	radioButton3.action = @selector(radioButton3Clicked:);
	[self.window.contentView addSubview:radioButton3];
	
	NSButton *radioButton4 = [[NSButton alloc] initWithFrame:NSMakeRect(10, 70, 100, 24)];
	[radioButton4 setButtonType:NSButtonTypeRadio];
	[radioButton4 setTitle:@LANGRU];
	if (userlang == 3)
		[radioButton4 setState:NSControlStateValueOn];
	radioButton4.target = self;
	radioButton4.action = @selector(radioButton4Clicked:);
	[self.window.contentView addSubview:radioButton4];
	
	NSButton *radioButton5 = [[NSButton alloc] initWithFrame:NSMakeRect(10, 40, 100, 24)];
	[radioButton5 setButtonType:NSButtonTypeRadio];
	[radioButton5 setTitle:@LANGES];
	if (userlang == 4)
		[radioButton5 setState:NSControlStateValueOn];
	radioButton5.target = self;
	radioButton5.action = @selector(radioButton5Clicked:);
	[self.window.contentView addSubview:radioButton5];
	
	NSButton *radioButton6 = [[NSButton alloc] initWithFrame:NSMakeRect(10, 10, 100, 24)];
	[radioButton6 setButtonType:NSButtonTypeRadio];
	[radioButton6 setTitle:@LANGPT];
	if (userlang == 5)
		[radioButton6 setState:NSControlStateValueOn];
	radioButton6.target = self;
	radioButton6.action = @selector(radioButton6Clicked:);
	[self.window.contentView addSubview:radioButton6];

	// Create the clickable image
	NSString *extrasPath = [appPath stringByAppendingPathComponent:@"extras.png"];
	NSURL *extrasURL = [NSURL fileURLWithPath:extrasPath];
	NSImage *extrasImage = [[NSImage alloc] initWithContentsOfURL:extrasURL];

	if (extrasImage)
	{
		[extrasImage setSize:NSMakeSize(160, 120)]; // Set the image size

		NSButton *extrasButton = [[NSButton alloc] initWithFrame:NSMakeRect(self.window.contentView.bounds.size.width - 170, 10, 160, 120)];
		[extrasButton setImage:extrasImage];
		[extrasButton setButtonType:NSButtonTypeMomentaryChange];
		[extrasButton setBordered:NO]; // Remove border
		[extrasButton setTarget:self];
		[extrasButton setAction:@selector(extrasButtonClicked:)];
		[extrasButton setAlphaValue:1.0]; // Make the button fully transparent
		[extrasButton setBackgroundColor:[NSColor clearColor]]; // Make the button's background color clear
		[self.window.contentView addSubview:extrasButton];
	}
	[self.window makeKeyAndOrderFront:nil];
}

- (void)extrasButtonClicked:(NSButton *)button
{
	LaunchExtras();
}

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox
{
	return 4; // number of options
}

- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index
{
	NSArray *options = @[@VIDMODE1, @VIDMODE2, @VIDMODE3, @VIDMODE4];
	return options[index];
}

- (NSString *)comboBox:(NSComboBox *)aComboBox completedString:(NSString *)uncompletedString
{
	// This method is called when the user types in the combo box.
	// Returning nil means the combo box won't accept the input.
	return nil;
}

- (void)comboBoxSelectionDidChange:(NSNotification *)notification
{
	NSComboBox *comboBox = notification.object;
	//NSLog(@"Combo box selection changed. Selected index: %ld", (long)comboBox.indexOfSelectedItem);
	vidmode = comboBox.indexOfSelectedItem;
}

- (void)button1Clicked:(NSButton *)button
{
	LaunchGame();
	[[NSApplication sharedApplication] terminate:nil];
}

- (void)button2Clicked:(NSButton *)button
{
	[[NSApplication sharedApplication] terminate:nil];
}

- (void)deselectOtherButtons:(NSButton *)button
{
	for (NSView *subview in self.window.contentView.subviews)
	{
		if ([subview isKindOfClass:[NSButton class]] && ((NSButton*)subview != button))
		{
			[(NSButton *)subview setState:NSControlStateValueOff];
		}
	}
}

- (void)radioButton1Clicked:(NSButton *)button
{
	userlang = 0;
	[self deselectOtherButtons:button];
}

- (void)radioButton2Clicked:(NSButton *)button
{
	userlang = 1;
	[self deselectOtherButtons:button];
}

- (void)radioButton3Clicked:(NSButton *)button
{
	userlang = 2;
	[self deselectOtherButtons:button];
}

- (void)radioButton4Clicked:(NSButton *)button
{
	userlang = 3;
	[self deselectOtherButtons:button];
}

- (void)radioButton5Clicked:(NSButton *)button
{
	userlang = 4;
	[self deselectOtherButtons:button];
}

- (void)radioButton6Clicked:(NSButton *)button
{
	userlang = 5;
	[self deselectOtherButtons:button];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	// Perform cleanup tasks here
	return NSTerminateNow;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

@end

int main(int argc, const char * argv[]) {
	LoadSettings();

	@autoreleasepool
	{
		NSMutableString *commandLine = [NSMutableString string];
		for (int i = 0; i < argc; i++)
		{
			[commandLine appendFormat:@"%s ", argv[i]];
		}
		commandline = strdup((char *)[commandLine UTF8String]);
	}

	AdjustSettings();

	if (skiplauncher)
	{
		// short circuit window creation, it's not necessary
		LaunchGame();
		return EXIT_SUCCESS;
	}

	AppDelegate *appDelegate = [[AppDelegate alloc] init];
	NSApplication *application = [NSApplication sharedApplication];
	[application setDelegate:appDelegate];
	[application run];
	
	return EXIT_SUCCESS;
}
