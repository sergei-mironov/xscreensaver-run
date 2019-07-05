Xscreensaver-run
================

This tiny program may be used to run screensavers from
[XScreenSaver](https://www.jwz.org/xscreensaver/) collection synchronously,
in fullscreen and without additional effects like screen locks or timers.

See also <https://unix.stackexchange.com/questions/290698/how-to-manually-start-an-xscreensaver-application-in-root-window-full-screen>


Build
-----

Just make install

    $ make install

NixOS users may use the following shell env:

    $ nix-shell -p xlibs.libX11.dev pkgs.strace pkgs.gdb


Usage
-----

Run any screensaver from Xscreensaver collection by typing `xscreensaver-run
SCREENSAVER_APP_ARGS`. For example:

    $ ./xscreensaver-run /usr/libexec/xscreensaver/apple2 -text -program 'uname -a'

Enjoy!


Bugs?
-----

Check FIXMEs in the source code. Feel free to fix them and share PRs!
