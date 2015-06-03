#include <stdio.h>
#include <X11/extensions/Xrandr.h>

int main() {
    Display *disp;
    XRRScreenResources *screen;
    XRROutputInfo *info;
    XRRCrtcInfo *crtc_info;
    int iscres;
    int icrtc;

    disp = XOpenDisplay(0);
    screen = XRRGetScreenResources (disp, DefaultRootWindow(disp));
    for (iscres = screen->noutput; iscres > 0; ) {
        --iscres;

        info = XRRGetOutputInfo (disp, screen, screen->outputs[iscres]);
        if (info->connection == RR_Connected) {
            for (icrtc = info->ncrtc; icrtc > 0;) {
                --icrtc;

                crtc_info = XRRGetCrtcInfo (disp, screen, screen->crtcs[icrtc]);
                fprintf(stderr, "==> %dx%d+%dx%d\n", crtc_info->x, crtc_info->y, crtc_info->width, crtc_info->height);

                XRRFreeCrtcInfo(crtc_info);
            }
        }
        XRRFreeOutputInfo (info);
    }
    XRRFreeScreenResources(screen);

    return 0;
}
