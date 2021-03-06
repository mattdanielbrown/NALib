
// This file is an inline implmenentation (II) file which is included in the
// NACocoa.m file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NACocoa.m for more information.
// Do not include this file anywhere else!



typedef struct NACocoaButton NACocoaButton;
struct NACocoaButton{
  NAButton button;
};

NA_HAPI void na_DestructCocoaButton(NACocoaButton* cocoaButton);
NA_RUNTIME_TYPE(NACocoaButton, na_DestructCocoaButton, NA_FALSE);

@interface NACocoaNativeButton : NSButton{
  NACocoaButton* cocoaButton;
  NSTrackingArea* trackingArea;
}
@end



@implementation NACocoaNativeButton

- (id) initWithButton:(NACocoaButton*)newCocoaButton flags:(uint32)flags isText:(bool)isText frame:(NSRect)frame{
  self = [super initWithFrame:frame];

  if(naGetFlagu32(flags, NA_BUTTON_BORDERLESS)){
    if(isText && naGetFlagu32(flags, NA_BUTTON_STATEFUL)){
      [self setBezelStyle:NSBezelStyleInline]; 
      [self setBordered:YES];
    }else{
      [self setBezelStyle:NABezelStyleRounded]; 
      [self setBordered:NO];
    }
  }else{
    [self setBezelStyle:naGetFlagu32(flags, NA_BUTTON_STATEFUL) ? NABezelStyleShadowlessSquare : NABezelStyleRounded]; 
    [self setBordered:YES];
  }

  if(isText){
    [self setButtonType:naGetFlagu32(flags, NA_BUTTON_STATEFUL) ? NAButtonTypePushOnPushOff : NAButtonTypeMomentaryLight];
  }else{
    [self setButtonType:naGetFlagu32(flags, NA_BUTTON_STATEFUL) ? NAButtonTypeToggle : NAButtonTypeMomentaryChange];
  }
  
  cocoaButton = newCocoaButton;
  [self setTarget:self];
  [self setAction:@selector(onPressed:)];
  
  // todo: make this dependent on whether tracking is needed or not.
  trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
      options:(NSTrackingAreaOptions)(NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveInActiveApp)
      owner:self userInfo:nil];
  [self addTrackingArea:trackingArea];

  return self;
}

- (void)dealloc{
  NA_COCOA_RELEASE(trackingArea);
  NA_COCOA_SUPER_DEALLOC();
}

- (void) setButtonText:(const NAUTF8Char*)text{
  [self setTitle:[NSString stringWithUTF8String:text]];
}

- (void) setUIImage:(const NAUIImage*)uiImage{
  [self setImage:naCreateResolutionIndependentNativeImage(
    self,
    uiImage,
    NA_UIIMAGE_KIND_MAIN)];
  [self setAlternateImage:naCreateResolutionIndependentNativeImage(
    self,
    uiImage,
    NA_UIIMAGE_KIND_ALT)];

  [[self cell] setImageScaling:NSImageScaleNone];
}

- (void) onPressed:(id)sender{
  NA_UNUSED(sender);
  na_DispatchUIElementCommand((NA_UIElement*)cocoaButton, NA_UI_COMMAND_PRESSED);
}

- (void) mouseEntered:(NSEvent*)event{
  NA_UNUSED(event);
  na_DispatchUIElementCommand((NA_UIElement*)cocoaButton, NA_UI_COMMAND_MOUSE_ENTERED);
}

- (void) mouseExited:(NSEvent*)event{
  NA_UNUSED(event);
  na_DispatchUIElementCommand((NA_UIElement*)cocoaButton, NA_UI_COMMAND_MOUSE_EXITED);
}

- (void) setButtonState:(NABool)state{
  [self setState:state ? NAStateOn : NAStateOff];
}

- (NABool) getButtonState{
  return [self state] == NAStateOn;
}

- (void) setDefaultButton:(NABool)isDefault{
  if(isDefault){
    [self setKeyEquivalent:@"\r"];
  }else{
    [self setKeyEquivalent:@""];
  }
}
  
- (void) setVisible:(NABool)visible{
  [self setHidden:visible ? NO : YES];
}

- (NARect) getInnerRect{
  return naMakeRectWithNSRect([self frame]);
}
@end



