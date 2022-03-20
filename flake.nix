{
  description = "Compiler and runtime replicating the GBA SDK used by commercial titles.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: 
    let flake = system: 
      let
        pkgs = import nixpkgs { inherit system; };
      in rec {
        packages.binutils = pkgs.stdenv.mkDerivation {
          pname = "agb-binutils";
          version = "2.31-agb1";
          src = ./.;
          dontConfigure = true;
          buildPhase = ''make binutils-all prefix=$out'';
          installPhase = ''make binutils-install-strip prefix=$out'';
          buildInputs = [ pkgs.zlib ];
        };

        packages.gas = pkgs.stdenv.mkDerivation {
          pname = "agb-gas";
          version = "2.31-agb1";
          src = ./.;
          dontConfigure = true;
          buildPhase = ''make gas-all prefix=$out'';
          installPhase = ''make gas-install-strip prefix=$out'';
          buildInputs = [ pkgs.zlib ];
        };

        defaultPackage = packages.binutils;
      };
    in 
      flake-utils.lib.eachDefaultSystem flake;
}
