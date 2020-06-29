with import <nixpkgs> {};

(overrideCC stdenv gcc10).mkDerivation {
  name = "QnodeGraph";

  src = ./.;

  nativeBuildInputs = [
    cmake
    pkg-config
    qt5.wrapQtAppsHook
  ];

  cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];

  buildInputs = [
    qt512.qtbase
    libsForQt5.kconfigwidgets.dev
  ];
}