NA_DEF NAButton* naNewTextButton(const NAUTF8Char* text, NASize size, uint32 flags){
//  #ifndef NDEBUG
//    if(naGetFlagu32(flags, NA_BUTTON_BORDERLESS))
//      naError("Borderless Text buttons should not be used as they can not be distinguished.");
//  #endif
  
  NACocoaButton* cocoaButton = naNew(NACocoaButton);

  NACocoaNativeButton* nativePtr = [[NACocoaNativeButton alloc]
    initWithButton:cocoaButton
    flags:flags
    isText:YES
    frame:naMakeNSRectWithSize(size)];
  na_InitButton((NAButton*)cocoaButton, NA_COCOA_PTR_OBJC_TO_C(nativePtr));
  
  [nativePtr setButtonText:text];
  
  return (NAButton*)cocoaButton;
}



NA_DEF NAButton* naNewTextOptionButton(const NAUTF8Char* text, NASize size){
  NACocoaButton* cocoaButton = naNew(NACocoaButton);
  
  NACocoaNativeButton* nativePtr = [[NACocoaNativeButton alloc]
    initWithButton:cocoaButton
    flags:NABezelStyleShadowlessSquare
    isText:YES
    frame:naMakeNSRectWithSize(size)];
  na_InitButton((NAButton*)cocoaButton, NA_COCOA_PTR_OBJC_TO_C(nativePtr));
  
  [nativePtr setButtonText:text];
  
  return (NAButton*)cocoaButton;
}



NA_DEF NAButton* naNewImageButton(const NAUIImage* uiImage, NASize size, uint32 flags){
  NACocoaButton* cocoaButton = naNew(NACocoaButton);
  
  NACocoaNativeButton* nativePtr = [[NACocoaNativeButton alloc]
    initWithButton:cocoaButton
    flags:flags
    isText:NO
    frame:naMakeNSRectWithSize(size)];
  na_InitButton((NAButton*)cocoaButton, NA_COCOA_PTR_OBJC_TO_C(nativePtr));
  
  [nativePtr setUIImage:uiImage];
  
  return (NAButton*)cocoaButton;
}



NA_DEF void na_DestructCocoaButton(NACocoaButton* cocoaButton){
  na_ClearButton((NAButton*)cocoaButton);
}



NA_DEF void naSetButtonImage(NAButton* button, const NAUIImage* uiImage){
  naDefineCocoaObject(NACocoaNativeButton, nativePtr, button);
  [nativePtr setUIImage:uiImage];
}



NA_DEF void naSetButtonState(NAButton* button, NABool state){
  naDefineCocoaObject(NACocoaNativeButton, nativePtr, button);
  [nativePtr setButtonState:state];
}



NA_DEF NABool naGetButtonState(NAButton* button){
  naDefineCocoaObject(NACocoaNativeButton, nativePtr, button);
  return [nativePtr getButtonState];
}



NA_DEF void naSetButtonSubmit(
  NAButton* button,
  NAReactionHandler handler,
  void* controller)
{
  naDefineCocoaObject(NACocoaNativeButton, nativePtr, button);
  [nativePtr setDefaultButton:NA_TRUE];
  naAddUIKeyboardShortcut(
    naGetUIElementWindow(button),
    naMakeKeybardStatus(NA_MODIFIER_FLAG_NONE, NA_KEYCODE_ENTER),
    handler,
    controller);
}



NA_DEF void naSetButtonAbort(
  NAButton* button,
  NAReactionHandler handler,
  void* controller)
{
  naAddUIKeyboardShortcut(
    naGetUIElementWindow(button),
    naMakeKeybardStatus(NA_MODIFIER_FLAG_NONE, NA_KEYCODE_ESC),
    handler,
    controller);
  naAddUIKeyboardShortcut(
    naGetUIElementWindow(button),
    naMakeKeybardStatus(NA_MODIFIER_FLAG_COMMAND, NA_KEYCODE_PERIOD),
    handler,
    controller);
}



NA_API void naSetButtonVisible(NAButton* button, NABool visible){
  naDefineCocoaObject(NACocoaNativeButton, nativePtr, button);
  [nativePtr setVisible:visible];
}



NA_HDEF NARect na_GetButtonAbsoluteInnerRect(NA_UIElement* button){
  naDefineCocoaObject(NACocoaNativeButton, nativePtr, button);
  NARect parentRect = naGetUIElementRect(naGetUIElementParent(button), naGetApplication(), NA_FALSE);
  NARect relRect = [nativePtr getInnerRect];
  return naMakeRect(
    naMakePos(parentRect.pos.x + relRect.pos.x, parentRect.pos.y + relRect.pos.y),
    relRect.size);
}



// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>
