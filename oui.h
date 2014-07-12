/*
OUI - A minimal immediate GUI handling & layouting library

Copyright (c) 2014 Leonard Ritter <leonard.ritter@duangle.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _UI_H_
#define _UI_H_

/*

OUI (spoken like the french "oui" for "yes") is a single-header library for
layouting GUI elements and handling their user input.

OUI has no widget types; instead, it provides only one kind of element, "Items",
which can be expanded to behave as containers, buttons, sliders, radio
buttons, and so on.

Together with a set of widget drawing routines it can be used to build flowing
user interfaces; the intended use is for bootstrap situations where only basic
UI services are needed.

*/

// item states as returned by uiGetState()

// the item is inactive
#define UI_COLD   0x0000
// the item is inactive, but the cursor is hovering over this item
#define UI_HOT    0x0001
// the item is toggled or activated (depends on item kind)
#define UI_ACTIVE 0x0002
// the item is unresponsive
#define UI_FROZEN 0x0003

// maximum number of items that may be added
#define UI_MAX_ITEMS 4096
// maximum size in bytes reserved for storage of application dependent data
// as passed to uiAllocData().
#define UI_MAX_BUFFERSIZE 1048576
// maximum size in bytes of a single data buffer passed to uiAllocData().
#define UI_MAX_DATASIZE 4096
// maximum depth of nested containers
#define UI_MAX_DEPTH 64

typedef unsigned int UIuint;

// opaque UI context
typedef struct UIcontext UIcontext;

// application defined context handle
typedef unsigned long long UIhandle;

// layout flags
typedef enum UIlayoutFlags {
    UI_LEFT = 1,
    UI_TOP = 2,
    UI_RIGHT = 4,
    UI_DOWN = 8,
    UI_HFILL = 5,
    UI_VFILL = 10,
    UI_HCENTER = 0,
    UI_VCENTER = 0,
    UI_CENTER = 0,
    UI_FILL = 15,
} UIlayoutFlags;

// event flags
typedef enum UIevent {
    // on button 0 down
    UI_BUTTON0_DOWN = 0x01,
    // on button 0 up
    UI_BUTTON0_UP = 0x02,
    // on button 0 up while item is hovered
    UI_BUTTON0_HOT_UP = 0x04,
    // item is being captured (button 0 constantly pressed)
    UI_BUTTON0_CAPTURE = 0x08,
} UIevent;

// handler callback; event is one of UI_EVENT_*
typedef void (*UIhandler)(int item, UIevent event);

// for cursor positions, mainly
typedef struct UIvec2 {
    union {
        int v[2];
        struct { int x, y; };
    };
} UIvec2;

// layout rectangle
typedef struct UIrect {
    union {
        int v[4];
        struct { int x, y, w, h; };
    };
} UIrect;

// unless declared otherwise, all operations have the complexity O(1).

// create a new UI context; call uiMakeCurrent() to make this context the
// current context.
UIcontext *uiCreateContext();

// select an UI context as the current context; a context must always be 
// selected before using any of the other UI functions
void uiMakeCurrent(UIcontext *ctx);

// release the memory of an UI context created with uiCreateContext(); if the
// context is the current context, the current context will be set to NULL
void uiDestroyContext(UIcontext *ctx);

// sets a mouse or gamepad button as pressed/released
// button is in the range 0..63 and maps to an application defined input
// source.
// enabled is 1 for pressed, 0 for released
void uiSetButton(int button, int enabled);

// returns the current state of an application dependent input button
// as set by uiSetButton().
// the function returns 1 if the button has been set to pressed, 0 for released.
int uiGetButton(int button);

// sets the current cursor position (usually belonging to a mouse) to the
// screen coordinates at (x,y)
void uiSetCursor(int x, int y);

// returns the current cursor position in screen coordinates as set by 
// uiSetCursor()
UIvec2 uiGetCursor();

// returns the offset of the cursor relative to the last call to uiProcess()
UIvec2 uiGetCursorDelta();

// returns the offset of the cursor relative to the beginning point of a drag
// operation.
UIvec2 uiGetCursorStartDelta();

