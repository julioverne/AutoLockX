#import "AutoLockX.h"

#define NSLog(...)

#define PLIST_PATH_Settings "/var/mobile/Library/Preferences/com.julioverne.autolockx.plist"

static BOOL Enabled;

static BOOL isBlackScreen;

static float minutes;
static BOOL stopMedia;

static void handle_event(void* target, void* refcon, IOHIDServiceRef service, IOHIDEventRef event)
{
	if(Enabled) {
		IOHIDEventType eventType = IOHIDEventGetType(event);
		if(eventType == kIOHIDEventTypeDigitizer || eventType == kIOHIDEventTypeButton || eventType == kIOHIDEventTypeKeyboard) {
			[[AutoLockX sharedInstance] resetLockTime];
		}
	}
}

@implementation AutoLockX
@synthesize eventSystem;

__strong static id _sharedObject;

+ (id)sharedInstance
{
	if (!_sharedObject) {
		_sharedObject = [[self alloc] init];
	}
	return _sharedObject;
}

- (id)init
{
	self = [super init];
	
	if(self.eventSystem==NULL) {
		self.eventSystem = IOHIDEventSystemClientCreate(kCFAllocatorDefault);
		IOHIDEventSystemClientScheduleWithRunLoop(self.eventSystem, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		IOHIDEventSystemClientRegisterEventCallback(self.eventSystem, handle_event, NULL, NULL);
	}
	
	return self;
}

- (void)lockDevice
{
	uint64_t abTime = mach_absolute_time();
	
	IOHIDEventRef event = IOHIDEventCreateKeyboardEvent(kCFAllocatorDefault, abTime, kHIDPage_Consumer, kHIDUsage_Csmr_Power, YES, 0);
	IOHIDEventSystemClientDispatchEvent(self.eventSystem, event);
	CFRelease(event);
	
	event = IOHIDEventCreateKeyboardEvent(kCFAllocatorDefault, abTime, kHIDPage_Consumer, kHIDUsage_Csmr_Power, NO, 0);
	IOHIDEventSystemClientDispatchEvent(self.eventSystem, event);
	CFRelease(event);
}

- (void)stopMedia
{
	MRMediaRemoteSendCommand(kMRPause, nil);
	MRMediaRemoteSendCommand(kMRStop, nil);
}

- (void)_actionAutoLockX
{
	NSLog(@"** AutoLockX: _actionAutoLockX");
	if(Enabled && !isBlackScreen) {
		dispatch_async(dispatch_get_main_queue(), ^(void){
			
			if(stopMedia) {
				[self stopMedia];
			}
			
			[self lockDevice];
			
			if(stopMedia) {
				dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 1 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
					[self stopMedia];
				});
			}
		});
	}
}

- (void)resetLockTime
{
	NSLog(@"** AutoLockX: resetLockTime");
	[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(_actionAutoLockX) object:nil];
	[self performSelector:@selector(_actionAutoLockX) withObject:nil afterDelay:(minutes * 60)];
}

@end


static void screenDisplayStatus(CFNotificationCenterRef center, void* observer, CFStringRef name, const void* object, CFDictionaryRef userInfo)
{
    uint64_t state;
    int token;
    notify_register_check("com.apple.iokit.hid.displayStatus", &token);
    notify_get_state(token, &state);
    notify_cancel(token);
    if(!state) {
		isBlackScreen = YES;
    } else {
		isBlackScreen = NO;
	}
}


static void settingsChangedAutoLockX(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo)
{
	@autoreleasepool {
		NSDictionary *Prefs = [[[NSDictionary alloc] initWithContentsOfFile:@PLIST_PATH_Settings]?:@{} copy];
		Enabled = (BOOL)[Prefs[@"Enabled"]?:@YES boolValue];
		minutes = (float)[Prefs[@"minutes"]?:@(5) floatValue];
		if(minutes < 1) {
			minutes = 1;
		}
		stopMedia = (BOOL)[Prefs[@"stopMedia"]?:@NO boolValue];
		[[AutoLockX sharedInstance] resetLockTime];
	}
}


%ctor
{
	@autoreleasepool {
		CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(), NULL, screenDisplayStatus, CFSTR("com.apple.iokit.hid.displayStatus"), NULL, CFNotificationSuspensionBehaviorDeliverImmediately);
		CFNotificationCenterAddObserver(CFNotificationCenterGetDarwinNotifyCenter(), NULL, settingsChangedAutoLockX, CFSTR("com.julioverne.autolockx/SettingsChanged"), NULL, CFNotificationSuspensionBehaviorCoalesce);
		settingsChangedAutoLockX(NULL, NULL, NULL, NULL, NULL);
		%init;
	}
}
