#' Convert between colour spaces
#' 
#' This function lets you convert between different representations of colours. 
#' The API is reminiscent of [grDevices::convertColor()], but the performance is
#' much better. It is not assured that `grDevices::convertColor()` and 
#' `convert_colour()` provide numerically equivalent conversion at 16bit level as
#' the formula used are potentially slightly different. For all intend and 
#' purpose, the resulting colours will be equivalent though.
#' 
#' @section Handling of non-finite and out of bounds values:
#' `NA`, `NaN`, `-Inf`, and `Inf` are treated as invalid input and will result 
#' in `NA` values for the colour. If a given colourspace has finite bounds in 
#' some of their channels, the input will be capped before conversion, and the
#' output will be capped before returning, so that both input and output colours
#' are valid colours in their respective space. This means that converting back
#' and forth between two colourspaces may result in a change in the colour if
#' the gamut of one of the spaces is less than the other.
#' 
#' @param colour A numeric matrix (or an object coercible to one) with colours 
#' encoded in the rows and the different colour space values in the columns. For 
#' all colourspaces except `'cmyk'` this will mean a matrix with three columns - 
#' for `'cmyk'` it means four columns.
#' 
#' @param from,to The input and output colour space. Allowed values are: `"cmy"`, 
#' `"cmyk"`, `"hsl"`, `"hsb"`, `"hsv"`, `"lab"` (CIE L*ab), `"hunterlab"` 
#' (Hunter Lab), `"oklab"`, `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, 
#' `"rgb"` (sRGB), `"xyz"`, `"yxy"` (CIE xyY), `"hcl"` (CIE Lch(uv) / polarLuv),
#' or `"oklch"` (Polar form of oklab)
#' 
#' @param white_from,white_to The white reference of the from and to colour 
#' space. Will only have an effect for relative colour spaces such as Lab and 
#' luv. Any value accepted by [as_white_ref()] allowed.
#' 
#' @return A numeric matrix with the same number of rows as `colour` and either 
#' 3 or 4 columns depending on the value of `to`. If `colour` is given as a 
#' `data.frame` the output will be a data.frame as well
#' 
#' @note This function and [convertColor()] are not
#' numerically equivalent due to rounding errors, but for all intend and purpose
#' they give the same results.
#' 
#' @seealso [grDevices::convertColor()], [grDevices::col2rgb()]
#' 
#' @export
#' 
#' @examples
#' spectrum <- decode_colour(rainbow(10))
#' spec_lab <- convert_colour(spectrum, 'rgb', 'lab')
#' spec_lab
#' 
#' # Convert between different white references
#' convert_colour(spec_lab, 'lab', 'lab', white_from = 'D65', white_to = 'F10')
#' 
convert_colour <- function(colour, from, to, white_from = 'D65', white_to = white_from) {
  res <- convert_c(colour, colourspace_match(from), colourspace_match(to), as_white_ref(white_from), as_white_ref(white_to))
  colnames(res) <- colour_dims[[to]]
  if (is.data.frame(colour)) res <- as.data.frame(res)
  res
}

convert_c <- function(colour, from, to, white_from, white_to) {
  .Call('convert_c', as.matrix(colour), as.integer(from), as.integer(to), 
        as.numeric(white_from), as.numeric(white_to), PACKAGE = 'farver')
}
