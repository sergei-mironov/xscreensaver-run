#include <X11/X.h>
#include <X11/Xlib.h>
#include <strings.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

int usage()
{
  fprintf(stderr,"Usage: xscreensaver-run SCREENSAVER_COMMAND\n");
}

/* X11 forces you to create a blank cursor if you want
to disable the cursor. */
Cursor blankcursor(Display* dis, Window win) {
  static char data[1] = {0};
  Cursor cursor;
  Pixmap blank;
  XColor dummy;

  blank = XCreateBitmapFromData(dis, win, data, 1, 1);
  if (blank == None)
    return 0;
  cursor = XCreatePixmapCursor(dis, blank, blank, &dummy, &dummy, 0, 0);
  XFreePixmap(dis, blank);

  return cursor;
}

int main (int argc, char** argv)
{
  if(argc < 2) {
    usage();
    return 1;
  }

  if(0==strcmp(argv[1],"-h") || 0==strcmp(argv[1],"--help")) {
    usage();
    return 0;
  }

  Display* dis = XOpenDisplay(NULL);
  Window win = XCreateSimpleWindow(dis, RootWindow(dis, 0), 0, 0, 10, 10,
                                   0, BlackPixel(dis, 0), BlackPixel(dis, 0));

  Atom wm_state = XInternAtom(dis, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(dis, "_NET_WM_STATE_FULLSCREEN", False);

  XEvent xev;
  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.window = win;
  xev.xclient.message_type = wm_state;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 1;
  xev.xclient.data.l[1] = fullscreen;
  xev.xclient.data.l[2] = 0;

  XMapWindow(dis, win);

  XSendEvent(dis, DefaultRootWindow(dis), False,
    SubstructureRedirectMask | SubstructureNotifyMask, &xev);

  XDefineCursor(dis, win, blankcursor(dis,win));

  XFlush(dis);

  /* Prepare command line arguments, add -window-id <id> argument */
  char buf[20];
  snprintf(buf, sizeof(buf), "0x%lx", win);
  int i; char *args[_SC_ARG_MAX];
  args[0]=argv[1];
  for(i=2; i<argc; i++) {
    args[i-1]=argv[i];
    if(args[i-1]==NULL) {
      break;
    }
  }
  args[i-1]="-window-id";
  args[i-0]=buf;
  args[i+1]=NULL;

  /* Fork an run the screensaver */
  int chld_id=fork();
  if(!chld_id) {
    prctl(PR_SET_PDEATHSIG, SIGHUP); /* FIXME: linux-only */
    int ret = execvp(argv[1],args);
    if (ret!=0) {
      fprintf(stderr,"Failed to run the command\n"
                     "Error code %d (%m)\n", ret);
      return 2;
    }
  }

  sleep(1); /* To prevent sudden window close due to event */

  XSelectInput(dis, win,
    ExposureMask | KeyPressMask | // ButtonPress | FIXME: <-- leads to sudden exits
    StructureNotifyMask | ButtonReleaseMask |
    KeyReleaseMask | EnterWindowMask | LeaveWindowMask |
    PointerMotionMask | Button1MotionMask | VisibilityChangeMask |
    ColormapChangeMask
    );

  /* Wait for any of keyboard,mouse,etc. Exit immediately. */
  while (1) {
    XEvent event;
    XNextEvent(dis, &event);
    break;
  }

  return 0;
}


