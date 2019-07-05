{ stdenv, fetchFromGitHub, libX11, xscreensaver }:

stdenv.mkDerivation rec {
  name = "xscreensaver-run-${version}";
  version = "1.0.0";

  src = fetchFromGitHub {
    owner = "grwlf";
    repo = "xscreensaver-run";
    rev = version;
    sha256 = "0000000000000000000000000000000000000000000000000000";
  };

  nativeBuildInputs = [ ];
  buildInputs = [ libX11 xscreensaver ];

  meta = with stdenv.lib; {
    description = "Run screensaver from XScreenSaver collection synchronousely";
    homepage = https://github.com/grwlf/xscreensaver-run;
    license = licenses.mit;
    maintainers = with maintainers; [ smironov ];
    platforms = platforms.linux;
  };
}

