{
  description = "A Nix-flake-based C development environment";

  inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0.1.*.tar.gz";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {
      packages = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.stdenv.mkDerivation {
          pname = "c-project";
          version = "0.1.0";
          src = ./.;

          # Build tools
          nativeBuildInputs = with pkgs; [
            cmake
            gcc
          ];

          # Runtime dependencies if any
          buildInputs = with pkgs; [
            gtest
          ];

          # Use the existing Makefile
          buildPhase = "make";
          
          # Install compiled binaries
          installPhase = ''
            mkdir -p $out/bin
            cp bin/* $out/bin/ || cp *.out $out/bin/ || cp main $out/bin/ || true
          '';

          meta = with pkgs.lib; {
            description = "C Project built with Nix using Make";
            license = licenses.mit;
            platforms = platforms.all;
          };
        };
      });

      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell.override
          {
            # Override stdenv in order to change compiler:
            # stdenv = pkgs.clangStdenv;
          }
          {
            packages = with pkgs; [
              clang-tools
              cmake
              codespell
              conan
              cppcheck
              doxygen
              gtest
              lcov
              vcpkg
              vcpkg-tool
            ] ++ (if pkgs.stdenv.isDarwin then [ ] else [ gdb ]);

            # Add build inputs to shell environment
            inputsFrom = [ self.packages.${pkgs.system}.default ];

            shellHook = ''
              echo "C Development Environment"
              echo "Build with: make"
              echo "Available tools: gcc, make, clang-tools, cppcheck, gtest"
            '';
          };
      });
    };
}