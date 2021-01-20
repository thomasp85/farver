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
#' (Hunter Lab), `"oklab"`, `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, 
#' `"rgb"` (sRGB), `"xyz"`, `"yxy"` (CIE xyY), `"hcl"` (CIE Lch(uv) / polarLuv),
#' or `"oklch"` (Polar form of oklab)
#' @param white The white reference of the output colour space. Will only have 
#' an effect for relative colour spaces such as Lab and luv. Any value accepted 
#' by [as_white_ref()] allowed.
#' @param na_value A valid colour string or `NA` to use when `colour` contains
#' `NA` elements. The general approach in farver is to carry `NA` values over,
#' but if you want to mimick [col2rgb()] you should set 
#' `na_value = 'transparent'`, i.e. treat `NA` as transparent white.
#' 
#' 
#' @return A numeric matrix with a row for each element in `colour` and either 
#' 3, 4, or 5 columns depending on the value of `alpha` and `to`.
#' 
#' @family encoding and decoding functions
#' 
#' @export
#' 
#' @examples 
#' # basic use
#' decode_colour(c('#43e1f6', 'steelblue', '#67ce9fe4'))
#' 
#' # Return alpha as well (no alpha value is interpreted as 1)
#' decode_colour(c('#43e1f6', 'steelblue', '#67ce9fe4'), alpha = TRUE)
#' 
#' # Decode directly into specific colour space
#' decode_colour(c('#43e1f6', 'steelblue', '#67ce9fe4'), to = 'lch')
#' 
decode_colour <- function(colour, alpha = FALSE, to = 'rgb', white = 'D65', na_value = NA) {
  if (to != 'rgb') {
    white <- as_white_ref(white)
  }
  alpha <- isTRUE(alpha)
  colours <- decode_c(colour, alpha, colourspace_match(to), white, na_value)
  colnames(colours) <- c(colour_dims[[to]], if (alpha) 'alpha' else NULL)
  colours
}

decode_c <- function(colour, alpha, to, white, na_value) {
  .Call('decode_c', as_colour_code(colour), alpha, as.integer(to), white, as.character(na_value), PACKAGE = 'farver')
}
