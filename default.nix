{ pkgs ? import <nixpkgs> { }, src ? ./src, subdir ? "" }:

let theSource = src; in
pkgs.stdenv.mkDerivation rec {
	pname = "cmk";
	version = "1.0.0";

	src = "${theSource}/${subdir}";

	nativeBuildInputs = with pkgs; [ gcc ];

	meta = with pkgs.lib; {
		description = "A c app to create cpp classes easily";
		license = licenses.mit;
	};
	
	buildPhase = ''
		mkdir -p $out/bin
		gcc $src/main.c -o cmk
	'';

	installPhase = ''
		mkdir -p $out/bin
		cp cmk $out/bin/cmk
	'';
}
