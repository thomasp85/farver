# farver 1.1.0.9999

* Added a `NEWS.md` file to track changes to the package.
* Remove Rcpp dependency for faster compilation. (#6)
* Add support for Lch(uv) (Hcl) for parallel to `grDevices::hcl()` (#9)
* Add `encode_colour()` for converting colours in any colourspace into `#RRGGBB`
  format (#7)
* Add `decode_colour()` for converting hexstrings into any colourspace