{
  inputs = {
    utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
  };

  outputs =
    { self
    , nixpkgs
    , utils
    , ...
    }:
    utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs { inherit system; };
      pkgs_32 = import nixpkgs {
        inherit system;
        crossSystem = {
          config = "i686-unknown-linux-gnu";
        };
      };
      pythonPackages = pkgs.python311Packages;
    in
    {
      devShells.default = pkgs.mkShell {
        name = "python-venv";
        venvDir = "./.venv-nixos";
        buildInputs = with pkgs; [
          pythonPackages.python
          pythonPackages.venvShellHook
          cmake
          clang
          glibc.static
        ];
        postVenvCreation = ''
          unset SOURCE_DATE_EPOCH
          pip install -r requirements.txt
        '';
        postShellHook = ''
          # allow pip to install wheels
          unset SOURCE_DATE_EPOCH
        '';
      };
      devShells.x86_compile = pkgs_32.mkShell.override
        {
          stdenv = pkgs_32.clangStdenv;
        }
        {
          packages = with pkgs_32; [
            cmake
            clang
          ];
          buildInputs = with pkgs_32;[
            glibc.static

          ];
        };
    });
}
