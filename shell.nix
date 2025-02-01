# SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
# SPDX-License-Identifier: MPL-2.0

let
  pkgs = import <nixpkgs> {};
in
  pkgs.mkShell {
    packages = with pkgs; [
      reuse
      gnumake
    ];
  }