#import <IOKit/hid/IOHIDEventTypes.h>
#import <IOKit/hidsystem/IOHIDUsageTables.h>
#import <dlfcn.h>
#import <objc/runtime.h>
#import <mach/mach.h>
#import <mach/mach_time.h>
#import <sys/sysctl.h>
#import <substrate.h>
#import <notify.h>
#import <signal.h>
#import <mach-o/dyld.h>
#import <ifaddrs.h>
#import <arpa/inet.h>
#import <GraphicsServices/GraphicsServices.h>

#import <MediaRemote.h>

#import <prefs.h>

@interface SpringBoard : UIApplication
- (void)_simulateLockButtonPress;

- (void)lockButtonUp:(__GSEvent *)event;
- (void)lockButtonDown:(__GSEvent *)event;

@end

typedef CFTypeRef IOHIDEventRef;
typedef CFTypeRef IOHIDEventSystemClientRef;
typedef CFTypeRef IOHIDEventSystemConnectionRef;
typedef CFTypeRef IOHIDServiceRef;

#define kIOHIDEventDigitizerSenderID 0x8000000817319372

extern "C" {
	
	IOHIDFloat IOHIDEventGetFloatValue(IOHIDEventRef event, IOHIDEventField field);
	
	IOHIDEventType IOHIDEventGetType(IOHIDEventRef event);
	
    IOHIDEventRef IOHIDEventCreateKeyboardEvent(CFAllocatorRef allocator, uint64_t time, uint16_t page, uint16_t usage, Boolean down, IOHIDEventOptionBits flags);

    IOHIDEventRef IOHIDEventCreateDigitizerEvent(CFAllocatorRef allocator, AbsoluteTime timeStamp, IOHIDDigitizerTransducerType type, uint32_t index, uint32_t identity, uint32_t eventMask, uint32_t buttonMask, IOHIDFloat x, IOHIDFloat y, IOHIDFloat z, IOHIDFloat tipPressure, IOHIDFloat barrelPressure, Boolean range, Boolean touch, IOOptionBits options);
    IOHIDEventRef IOHIDEventCreateDigitizerFingerEvent(CFAllocatorRef allocator, AbsoluteTime timeStamp, uint32_t index, uint32_t identity, uint32_t eventMask, IOHIDFloat x, IOHIDFloat y, IOHIDFloat z, IOHIDFloat tipPressure, IOHIDFloat twist, Boolean range, Boolean touch, IOOptionBits options);

    IOHIDEventSystemClientRef IOHIDEventSystemClientCreate(CFAllocatorRef allocator);
	
	IOHIDEventSystemClientRef IOHIDEventSystemClientCreateWithType(CFAllocatorRef allocator, int type, void* ctx);

    void IOHIDEventAppendEvent(IOHIDEventRef parent, IOHIDEventRef child);
    void IOHIDEventSetIntegerValue(IOHIDEventRef event, IOHIDEventField field, int value);
    void IOHIDEventSetSenderID(IOHIDEventRef event, uint64_t sender);
	uint64_t IOHIDEventGetSenderID(IOHIDEventRef event);
    void IOHIDEventSystemClientDispatchEvent(IOHIDEventSystemClientRef client, IOHIDEventRef event);
    void IOHIDEventSystemConnectionDispatchEvent(IOHIDEventSystemConnectionRef connection, IOHIDEventRef event);
	
	void IOHIDEventSetIntegerValueWithOptions(IOHIDEventRef event, IOHIDEventField field, int value, IOOptionBits options);
	
	void IOHIDEventSystemClientScheduleWithRunLoop(IOHIDEventSystemClientRef client, CFRunLoopRef runloop, CFStringRef mode);
	
	
	typedef void(*IOHIDEventSystemClientEventCallback)(void* target, void* refcon, IOHIDServiceRef queue, IOHIDEventRef event);
	
	void IOHIDEventSystemClientRegisterEventCallback(IOHIDEventSystemClientRef client, IOHIDEventSystemClientEventCallback callback, void* target, void* refcon);
	void IOHIDEventSystemClientUnregisterEventCallback(IOHIDEventSystemClientRef client);
	void IOHIDEventSystemClientUnscheduleWithRunLoop(IOHIDEventSystemClientRef client, CFRunLoopRef runloop, CFStringRef mode);
	
	IOHIDEventRef IOHIDEventCreateDigitizerFingerEventWithQuality(CFAllocatorRef allocator, AbsoluteTime timeStamp,
																  uint32_t index, uint32_t identity, uint32_t eventMask,
																  IOHIDFloat x, IOHIDFloat y, IOHIDFloat z, IOHIDFloat tipPressure, IOHIDFloat twist,
																  IOHIDFloat minorRadius, IOHIDFloat majorRadius, IOHIDFloat quality, IOHIDFloat density, IOHIDFloat irregularity,
																  Boolean range, Boolean touch, IOOptionBits options);
																  
																  
	void BKSHIDEventSetDigitizerInfo(IOHIDEventRef digitizerEvent, uint32_t contextID, uint8_t systemGestureisPossible, uint8_t isSystemGestureStateChangeEvent, CFStringRef displayUUID, CFTimeInterval initialTouchTimestamp, float maxForce);

}

#ifndef _IOKIT_HID_IOHIDEVENTTYPES_H_7
#define _IOKIT_HID_IOHIDEVENTTYPES_H_7

#ifndef _IOKIT_HID_IOHIDEVENTTYPES_H
#include <IOKit/hid/IOHIDEventTypes.h>
#endif

#define kIOHIDEventTypeGyro        20
#define kIOHIDEventTypeCompass     21

//Not kIOHIDDigitizerTransducerTypeHand
#define kIOHIDTransducerTypeHand 3

#define kIOHIDEventFieldBuiltIn 4

//  enum {
//      kIOHIDEventFieldDigitizerX = IOHIDEventFieldBase(kIOHIDEventTypeDigitizer),
//      ...
//      kIOHIDEventFieldDigitizerChildEventMask,
//      kIOHIDEventFieldDigitizerDisplayIntegrated // + 25
//  };
#define kIOHIDEventFieldDigitizerDisplayIntegrated 720921

#define kIOHIDDigitizerEventFromEdgeTip 0x00000800

#endif

@interface AutoLockX : NSObject
@property (nonatomic) IOHIDEventSystemClientRef eventSystem;
+ (id)sharedInstance;
- (void)resetLockTime;
@end
