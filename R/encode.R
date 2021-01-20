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
#' (Hunter Lab), `"oklab"`, `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, 
#' `"rgb"` (sRGB), `"xyz"`, `"yxy"` (CIE xyY), `"hcl"` (CIE Lch(uv) / polarLuv),
#' or `"oklch"` (Polar form of oklab)
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
#' 
#' @examples 
#' spectrum <- decode_colour(rainbow(10))
#' 
#' encode_colour(spectrum)
#' 
#' # Attach alpha values
#' encode_colour(spectrum, alpha = c(0.5, 1))
#' 
#' # Encode from a different colour space
#' spectrum_hcl <- convert_colour(spectrum, 'rgb', 'hcl')
#' encode_colour(spectrum_hcl, from = 'hcl')
#' 
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
    } else if (is.na(alpha) || alpha == 1) {
      alpha = NULL
    }
  }
  .Call('encode_c', as.matrix(colour), alpha, as.integer(from), white, PACKAGE = 'farver')
}