// clear the item buffer; uiClear() should be called before each UI declaration
// to avoid concatenation of the same UI multiple times.
// After the call, all previously declared item IDs are invalid, and all
// application dependent context data has been freed.
void uiClear();

// create a new UI item and return the new items ID.
int uiItem();

// assign an item to a container.
// parent is the item ID of the containing item; an item ID of 0 refers to the
// root item.
// if item is already assigned to a parent, an assertion will be thrown.
void uiSetParent(int item, int parent);

// layout all added items and update the internal state according to the
// current cursor position and button states.
// It is safe to immediately draw the items after a call to uiProcess().
// this is an O(N) operation for N = number of declared items.
void uiProcess();

// returns the number of child items a container item contains. If the item 
// is not a container or does not contain any items, 0 is returned.
// if item is 0, the child item count of the root item will be returned.
int uiGetChildCount(int item);

// returns the first child item of a container item. If the item is not
// a container or does not contain any items, -1 is returned.
// if item is 0, the first child item of the root item will be returned.
int uiFirstChild(int item);

// returns the last child item of a container item. If the item is not
// a container or does not contain any items, -1 is returned.
// if item is 0, the last child item of the root item will be returned.
int uiLastChild(int item);

// returns an items parent container item.
// if item is 0, -1 will be returned.
int uiParent(int item);

// returns an items child index relative to its parent. If the item is the
// first item, the return value is 0; If the item is the last item, the return
// value is equivalent to uiGetChildCount(uiParent(item))-1.
// if item is 0, 0 will be returned.
int uiGetChildId(int item);

// returns an items next sibling in the list of the parent containers children.
// if item is 0 or the item is the last child item, -1 will be returned.
int uiNextSibling(int item);

void uiSetSize(int item, int w, int h);
void uiSetLayout(int item, int flags);
void uiSetMargins(int item, int t, int r, int b, int l);
void uiSetRelativeTo(int item, int titem, int ritem, int bitem, int litem);

// returns the items layout rectangle relative to the parent. If uiGetRect()
// is called before uiProcess(), the values of the returned rectangle are
// undefined.
UIrect uiGetRect(int item);

// allocate space for application-dependent context data and return the pointer
// if successful. If no data has been allocated, a new pointer is returned. 
// Otherwise, an assertion is thrown.
// The memory of the pointer is managed by the UI context.
void *uiAllocData(int item, int size);

// return the application-dependent context data for an item as passed to
// uiAllocData(). The memory of the pointer is managed by the UI context
// and must not be altered.
const void *uiGetData(int item);

// set the application-dependent handle of an item.
// handle is an application defined 64-bit handle. If handle is 0, the item
// will not be interactive.
void uiSetHandle(int item, UIhandle handle);

// return the application-dependent handle of the item as passed to uiSetHandle().
UIhandle uiGetHandle(int item);

// return the current state of the item. This state is only valid after
// a call to uiProcess().
// The returned value is one of UI_COLD, UI_HOT, UI_ACTIVE.
int uiGetState(int item);

// set the handler callback for an interactive item. 
// flags is a combination of UI_EVENT_* and designates for which events the 
// handler should be called. 
void uiSetHandler(int item, UIhandler handler, int flags);

// return the handler callback for an item as passed to uiSetHandler()
UIhandler uiGetHandler(int item);

// return the handler flags for an item as passed to uiSetHandler()
int uiGetHandlerFlags(int item);

#endif // _UI_H_

#define UI_IMPLEMENTATION
#ifdef UI_IMPLEMENTATION

#include <assert.h>

#ifdef _MSC_VER
	#pragma warning (disable: 4996) // Switch off security warnings
	#pragma warning (disable: 4100) // Switch off unreferenced formal parameter warnings
	#ifdef __cplusplus
	#define UI_INLINE inline
	#else
	#define UI_INLINE
	#endif
#else
	#define UI_INLINE inline
#endif

#define UI_MAX_KIND 16

