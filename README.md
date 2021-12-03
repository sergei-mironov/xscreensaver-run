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


NixOS users may use the following commands to install the application in the
current user's profile:

    $ nix-build
    $ nix-env -i ./result

Usage
-----

Run any screensaver from Xscreensaver collection by typing `xscreensaver-run
SCREENSAVER_APP_ARGS`. For example:

    $ ./xscreensaver-run /usr/libexec/xscreensaver/apple2 -text -program 'uname -a'

Enjoy!


Development notes
-----------------

### Environment

To test the application in Nix:

1. `$ nix-shell`
2. `(shell) $ nix-build '<nixpkgs>' -A pkgs.xscreensaver`
3. `(shell) $ make`
4. `(shell) $ ./xscreensaver-run ./result/libexec/xscreensaver/apple2 -text`

### Hints

* To reset root window's cursor (just in case): `xsetroot -cursor_name left_ptr`

### References

* [X Event definitions](https://fossies.org/dox/tightvnc-1.3.10_unixsrc/X_8h.html)
* [X Event documentation](https://tronche.com/gui/x/xlib/events/processing-overview.html)
* https://linux.die.net/man/3/xwindowevent
* https://linux.die.net/man/3/xgrabkeyboard

Bugs?
-----

Yes! Check FIXMEs in the source code. Feel free to fix them and file PRs!

