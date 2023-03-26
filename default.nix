{ pkgs ? import <nixpkgs> {} } :

(import ./xscreensaver-run.nix {
  libX11 = pkgs.xorg.libX11;
  inherit (pkgs) lib stdenv fetchgit xscreensaver makeWrapper;
}).overrideAttrs(old : {
  src = ./.;
  buildInputs = (old.buildInputs ++ [ pkgs.ccls pkgs.xscreensaver ]);
  shellHook = ''
    export PATH="${pkgs.xscreensaver}/libexec/xscreensaver:$PATH"
  '';
})
