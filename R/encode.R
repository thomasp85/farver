#' Encode colours into RGB hex-strings
#' 
#' This is a version of [grDevices::rgb()] that works with the standard colour
#' format used in farver (matrix or data.frame with colours in rows). It further
#' support taking input from any colour space.
#' 
#' @inheritSection convert_colour Handling of non-finite and out of bounds values
#' 
#' @inheritParams convert_colour
#' @param alpha A numeric vector between 0 and 1. Will be recycled to the number
#' of rows in `colour`. If `NULL` or a single `NA` it will be ignored.
#' @param from The input colour space. Allowed values are: `"cmy"`, 
#' `"cmyk"`, `"hsl"`, `"hsb"`, `"hsv"`, `"lab"` (CIE L*ab), `"hunterlab"` 
#' (Hunter Lab), `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, `"rgb"` (sRGB), 
#' `"xyz"`, `"yxy"` (CIE xyY), or `"hcl"` (CIE Lch(uv) / polarLuv)
#' @param white The white reference of the input colour space. Will only have an 
#' effect for relative colour spaces such as Lab and luv. Any value accepted by 
#' [as_white_ref()] allowed.
#' 
#' @return A character vector with colours encoded as `#RRGGBB(AA)`
#' 
#' @family encoding and decoding functions
#' 
#' @note The output may differ slightly from that of [grDevices::rgb()] since
#' `rgb()` doesn't round numeric values correctly.
#' 
#' @export
encode_colour <- function(colour, alpha = NULL, from = 'rgb', white = 'D65') {
  if (from != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_c(colour, alpha, colourspace_match(from), white)
}

encode_c <- function(colour, alpha, from, white) {
  if (!is.null(alpha)) {
    alpha <- alpha * 255
    if (length(alpha) != 1) {
      alpha = rep_len(alpha, nrow(colour))
    } else if (is.na(alpha)) {
      alpha = NULL
    }
  }
  .Call('encode_c', as.matrix(colour), alpha, as.integer(from), white, PACKAGE = 'farver')
}

#' Decode RGB hex-strings into colour values
#' 
#' This is a version of [grDevices::col2rgb()] that returns the colour values in
#' the standard form expected by farver (matrix with a row per colour). As with
#' [encode_colour()] it can do colour conversion on the fly, meaning that you can 
#' decode a hex string directly into any of the supported colour spaces.
#' 
#' @inheritSection convert_colour Handling of non-finite and out of bounds values
#' 
#' @param colour A character vector of hex-encoded values or a valid colour name
#' as given in [grDevices::colours()].
#' @param alpha If `TRUE` the alpha channel will be returned as well (scaled 
#' between 0 and 1). If no alpha channel exists in the colour it will be 
#' assumed 1. If `FALSE` any alpha channel is ignored.
#' @param to The output colour space. Allowed values are: `"cmy"`, 
#' `"cmyk"`, `"hsl"`, `"hsb"`, `"hsv"`, `"lab"` (CIE L*ab), `"hunterlab"` 
#' (Hunter Lab), `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, `"rgb"` (sRGB), 
#' `"xyz"`, `"yxy"` (CIE xyY), or `"hcl"` (CIE Lch(uv) / polarLuv) 
#' @param white The white reference of the output colour space. Will only have 
#' an effect for relative colour spaces such as Lab and luv. Any value accepted 
#' by [as_white_ref()] allowed.
#' 
#' @return A numeric matrix with a row for each element in `colour` and either 
#' 3, 4, or 5 columns depending on the value of `alpha` and `to`.
#' 
#' @family encoding and decoding functions
#' 
#' @export
decode_colour <- function(colour, alpha = FALSE, to = 'rgb', white = 'D65') {
  #if (to != 'rgb') {
    white <- as_white_ref(white)
  #}
  alpha <- isTRUE(alpha)
  colours <- decode_c(colour, alpha, colourspace_match(to), white)
  colnames(colours) <- c(colour_dims[[to]], if (alpha) 'alpha' else NULL)
  colours
}

decode_c <- function(colour, alpha, to, white) {
  .Call('decode_c', as.character(colour), alpha, as.integer(to), white, PACKAGE = 'farver')
}