typedef struct UIitem {
    // declaration independent unique handle (for persistence)
    UIhandle handle;
    // handler
    UIhandler handler;
    
    // container structure
    
    // number of kids
    int numkids;
    // index of first kid
    int firstkid;
    // index of last kid
    int lastkid;
    
    // child structure
    
    // parent item
    int parent;
    // index of kid relative to parent
    int kidid;
    // index of next sibling with same parent
    int nextitem;
    
    // one or multiple of UIlayoutFlags
    int layout_flags;
    // size
    UIvec2 size;
    int visited;
    // margin offsets, interpretation depends on flags
    int margins[4];
    // neighbors to position borders to
    int relto[4];
    // relative rect
    UIrect rect;
    
    // attributes
    
    // index of data or -1 for none
    int data;
    // size of data
    int datasize;
    // a combination of UIevents
    int event_flags;
} UIitem;

typedef enum UIstate {
    UI_STATE_IDLE = 0,
    UI_STATE_CAPTURE,
} UIstate;

struct UIcontext {
    // button state in this frame
    unsigned long long buttons;
    // button state in the previous frame
    unsigned long long last_buttons;
    
    // where the cursor was at the beginning of the active state
    UIvec2 start_cursor;
    // where the cursor was last frame
    UIvec2 last_cursor;
    // where the cursor is currently
    UIvec2 cursor;
    
    UIhandle hot_handle;
    UIhandle active_handle;
    int hot_item;
    int active_item;
    UIstate state;
    
    int count;    
    UIitem items[UI_MAX_ITEMS];
    int datasize;
    unsigned char data[UI_MAX_BUFFERSIZE];
};

UI_INLINE int ui_max(int a, int b) {
    return (a>b)?a:b;
}

UI_INLINE int ui_min(int a, int b) {
    return (a<b)?a:b;
}

static UIcontext *ui_context = NULL;

UIcontext *uiCreateContext() {
    UIcontext *ctx = (UIcontext *)malloc(sizeof(UIcontext));
    memset(ctx, 0, sizeof(UIcontext));
    return ctx;
}

void uiMakeCurrent(UIcontext *ctx) {
    ui_context = ctx;
    if (ui_context)
        uiClear();
}

void uiDestroyContext(UIcontext *ctx) {
    if (ui_context == ctx)
        uiMakeCurrent(NULL);
    free(ctx);
}

void uiSetButton(int button, int enabled) {
    assert(ui_context);
    unsigned long long mask = 1ull<<button;
    // set new bit
    ui_context->buttons = (enabled)?
        (ui_context->buttons | mask):
        (ui_context->buttons & ~mask);
}

int uiGetLastButton(int button) {
    assert(ui_context);
    return (ui_context->last_buttons & (1ull<<button))?1:0;
}

int uiGetButton(int button) {
    assert(ui_context);
    return (ui_context->buttons & (1ull<<button))?1:0;
}

int uiButtonPressed(int button) {
    assert(ui_context);
    return !uiGetLastButton(button) && uiGetButton(button);
}

int uiButtonReleased(int button) {
    assert(ui_context);
    return uiGetLastButton(button) && !uiGetButton(button);
}

void uiSetCursor(int x, int y) {
    assert(ui_context);
    ui_context->cursor.x = x;
    ui_context->cursor.y = y;
}

UIvec2 uiGetCursor() {
    assert(ui_context);
    return ui_context->cursor;
}

UIvec2 uiGetCursorDelta() {
    assert(ui_context);
    UIvec2 result = {{{
        ui_context->cursor.x - ui_context->last_cursor.x,
        ui_context->cursor.y - ui_context->last_cursor.y
    }}};
    return result;
}

UIvec2 uiGetCursorStartDelta() {
    assert(ui_context);
    UIvec2 result = {{{
        ui_context->cursor.x - ui_context->start_cursor.x,
        ui_context->cursor.y - ui_context->start_cursor.y
    }}};
    return result;
}

UIitem *uiItemPtr(int item) {
    assert(ui_context && (item >= 0) && (item < ui_context->count));
    return ui_context->items + item;
}

