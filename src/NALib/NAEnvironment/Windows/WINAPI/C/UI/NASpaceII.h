
// This file is part of NALib, a collection of C source code.
// Full license notice at the bottom.


// This file is an inline implmenentation (II) file which is included in the
// NACocoa.m file. This is a bit special as it is marked as a .h file but
// actually contains non-inlinenable code. See NACocoa.m for more information.
// Do not include this file anywhere else!



typedef struct NAWINAPISpace NAWINAPISpace;
struct NAWINAPISpace {
  NACoreSpace corespace;
};



NAWINAPIColor* naGetWINAPISpaceBackgroundColor(NAWINAPISpace* winapispace);



NAWINAPICallbackInfo naSpaceWINAPIProc(NAUIElement* uielement, UINT message, WPARAM wParam, LPARAM lParam){
  NAWINAPICallbackInfo info = {NA_FALSE, 0};
  RECT spacerect;
  NACoreUIElement* childelement;
  NAWINAPISpace* winapispace = (NAWINAPISpace*)uielement;
  NAWINAPIApplication* app = (NAWINAPIApplication*)naGetApplication();
  NAWINAPIColor* bgColor;

  switch(message){
  case WM_SHOWWINDOW:
  case WM_WINDOWPOSCHANGING:
  case WM_CHILDACTIVATE:
  case WM_WINDOWPOSCHANGED:
  case WM_MOVE:
  case WM_PAINT:
  case WM_NCPAINT:
  case WM_PRINTCLIENT:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLOREDIT:
  case WM_NCHITTEST:
  case WM_SETCURSOR:
    break;

  case WM_MOUSEMOVE:
    printf("hoi");
    info.result = 0;
    break;

  case WM_MOUSELEAVE:
    printf("boi\n");
    info.result = 0;
    break;

  case WM_CTLCOLORSTATIC: // Message is sent to parent space. wParam: device context, lParam HWND handle to actual control, return: background color brush
    childelement = (NACoreUIElement*)naGetUINALibEquivalent((HWND)lParam);
    switch(childelement->elementtype){
    case NA_UI_LABEL:
      if(naIsLabelEnabled(childelement)){
        SetTextColor((HDC)wParam, app->fgColor.color);
      }else{
        SetTextColor((HDC)wParam, app->fgColorDisabled.color);
      }
      bgColor = naGetWINAPISpaceBackgroundColor(uielement);
      SetBkColor((HDC)wParam, bgColor->color);
      info.result = (LRESULT)bgColor->brush;
      info.hasbeenhandeled = NA_TRUE;
      break;
    }
    break;

  case WM_ERASEBKGND: // wParam: Device context, return >1 if erasing, 0 otherwise
    GetClientRect(naGetUIElementNativeID(uielement), &spacerect);
    bgColor = naGetWINAPISpaceBackgroundColor(uielement);
    if(bgColor != &(app->bgColor)){ // Only draw if not transparent
      FillRect((HDC)wParam, &spacerect, bgColor->brush);
    }
    info.hasbeenhandeled = NA_TRUE;
    info.result = 1;
    break;

  default:
    //printf("Uncaught Space message\n");
    break;
  }
  
  return info;
}



NAWINAPIColor* naGetWINAPISpaceBackgroundColor(NAWINAPISpace* winapispace){
  NAWINAPIApplication* app = (NAWINAPIApplication*)naGetApplication();
  NAWINAPIColor* retcolor;
  NAInt alternateLevel = 0;
  NAUIElement* parent = winapispace;
  while(parent){
    if(naGetSpaceAlternateBackground(parent)){alternateLevel++;}
    parent = naGetUIElementParentSpace(parent);
  }
  switch(alternateLevel){
  case 0: retcolor = &(app->bgColor); break;
  case 1: retcolor = &(app->bgColorAlternate); break;
  case 2:
  default:
    retcolor = &(app->bgColorAlternate2); break;
  }
  return retcolor;
}

