#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <strings.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

static int g_verbose = 0;

#define verbose_printf(args...) \
  do { \
    if(g_verbose) { \
      fprintf(stderr, args); \
    } \
  } while(0)

void usage()
{
  fprintf(stderr, "Usage: xscreensaver-run [-h] [-v] -- SCREENSAVER_CMD ARGS..\n");
}

/* X11 forces you to create a blank cursor if you want
to disable the cursor. */
Cursor blankcursor(Display* dis, Window win)
{
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
  int react_mouse = 0;
  int react_visibility = 0;
  int opt;
  while ((opt = getopt(argc, argv, "hv")) != -1) {
    switch (opt) {
    case 'h': usage(); exit(EXIT_FAILURE);
    case 'v': g_verbose = 1; break;
    // case 'M': react_mouse = 1; break;
    // case 'V': react_visibility = 1; break;
    default:
        usage();
        exit(EXIT_FAILURE);
    }
  }

  if(optind >= argc) {
    usage();
    exit(EXIT_FAILURE);
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

  XFlush(dis);

  /* Prepare command line arguments, add -window-id <id> argument */
  int command_idx = optind;
  char buf[20];
  snprintf(buf, sizeof(buf), "0x%lx", win);
  char *args[_SC_ARG_MAX];
  for(; optind<argc; optind++) {
    args[optind-command_idx]=argv[optind];
  }
  args[(optind++)-command_idx]="-window-id";
  args[(optind++)-command_idx]=buf;
  args[(optind++)-command_idx]=NULL;

  /* Fork an run the screensaver */
  int chld_id=fork();
  if(!chld_id) {
    prctl(PR_SET_PDEATHSIG, SIGHUP); /* FIXME: linux-only. Find a more
                                        platform-independant solution. */
    int ret = execvp(args[0],args);
    if (ret!=0) {
      verbose_printf("Failed to run the command '%s ...'\n"
                     "Error code %d (%m)\n", args[0], ret);
      return 2;
    }
    /* FIXME: handle early exit of the screensaver */
  }

  Window root_win;

  // sleep(1); /* FIXME: A hack to immediate close due to focusing-related events */

  {
  int screen_num;
  Screen *screen;
  screen_num = DefaultScreen(dis);
  screen = XScreenOfDisplay(dis, screen_num);
  root_win = RootWindow(dis, XScreenNumberOfScreen(screen));
  int ret;
  root_win=win;
  ret = XGrabPointer(dis, root_win, True,
                ButtonReleaseMask | ButtonPressMask|Button1MotionMask,
                GrabModeAsync, GrabModeAsync,
                root_win, blankcursor(dis,root_win), CurrentTime);
  if(ret != GrabSuccess) {
    verbose_printf("Unable to grab root window mouse pointer: %d\n", ret);
    return 3;
  }
  ret = XGrabKeyboard(dis,
           root_win,
           KeyPressMask,
           GrabModeAsync, GrabModeAsync, CurrentTime);
  if(ret != GrabSuccess) {
    verbose_printf("Unable grab root window keyboard: %d\n", ret);
    return 4;
  }
  }


  /* Remove all the events from the queue. */
  XEvent event;
  while(True == XWindowEvent(dis, root_win,
      ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask,
      &event));

  /* Wait for new keyboard,mouse,etc. Exit immediately. */
  while (1) {
    memset(&event, 0, sizeof(event));
    XAllowEvents(dis, SyncPointer, CurrentTime);
    XWindowEvent(dis, root_win,
      ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask,
      &event);
    break;
  }

  verbose_printf("Exiting due to X event type %d\n", event.type);
  return 0;
}


