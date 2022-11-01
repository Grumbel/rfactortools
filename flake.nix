{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        pythonPackages = pkgs.python310Packages;
      in rec {
        packages = rec {
          default = rfactortools;

          rfactortools = pythonPackages.buildPythonPackage rec {
            name = "rfactortools";

            src = ./.;

            doCheck = false;

            propagatedBuildInputs = with pythonPackages; [
              setuptools
              pillow
              tkinter
            ];
          };
        };
      }
    );
}
