
// This file is an inline implmenentation (II) file which is included in the
// NACocoa.m file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NACocoa.m for more information.
// Do not include this file anywhere else!



//@implementation MDVerticallyCenteredTextFieldCell
//
//- (NSRect)adjustedFrameToVerticallyCenterText:(NSRect)rect {
////    CGFloat fontSize = self.font.boundingRectForFont.size.height;
////    NSRect boundRect = [[self font] boundingRectForFont];
////    CGFloat ascender = [[self font] ascender];
////    CGFloat capHeight = [[self font] capHeight];
////    CGFloat descender = [[self font] descender];
////    CGFloat xHeight = [[self font] xHeight];
////    CGFloat test = fontSize - ascender + descender;
////    CGFloat offset = 15 - (fontSize + (fontSize + boundRect.origin.y - ascender + descender));
////    CGFloat offset = 18 - (floor(fontSize) + floor(boundRect.origin.y));
//    CGFloat offset = 0;
//    return NSMakeRect(rect.origin.x, offset, rect.size.width, rect.size.height - offset);
////    return NSMakeRect(rect.origin.x, 15 - (fontSize + boundRect.origin.y), rect.size.width, fontSize);
//}
//- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView
//         editor:(NSText *)editor delegate:(id)delegate event:(NSEvent *)event {
//    [super editWithFrame:[self adjustedFrameToVerticallyCenterText:aRect]
//          inView:controlView editor:editor delegate:delegate event:event];
//}
//
//- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView
//                 editor:(NSText *)editor delegate:(id)delegate 
//                  start:(NSInteger)start length:(NSInteger)length {
//
//    [super selectWithFrame:[self adjustedFrameToVerticallyCenterText:aRect]
//                    inView:controlView editor:editor delegate:delegate
//                     start:start length:length];
//}
//
////- (void)drawInteriorWithFrame:(NSRect)frame inView:(NSView *)view {
////    [super drawInteriorWithFrame:[self adjustedFrameToVerticallyCenterText:frame] inView:view];
////}
//- (void)drawWithFrame:(NSRect)frame inView:(NSView *)view {
////    CGFontRef cgfont = CTFontCopyGraphicsFont([self font], nil);
////  CGFloat cgAscent = CGFontGetAscent(cgfont);
//
////  NSLayoutManager* layout = [[NSLayoutManager alloc] init];
////  CGFloat defaultLineHeight = [layout defaultLineHeightForFont:[self font]];
////  CGFloat baselineOffset = [layout defaultBaselineOffsetForFont:[self font]];
////
////    NSRect titleRect = [self titleRectForBounds:frame];
////    CGFloat fontHeight = self.font.boundingRectForFont.size.height;
////    NSRect boundRect = [[self font] boundingRectForFont];
////    CGFloat origin = boundRect.origin.y;
////    CGFloat ascender = ([[self font] ascender]);
////    CGFloat capHeight = [[self font] capHeight];
////    CGFloat descender = ([[self font] descender]);
////    CGFloat xHeight = [[self font] xHeight];
////    CGFloat underlinePos = [[self font] underlinePosition];
////    CGFloat leading = [[self font] leading];
////    NSRect glyphrect = [[self font] boundingRectForCGGlyph:'x'];
////    const CGFloat* matrix = [[self font] matrix];
////CGFloat baseline = ceil(NSMinY(titleRect) + [[self font] ascender]);
////    
////    CGFloat testleading = leading;
////    if(testleading < 0){testleading = 0;}
////    testleading = floor(testleading + .5);
////    
////    CGFloat testlineheight = floor(ascender + .5) - ceil(descender - .5) + leading;
////    CGFloat testDelta = 0;
////    if(leading <= 0){
////      testDelta = floor (0.2 * testlineheight + 0.5);
////    }
////    
////    CGFloat test = defaultLineHeight;
////
////    printf("%f" NA_NL, baselineOffset);
////
////    [[NSColor yellowColor] setFill];
////    boundRect.origin.y = 0;
//////    boundRect.size.height = defaultLineHeight;
////    boundRect.size.width += 20;
////    NSRectFill(boundRect);
////    boundRect.size.width -= 20;
////
////    [[NSColor orangeColor] setFill];
////    boundRect.origin.y = 0;
////    boundRect.size.height = ascender;
////    NSRectFill(boundRect);
////
////    [[NSColor redColor] setFill];
////    boundRect.origin.y = ascender;
////    boundRect.size.height = -descender;
////    NSRectFill(boundRect);
////
////    [[NSColor orangeColor] setFill];
////    boundRect.origin.y = test;
////    boundRect.size.height = ascender;
////    NSRectFill(boundRect);
////
////    [[NSColor redColor] setFill];
////    boundRect.origin.y = test + ascender;
////    boundRect.size.height = -descender;
////    NSRectFill(boundRect);
//    
//    [super drawWithFrame:[self adjustedFrameToVerticallyCenterText:frame] inView:view];
//}
//
//@end

