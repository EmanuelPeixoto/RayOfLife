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
        server = pkgs.stdenv.mkDerivation {
          pname = "Ray-of-Life";
          version = "1.0.0";

          src = ./.;  # Use the current directory as source

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

          # Build steps
          buildPhase = ''
            mkdir -p $out/bin  # Ensure the output directory exists
            make build
          '';
        };
      };

      # Development shell for building
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
        '';
      };
    });
}