//@implementation NANativeSpace
//- (id) initWithCoreSpace:(NACoreSpace*)newcorespace frame:(NSRect)frame{
//  self = [super initWithFrame:frame];
//
//  trackingarea = [[NSTrackingArea alloc] initWithRect:[self bounds]
//      options:NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow
//      owner:self userInfo:nil];
//  [self addTrackingArea:trackingarea];
//  [self setWantsLayer:YES];
//
//  corespace = newcorespace;
//  return self;
//}
//- (void)drawRect:(NSRect)dirtyRect{
//  [super drawRect:dirtyRect];
//  if(corespace->alternatebackground){
//    [[[NSColor controlTextColor] colorWithAlphaComponent:.075] setFill];
//    NSRectFill(dirtyRect);
//  }
//}
//- (void)mouseMoved:(NSEvent*)event{
//  NA_UNUSED(event);
//  naDispatchUIElementCommand((NACoreUIElement*)corespace, NA_UI_COMMAND_MOUSE_MOVED);
//}
//- (void)mouseEntered:(NSEvent*)event{
//  NA_UNUSED(event);
//  naDispatchUIElementCommand((NACoreUIElement*)corespace, NA_UI_COMMAND_MOUSE_ENTERED);
//}
//- (void)mouseExited:(NSEvent*)event{
//  NA_UNUSED(event);
//  naDispatchUIElementCommand((NACoreUIElement*)corespace, NA_UI_COMMAND_MOUSE_EXITED);
//}
//@end



NA_DEF NASpace* naNewSpace(NASize size){
  HWND hWnd;
  DWORD exStyle;
  DWORD style;

  NAWINAPISpace* winapispace = naAlloc(NAWINAPISpace);

  exStyle = 0;
  style = WS_CHILD | WS_VISIBLE;

	hWnd = CreateWindow(
		TEXT("NASpace"), "Space", style,
		0, 0, (int)size.width, (int)size.height,
		naGetApplicationOffscreenWindow(), NULL, (HINSTANCE)naGetUIElementNativeID(naGetApplication()), NULL );
  DWORD lasterror = GetLastError();

  naInitCoreSpace(&(winapispace->corespace), hWnd);
  winapispace->corespace.alternatebackground = NA_FALSE;

  return (NASpace*)winapispace;
}



NA_DEF void naDestructSpace(NASpace* space){
  NAWINAPISpace* winapispace = (NAWINAPISpace*)space;
  naClearCoreSpace(&(winapispace->corespace));
}



NA_DEF void naAddSpaceChild(NASpace* space, NAUIElement* child, NAPos pos){
  RECT spacerect;
  RECT childrect;
  GetClientRect(naGetUIElementNativeID(space), &spacerect);
  GetClientRect(naGetUIElementNativeID(child), &childrect);
  int spaceheight = spacerect.bottom - spacerect.top;
  int childheight = childrect.bottom - childrect.top;
  SetWindowPos(naGetUIElementNativeID(child), HWND_TOP, (int)pos.x, spaceheight - (int)pos.y - childheight, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
  naSetUIElementParent(child, space);
}



NA_HDEF NARect naGetSpaceAbsoluteInnerRect(NACoreUIElement* space){
  NARect rect;
  NARect screenrect;
  RECT contentrect;
  POINT testpoint = {0, 0};
  NACoreUIElement* corespace;

  corespace = (NACoreUIElement*)space;

  GetClientRect(corespace->nativeID, &contentrect);
  ClientToScreen(corespace->nativeID, &testpoint);
  screenrect = naGetMainScreenRect();

  rect.pos.x = testpoint.x;
  rect.pos.y = screenrect.size.height - ((double)testpoint.y + (contentrect.bottom - contentrect.top));
  rect.size.width = (double)contentrect.right - (double)contentrect.left;
  rect.size.height = (double)contentrect.bottom - (double)contentrect.top;
  return rect;
}



NA_DEF void naSetSpaceAlternateBackground(NASpace* space, NABool alternate){
  NACoreSpace* corespace = (NACoreSpace*)space;
  corespace->alternatebackground = alternate;
  naRefreshUIElement(space, 0.);
}



// Copyright (c) NALib, Tobias Stamm
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.