@implementation NACocoaNativeLabel
- (id) initWithLabel:(NALabel*)newLabel frame:(NSRect)frame{
  self = [super initWithFrame:frame];
//  [self setCell:[[MDVerticallyCenteredTextFieldCell alloc] initTextCell:@"Wurst"]];
  [self setSelectable:YES];
  [self setEditable:NO];
  [self setBordered:NO];
//  [self setBackgroundColor:[NSColor colorWithCalibratedRed:(CGFloat)0. green:(CGFloat)0. blue:(CGFloat)1. alpha:(CGFloat).1]];
  [self setDrawsBackground:NO];
  [self setTextColor:[NSColor labelColor]];
  [[self cell] setLineBreakMode:NSLineBreakByWordWrapping];
  [self setFont:[NSFont labelFontOfSize:[NSFont systemFontSize]]];
  label = newLabel;
  return self;
}
- (void) setText:(const NAUTF8Char*)text{
  [self setStringValue:[NSString stringWithUTF8String:text]];
}
- (void) setColor:(const NABabyColor*)color{
  if(color){
    uint8 buf[4];
    naFillu8WithBabyColor(buf, *color, NA_COLOR_BUFFER_RGBA);
    [self setTextColor:[NSColor colorWithCalibratedRed:buf[0] / 255. green:buf[1] / 255. blue:buf[2] / 255. alpha:buf[3] / 255.]];
  }else{
    [self setTextColor:[NSColor labelColor]];
  }
}
- (void) setLink:(const NAUTF8Char*)url{
  NSMutableAttributedString* attrString;
  NSRange range;
  NSURL* nsurl;
  NSMutableParagraphStyle* paragraphStyle;
  
  [self setAllowsEditingTextAttributes: YES];
  [self setSelectable: YES];

  attrString = NA_COCOA_AUTORELEASE([[NSMutableAttributedString alloc] initWithString: [self stringValue]]);
  range = NSMakeRange(0, [attrString length]);

  nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
  [attrString beginEditing];
  [attrString addAttribute:NSLinkAttributeName value:[nsurl absoluteString] range:range];
  paragraphStyle = [[NSMutableParagraphStyle alloc] init];
  [paragraphStyle setParagraphStyle:[NSParagraphStyle defaultParagraphStyle]];
  paragraphStyle.alignment = [self alignment];
  [attrString addAttribute:NSParagraphStyleAttributeName value:paragraphStyle range:range];
  [attrString addAttribute:NSUnderlineStyleAttributeName value:[NSNumber numberWithInt:NSUnderlineStyleSingle] range:range];
  [attrString addAttribute:NSForegroundColorAttributeName value:[NSColor linkColor] range:range];
  [attrString endEditing];
  
  [self setAttributedStringValue: attrString];
}
- (void) setLabelEnabled:(NABool)enabled{
  [self setAlphaValue:enabled ? (CGFloat)1. : (CGFloat).35];
}
- (void) setTextAlignment:(NATextAlignment) alignment{
  [self setAlignment:getNSTextAlignmentWithAlignment(alignment)];
}
- (void) setFontKind:(NAFontKind)kind{
  [self setFont:NA_COCOA_PTR_C_TO_OBJC(na_GetFontWithKind(kind))];
}
@end



NA_DEF NALabel* naNewLabel(const NAUTF8Char* text, NASize size){
  NALabel* label = naAlloc(NALabel);

  NSRect frameRect = NSMakeRect((CGFloat)0., (CGFloat)0., (CGFloat)size.width, (CGFloat)size.height);
  NACocoaNativeLabel* nativePtr = [[NACocoaNativeLabel alloc] initWithLabel:label frame:frameRect];
  na_InitLabel(label, NA_COCOA_PTR_OBJC_TO_C(nativePtr));
  naSetLabelText(label, text);
  
  return (NALabel*)label;
}



NA_DEF void na_DestructLabel(NALabel* label){
  na_ClearLabel(label);
}



NA_DEF void naSetLabelText(NALabel* label, const NAUTF8Char* text){
  naDefineCocoaObject(NACocoaNativeLabel, nativePtr, label);
  [cocoaLabel setText:text];
}



NA_DEF void naSetLabelTextColor(NALabel* label, const NABabyColor* color){
  naDefineCocoaObject(NACocoaNativeLabel, nativePtr, label);
  [cocoaLabel setColor:color];
}



NA_DEF void naSetLabelLink(NALabel* label, const NAUTF8Char* url){
  naDefineCocoaObject(NACocoaNativeLabel, nativePtr, label);
  [cocoaLabel setLink: url];
}



NA_DEF void naSetLabelEnabled(NALabel* label, NABool enabled){
  naDefineCocoaObject(NACocoaNativeLabel, nativePtr, label);
  [cocoaLabel setLabelEnabled:enabled];
}



NA_DEF void naSetLabelTextAlignment(NALabel* label, NATextAlignment alignment){
  naDefineCocoaObject(NACocoaNativeLabel, nativePtr, label);
  [cocoaLabel setTextAlignment:alignment];
}



NA_DEF void naSetLabelFontKind(NALabel* label, NAFontKind kind){
  naDefineCocoaObject(NACocoaNativeLabel, nativePtr, label);
  [cocoaLabel setFontKind:kind];
}



NA_HDEF NARect na_GetLabelAbsoluteInnerRect(NA_UIElement* label){
  NA_UNUSED(label);
  return naMakeRectS(20, 40, 100, 50);
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
