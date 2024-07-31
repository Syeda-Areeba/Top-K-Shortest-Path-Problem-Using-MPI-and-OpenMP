with import <nixpkgs> {};

mkShell {
  buildInputs = [
    mpich
  ];
}