void uiClear() {
    assert(ui_context);
    ui_context->count = 1;
    ui_context->datasize = 0;
    ui_context->hot_item = -1;
    ui_context->active_item = -1;

    // init root object    
    UIitem *item = ui_context->items;
    memset(item, 0, sizeof(UIitem));
    item->parent = -1;
    item->firstkid = -1;
    item->lastkid = -1;
    item->nextitem = -1;
    
    item->data = -1;
}

int uiItem() {
    assert(ui_context && (ui_context->count < UI_MAX_ITEMS));
    int idx = ui_context->count++;
    UIitem *item = uiItemPtr(idx);
    memset(item, 0, sizeof(UIitem));
    item->parent = -1;
    item->firstkid = -1;
    item->lastkid = -1;
    item->nextitem = -1;
    item->data = -1;
    for (int i = 0; i < 4; ++i)
        item->relto[i] = -1;
    return idx;
}

void uiSetParent(int item, int parent) {
    assert(item > 0);
    assert(uiParent(item) == -1);
    UIitem *pitem = uiItemPtr(item);
    UIitem *pparent = uiItemPtr(parent);
    pitem->parent = parent;
    pitem->kidid = pparent->numkids++;
    if (pparent->lastkid < 0) {
        pparent->firstkid = item;
        pparent->lastkid = item;
    } else {
        uiItemPtr(pparent->lastkid)->nextitem = item;
        pparent->lastkid = item;
    }
}

void uiSetSize(int item, int w, int h) {
    UIitem *pitem = uiItemPtr(item);
    pitem->size.x = w;
    pitem->size.y = h;
}

void uiSetLayout(int item, int flags) {
    UIitem *pitem = uiItemPtr(item);
    pitem->layout_flags = flags;
}

void uiSetMargins(int item, int l, int t, int r, int b) {
    UIitem *pitem = uiItemPtr(item);
    pitem->margins[0] = l;
    pitem->margins[1] = t;
    pitem->margins[2] = r;
    pitem->margins[3] = b;
}

void uiSetRelativeTo(int item, int litem, int titem, int ritem, int bitem) {
    UIitem *pitem = uiItemPtr(item);
    pitem->relto[0] = litem;
    pitem->relto[1] = titem;
    pitem->relto[2] = ritem;
    pitem->relto[3] = bitem;
}

UI_INLINE int uiComputeChainSize(UIitem *pkid, int dim) {
    UIitem *pitem = pkid;
    int wdim = dim+2;
    int size = pitem->rect.v[wdim];
    int it = 0;
    pitem->visited |= 1<<dim;
    // traverse along left neighbors
    while ((pitem->layout_flags>>dim) & UI_LEFT) {
        size += pitem->margins[dim];
        if (pitem->relto[dim] < 0) break;
        pitem = uiItemPtr(pitem->relto[dim]);
        pitem->visited |= 1<<dim;
        size += pitem->rect.v[wdim];
        it++;
        assert(it<1000000); // infinite loop
    }
    // traverse along right neighbors
    pitem = pkid;
    it = 0;
    while ((pitem->layout_flags>>dim) & UI_RIGHT) {
        size += pitem->margins[wdim];
        if (pitem->relto[wdim] < 0) break;
        pitem = uiItemPtr(pitem->relto[wdim]);
        pitem->visited |= 1<<dim;
        size += pitem->rect.v[wdim];
        it++;
        assert(it<1000000); // infinite loop
    }
    return size;
}

UI_INLINE void uiComputeSizeDim(UIitem *pitem, int dim) {
    int wdim = dim+2;
    if (pitem->size.v[dim]) {
        pitem->rect.v[wdim] = pitem->size.v[dim];
    } else {
        int size = 0;
        int kid = pitem->firstkid;
        while (kid > 0) {
            UIitem *pkid = uiItemPtr(kid);
            if (!(pkid->visited & (1<<dim))) {
                size = ui_max(size, uiComputeChainSize(pkid, dim));
            }
            kid = uiNextSibling(kid);
        }
    
        pitem->rect.v[wdim] = size;
    }
}

