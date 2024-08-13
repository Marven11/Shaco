{
  inputs = {
    utils.url = "github:numtide/flake-utils";
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


    });
}
