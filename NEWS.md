# farver (development version)

* Small adjustments to support super weird architectures (#15, #16)
* `decode_colour()` now allows you to specify how `NA` should get interpreted

# farver 2.0.1

* Fixed symbol remapping issues on Solaris
* UPPER and Title case for colour names are now supported alongside lower case

# farver 2.0.0

* Added a `NEWS.md` file to track changes to the package.
* Remove Rcpp dependency for faster compilation. (#6)
* Add support for Lch(uv) (Hcl) for parallel to `grDevices::hcl()` (#9)
* Add `encode_colour()` for converting colours in any colourspace into `#RRGGBB`
  format (#7)
* Add `decode_colour()` for converting hexstrings into any colourspace
* Fix bug in luv conversion where 0 in the l channel resulted in `NaN` results 
  (#12)
* Provide a family of channel manipulation functions to directly manipulate 
  encodes strings