static void uiComputeBestSize(int item) {
    UIitem *pitem = uiItemPtr(item);
    pitem->visited = 0;
    // children expand the size
    int kid = uiFirstChild(item);
    while (kid > 0) {
        uiComputeBestSize(kid);
        kid = uiNextSibling(kid);
    }
    
    uiComputeSizeDim(pitem, 0);
    uiComputeSizeDim(pitem, 1);
}

static void uiLayoutChildItem(UIitem *pparent, UIitem *pitem, int dim) {
    if (pitem->visited & (4<<dim)) return;
    pitem->visited |= (4<<dim);
    
    int wdim = dim+2;
    
    int wl = 0;
    int wr = pparent->rect.v[wdim];
    
    int flags = pitem->layout_flags>>dim;
    if ((flags & UI_LEFT) && (pitem->relto[dim] > 0)) {
        UIitem *pl = uiItemPtr(pitem->relto[dim]);
        uiLayoutChildItem(pparent, pl, dim);
        wl = pl->rect.v[dim]+pl->rect.v[wdim];
        wr -= wl;
    }
    if ((flags & UI_RIGHT) && (pitem->relto[wdim] > 0)) {
        UIitem *pl = uiItemPtr(pitem->relto[wdim]);
        uiLayoutChildItem(pparent, pl, dim);
        wr = pl->rect.v[dim]-wl;
    }
    
    switch(flags & UI_HFILL) {
    default:
    case UI_HCENTER: {
        pitem->rect.v[dim] = wl+(wr-pitem->rect.v[wdim])/2+pitem->margins[dim];
    } break;
    case UI_LEFT: {
        pitem->rect.v[dim] = wl+pitem->margins[dim];
    } break;
    case UI_RIGHT: {
        pitem->rect.v[dim] = wl+wr-pitem->rect.v[wdim]-pitem->margins[wdim];
    } break;
    case UI_HFILL: {
        pitem->rect.v[dim] = wl+pitem->margins[dim];
        pitem->rect.v[wdim] = wr-pitem->margins[dim]-pitem->margins[wdim];
    } break;
    }    
}

UI_INLINE void uiLayoutItemDim(UIitem *pitem, int dim) {
    int kid = pitem->firstkid;
    while (kid > 0) {
        UIitem *pkid = uiItemPtr(kid);
        uiLayoutChildItem(pitem, pkid, dim);
        kid = uiNextSibling(kid);
    }
}

static void uiLayoutItem(int item) {
    UIitem *pitem = uiItemPtr(item);
    
    uiLayoutItemDim(pitem, 0);
    uiLayoutItemDim(pitem, 1);
    
    int kid = uiFirstChild(item);
    while (kid > 0) {
        uiLayoutItem(kid);
        kid = uiNextSibling(kid);
    }
}

UIrect uiGetRect(int item) {
    return uiItemPtr(item)->rect;
}

int uiFirstChild(int item) {
    return uiItemPtr(item)->firstkid;
}

int uiLastChild(int item) {
    return uiItemPtr(item)->lastkid;
}

int uiNextSibling(int item) {
    return uiItemPtr(item)->nextitem;
}

int uiParent(int item) {
    return uiItemPtr(item)->parent;
}

const void *uiGetData(int item) {
    UIitem *pitem = uiItemPtr(item);
    if (pitem->data < 0) return NULL;
    return ui_context->data + pitem->data;
}

void *uiAllocData(int item, int size) {
    assert((size > 0) && (size < UI_MAX_DATASIZE));
    UIitem *pitem = uiItemPtr(item);
    assert(pitem->data < 0);
    assert((ui_context->datasize+size) <= UI_MAX_BUFFERSIZE);
    pitem->data = ui_context->datasize;
    ui_context->datasize += size;
    return ui_context->data + pitem->data;
}

void uiSetHandle(int item, UIhandle handle) {
    uiItemPtr(item)->handle = handle;
    if (handle) {
        if (handle == ui_context->hot_handle)
            ui_context->hot_item = item;
        if (handle == ui_context->active_handle)
            ui_context->active_item = item;
    }
}

UIhandle uiGetHandle(int item) {
    return uiItemPtr(item)->handle;
}

