#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <cairo.h>
#include <cairo-xlib.h>

// configure here
#define FONT "Arial"
#define FONT_SIZE 70
#define FONT_COLOR 1,1,1
#define FONT_BORDER 2
#define FONT_BORDER_COLOR 0,0,0
#define FONT_BORDER_COLOR_TOPMOST 1,0,0

int main(int argc,char** args) {
    struct timespec ts={0,10000000};
    Display *d;
    Window overlay;
    Window root;

    d=XOpenDisplay(NULL);

    int s=DefaultScreen(d);
    root=RootWindow(d,s);

    XCompositeRedirectSubwindows(d,root,CompositeRedirectAutomatic);
    XSelectInput(d,root,SubstructureNotifyMask);

    // total display dimensions for creating cairo surface
    int width=DisplayWidth(d,s);
    int height=DisplayHeight(d,s);

    overlay=XCompositeGetOverlayWindow(d,root);
    XserverRegion region=XFixesCreateRegion(d,NULL,0);
    XFixesSetWindowShapeRegion(d,overlay,ShapeBounding,0,0,0);
    XFixesSetWindowShapeRegion(d,overlay,ShapeInput,0,0,region);
    XFixesDestroyRegion(d,region);

    cairo_surface_t *surf=cairo_xlib_surface_create(d,overlay,DefaultVisual(d,s),width,height);
    cairo_t *cr=cairo_create(surf);

    XSelectInput(d,overlay,ExposureMask);

    // read parameters from command line
    int N=atoi(args[1]);
    int x=atoi(args[2]);
    int y=atoi(args[3]);
    int r=atoi(args[4]);
    int i;
    int shadowdiff=FONT_BORDER;

    cairo_text_extents_t extents;

    XEvent ev;
    while(1) {
        overlay=XCompositeGetOverlayWindow(d,root);

        float step=(2.0*3.1416)/((float)N);
        float cangle=3.1416/2.0;

        cairo_select_font_face(cr,FONT,CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr,FONT_SIZE);

        for(i=5;i<argc;i++){
            if(atoi(args[i])==-1) {
                N=atoi(args[i+1]);
                x=atoi(args[i+2]);
                y=atoi(args[i+3]);
                r=atoi(args[i+4]);
                i=i+5;
                step=(2.0*3.1416)/((float)N);
                cangle=3.1416/2.0;
                cairo_set_source_rgb(cr,FONT_BORDER_COLOR_TOPMOST);
            }
            else {
                if(i==5) {
                    cairo_set_source_rgb(cr,FONT_BORDER_COLOR_TOPMOST);
                }
                else {
                    cairo_set_source_rgb(cr,FONT_BORDER_COLOR);
                }
            }
            cairo_text_extents(cr,args[i],&extents);
            int X=x-(extents.width/2+extents.x_bearing);
            int Y=y-(extents.height/2+extents.y_bearing);

            // draw shadows
            cairo_move_to(cr,X+r*cosf(cangle)-shadowdiff,Y-r*sinf(cangle)+shadowdiff);
            cairo_show_text(cr,args[i]);
            cairo_move_to(cr,X+r*cosf(cangle)+shadowdiff,Y-r*sinf(cangle)+shadowdiff);
            cairo_show_text(cr,args[i]);
            cairo_move_to(cr,X+r*cosf(cangle)+shadowdiff,Y-r*sinf(cangle)-shadowdiff);
            cairo_show_text(cr,args[i]);
            cairo_move_to(cr,X+r*cosf(cangle)-shadowdiff,Y-r*sinf(cangle)-shadowdiff);
            cairo_show_text(cr,args[i]);

            // draw the text
            cairo_set_source_rgb(cr,FONT_COLOR);
            cairo_move_to(cr,X+r*cosf(cangle),Y-r*sinf(cangle));
            cairo_show_text(cr,args[i]);
            cangle-=step;
        }

        cairo_restore(cr);
        cairo_stroke(cr);

        XCompositeReleaseOverlayWindow(d,root);
        nanosleep(&ts,NULL);
    }

    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    XCloseDisplay(d);
    return 0;
}
