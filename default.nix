{
  nixpkgs ? import (builtins.fetchGit {
    url = "https://siriobalmelli@github.com/siriobalmelli-foss/nixpkgs.git";
    ref = "master";
    }) {}
}:

with nixpkgs;

let
  nonlibc = nixpkgs.nonlibc or import (builtins.fetchGit {
    url = "https://siriobalmelli@github.com/siriobalmelli/nonlibc.git";
    ref = "master";
    }) {};

in
  stdenv.mkDerivation rec {
    name = "f2tp";
    version = "0.0.1";

    # just work with the current directory (aka: Git repo), no fancy tarness
    src = nix-gitignore.gitignoreSource [] ./.;

    nativeBuildInputs = [
      meson
      ninja
      pkg-config
      python3
    ];

    propagatedBuildInputs = [
      libsodium
      nonlibc
    ];

    meta = with stdenv.lib; {
      description = "Fast FEC Transport Protocol (library)";
      homepage = https://github.com/siriobalmelli/f2tp;
      license = licenses.lgpl21Plus;
      platforms = platforms.unix;
      maintainers = with maintainers; [ siriobalmelli ];
    };
  }
