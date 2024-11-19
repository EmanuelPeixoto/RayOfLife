{
  description = "Raylib Conways game of life";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      packages = {
        ROL = pkgs.stdenv.mkDerivation {
          pname = "Ray-of-Life";
          version = "1.0.0";
          src = ./.;
          buildInputs = [
            pkgs.gcc
            pkgs.gdb
            pkgs.gnumake
            pkgs.pkg-config
            pkgs.raylib
          ];
          nativeBuildInputs = [ pkgs.gnumake ];
          meta = with pkgs.lib; {
            description = "Game of life using raylib";
            license = licenses.mit;
          };
          buildPhase = ''
            gcc -o Ray-of-Life main.c -lraylib
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp Ray-of-Life $out/bin/
          '';
        };
      };
      devShells.default = pkgs.mkShell {
        buildInputs = [
          pkgs.gcc
          pkgs.pkg-config
          pkgs.raylib
          pkgs.gdb
          pkgs.gnumake
        ];
        shellHook = ''
          echo "Welcome to the development environment!"
          USER_SHELL=$(getent passwd $USER | cut -d: -f7)
          exec $USER_SHELL
        '';
      };
    });
}