void uiSetHandler(int item, UIhandler handler, int flags) {
    UIitem *pitem = uiItemPtr(item);
    pitem->handler = handler;
    pitem->event_flags = flags;
}

UIhandler uiGetHandler(int item) {
    return uiItemPtr(item)->handler;
}

int uiGetHandlerFlags(int item) {
    return uiItemPtr(item)->event_flags;
}

int uiGetChildId(int item) {
    return uiItemPtr(item)->kidid;
}

int uiGetChildCount(int item) {
    return uiItemPtr(item)->numkids;
}

int uiFindItem(int item, int x, int y) {
    UIrect rect = uiGetRect(item);
    x -= rect.x;
    y -= rect.y;
    if ((x>=0)
     && (y>=0)
     && (x<rect.w)
     && (y<rect.h)) {
        int kid = uiFirstChild(item);
        while (kid > 0) {
            int best_hit = uiFindItem(kid,x,y);
            if (best_hit >= 0) return best_hit;
            kid = uiNextSibling(kid);
        }
        return item;
    }
    return -1;
}

void uiNotifyItem(int item, UIevent event) {
    UIitem *pitem = uiItemPtr(item);
    if (pitem->handler && (pitem->event_flags & event)) {
        pitem->handler(item, event);
    }
}

void uiProcess() {
    uiComputeBestSize(0);
    // position root element rect
    uiItemPtr(0)->rect.x = uiItemPtr(0)->margins[0];
    uiItemPtr(0)->rect.y = uiItemPtr(0)->margins[1];    
    uiLayoutItem(0);
    int hot = uiFindItem(0, ui_context->cursor.x, ui_context->cursor.y);

    switch(ui_context->state) {
    default:
    case UI_STATE_IDLE: {
        if (uiGetButton(0)) {
            ui_context->start_cursor = ui_context->cursor;
            ui_context->last_cursor = ui_context->cursor;
            ui_context->hot_item = -1;
            ui_context->active_item = hot;
            if (ui_context->active_item >= 0) {
                uiNotifyItem(ui_context->active_item, UI_BUTTON0_DOWN);
            }            
            ui_context->state = UI_STATE_CAPTURE;            
        } else {
            ui_context->hot_item = hot;
        }
    } break;
    case UI_STATE_CAPTURE: {
        if (!uiGetButton(0)) {
            if (ui_context->active_item >= 0) {
                uiNotifyItem(ui_context->active_item, UI_BUTTON0_UP);
                if (ui_context->active_item == hot) {
                    uiNotifyItem(ui_context->active_item, UI_BUTTON0_HOT_UP);
                }
            }
            ui_context->active_item = -1;
            ui_context->state = UI_STATE_IDLE;
        } else {
            if (ui_context->active_item >= 0) {
                uiNotifyItem(ui_context->active_item, UI_BUTTON0_CAPTURE);
            }            
            if (hot == ui_context->active_item)
                ui_context->hot_item = hot;
            else
                ui_context->hot_item = -1;
        }
    } break;
    }
    
    ui_context->last_cursor = ui_context->cursor;
    ui_context->hot_handle = (ui_context->hot_item>=0)?
        uiGetHandle(ui_context->hot_item):0;
    ui_context->active_handle = (ui_context->active_item>=0)?
        uiGetHandle(ui_context->active_item):0;
}

int uiIsActive(int item) {
    assert(ui_context);
    return ui_context->active_item == item;
}

int uiIsHot(int item) {
    assert(ui_context);
    return ui_context->hot_item == item;
}

int uiGetState(int item) {
    UIitem *pitem = uiItemPtr(item);
    if (uiIsActive(item)) {
        if (pitem->event_flags & (UI_BUTTON0_CAPTURE|UI_BUTTON0_UP)) return UI_ACTIVE;
        if ((pitem->event_flags & UI_BUTTON0_HOT_UP)
            && uiIsHot(item)) return UI_ACTIVE;
        return UI_COLD;
    } else if (uiIsHot(item)) {
        return UI_HOT;
    }
    return UI_COLD;
}

#endif // UI_IMPLEMENTATION