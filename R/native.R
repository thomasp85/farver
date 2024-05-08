#' Convert to and from the R native colour representation
#' 
#' Colours in R are internally encoded as integers when they are passed around
#' to graphics devices. The encoding splits the 32 bit in the integer between
#' red, green, blue, and alpha, so that each get 8 bit, equivalent to 256 
#' values. It is very seldom that an R user is subjected to this representation,
#' but it is present in the `nativeRaster` format which can be obtained from 
#' e.g. capturing the content of a graphic device (using `dev.capture()`) or reading
#' in PNG files using `png::readPNG(native = TRUE)`. It is very rare that you 
#' might need to convert back and forth between this format, but it is provided
#' here for completeness.
#' 
#' @param colour For `encode_native` either a vector of hex-encoded 
#' colours/colour names or a matrix encoding colours in any of the supported 
#' colour spaces. For `decode_native` it is a vector of 
#' integers.
#' @inheritParams encode_colour
#' 
#' @return `encode_native()` returns an integer vector and `decode_native()`
#' returns a character vector, both matching the length of the input.
#' 
#' @export
#' @name native-encoding
#' @rdname native_encoding
#' 
#' @examples 
#' 
#' # Get native representation of navyblue and #228B22
#' native_col <- encode_native(c('navyblue', '#228B22'))
#' native_col
#' 
#' # Convert back
#' decode_native(native_col)
#' 
encode_native <- function(colour, alpha = NULL, from = 'rgb', white = 'D65', na_value = NA) {
  if (is.character(colour)) {
    return(encode_native_c(colour, na_value = na_value))
  }
  if (from != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_c(colour, alpha, colourspace_match(from), white, out_format = 2L, na_value = na_value)
}

#' @rdname native_encoding
#' @export
decode_native <- function(colour) {
  decode_native_c(colour)
} 

encode_native_c <- function(colour, na_value = NA_character_) {
  if (length(na_value) == 0) {
    na_value <- NA_character_
  }
  if (length(na_value) > 1) {
    stop("na_value must be a string")
  }
  na_value <- as.character(na_value)
  .Call(`farver_encode_native_c`, colour, na_value)
}
decode_native_c <- function(colour) {
  .Call(`farver_decode_native_c`, as.integer(colour))
}
