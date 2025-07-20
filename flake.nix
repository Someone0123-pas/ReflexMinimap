{
  description = "Minimap for Kirby and the Amazing Mirror";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: flake-utils.lib.eachDefaultSystem (system:
  let
    pkgs = import nixpkgs { inherit system; };

    revision = with self; if sourceInfo?dirtyShortRev
      then sourceInfo.dirtyShortRev
      else sourceInfo.shortRev;

    katam-minimap = pkgs.stdenv.mkDerivation {
      pname = "katam-minimap";
      version = "0.0.${revision}";
      src = ./.;

      nativeBuildInputs = with pkgs; [
	cmake
	ninja
	wget
      ];

      buildInputs = with pkgs; [
	wayland
	libGL
      ];
    };
  in {
    packages.default = katam-minimap;

    apps.default = flake-utils.lib.mkApp {
      drv = self.packages.${system}.default;
    };

    devShells.default = pkgs.mkShell {
      inputsFrom = [ self.packages.${system}.default ];

      packages = with pkgs; [
	clang-tools
      ];
    };
  });
}
