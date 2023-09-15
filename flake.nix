{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
  };
  outputs = {nixpkgs, ...}: let
    system = "aarch64-darwin";
    pkgs = import nixpkgs {
      inherit system;
    };
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      name = "Beep";
      src = ./src;

      buildInputs = [pkgs.SDL2];

      buildPhase = ''
        CC -o beep main.c -Wall -Wextra `sdl2-config --cflags --libs`
      '';

      installPhase = ''
        mkdir -p $out/bin
        cp beep $out/bin
      '';
    };
  };
}
