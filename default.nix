{ pkgs ? import <nixpkgs> {} } :

(import ./xscreensaver-run.nix {
  libX11 = pkgs.xlibs.libX11;
  inherit (pkgs) stdenv fetchgit xscreensaver makeWrapper;
}).overrideAttrs(old : {
  src = ./.;
  buildInputs = (old.buildInputs ++ [ pkgs.ccls ]);
})
