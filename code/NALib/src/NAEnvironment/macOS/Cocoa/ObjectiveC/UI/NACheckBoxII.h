
// This file is an inline implmenentation (II) file which is included in the
// NACocoa.m file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NACocoa.m for more information.
// Do not include this file anywhere else!


@implementation NACocoaNativeCheckBox
- (id) initWithCheckBox:(NACheckBox*)newCheckBox frame:(NSRect)frame{
  self = [super initWithFrame:frame];
  
  [self setButtonType:NAButtonTypeSwitch];
  checkBox = newCheckBox;
  [self setTarget:self];
  [self setAction:@selector(onPressed:)];

  return self;
}
- (void) setText:(const NAUTF8Char*)text{
  [self setTitle:[NSString stringWithUTF8String:text]];
}
- (void) setColor:(const NABabyColor*)color{
  NSColor* nsColor;
  if(color){
    uint8 buf[4];
    naFillu8WithBabyColor(buf, *color, NA_COLOR_BUFFER_RGBA);
    nsColor = [NSColor colorWithCalibratedRed:buf[0] / 255. green:buf[1] / 255. blue:buf[2] / 255. alpha:buf[3] / 255.];
  }else{
    nsColor = [NSColor labelColor];
  }
  NSMutableAttributedString* attrString = [[NSMutableAttributedString alloc] initWithAttributedString:[self attributedTitle]];
  NSRange range = NSMakeRange(0, [attrString length]);

  [attrString beginEditing];
  NSMutableParagraphStyle* paragraphStyle = [[NSMutableParagraphStyle alloc] init];
  [paragraphStyle setParagraphStyle:[NSParagraphStyle defaultParagraphStyle]];
  paragraphStyle.alignment = [self alignment];
  [attrString addAttribute:NSForegroundColorAttributeName value:nsColor range:range];
  [attrString endEditing];
  
  [self setAttributedTitle: attrString];
}
- (void) onPressed:(id)sender{
  NA_UNUSED(sender);
  na_DispatchUIElementCommand((NA_UIElement*)checkBox, NA_UI_COMMAND_PRESSED);
}
- (void) setCheckBoxState:(NABool)state{
  [self setState:state ? NAStateOn : NAStateOff];
}
- (NABool) checkBoxState{
  return ([self state] == NAStateOn) ? NA_TRUE : NA_FALSE;
}
@end



NA_DEF NACheckBox* naNewCheckBox(const NAUTF8Char* text, NASize size){
  NACocoaNativeCheckBox* nativePtr;
  NACheckBox* checkBox = naAlloc(NACheckBox);
  NSRect frameRect = NSMakeRect((CGFloat)0., (CGFloat)0., (CGFloat)size.width, (CGFloat)size.height);
  NSRect boundrect = frameRect;
  boundrect.origin.x = 0;
  boundrect.origin.y = 0;

  nativePtr = [[NACocoaNativeCheckBox alloc] initWithCheckBox:checkBox frame:frameRect];
  na_InitCheckBox(checkBox, NA_COCOA_PTR_OBJC_TO_C(nativePtr));
  [nativePtr setText:text];
  
  return (NACheckBox*)checkBox;
}



NA_DEF void na_DestructCheckBox(NACheckBox* checkBox){
  na_ClearCheckBox(checkBox);
}



NA_DEF void naSetCheckBoxTextColor(NACheckBox* checkBox, const NABabyColor* color){
  naDefineCocoaObject(NACocoaNativeCheckBox, nativePtr, checkBox);
  [nativePtr setColor:color];
}



NA_HDEF NARect na_GetCheckBoxAbsoluteInnerRect(NA_UIElement* checkBox){
  NA_UNUSED(checkBox);
  return naMakeRectS(20, 40, 100, 50);
}



NA_DEF void naSetCheckBoxState(NACheckBox* checkBox, NABool state){
  naDefineCocoaObject(NACocoaNativeCheckBox, nativePtr, checkBox);
  [nativePtr setCheckBoxState:state];
}



NA_DEF NABool naGetCheckBoxState(NACheckBox* checkBox){
  naDefineCocoaObject(NACocoaNativeCheckBox, nativePtr, checkBox);
  return [nativePtr checkBoxState];
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
