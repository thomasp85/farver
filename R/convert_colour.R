#' Convert between colour spaces
#' 
#' This function lets you convert between different representations of colours. 
#' The API is reminiscent of [grDevices::convertColor()], but the performance is
#' much better. It is not assured that `grDevices::convertColor()` and 
#' `convert_colour()` provide numerically eqivalent conversion at 16bit level as
#' the formula used are potentially slightly different. For all intend and 
#' purpose, the resulting colours will be equivalent though.
#' 
#' @param colour A numeric matrix with colours encoded in the rows and the 
#' different colour space values in the columns. For all colourspaces except 
#' `'cmyk'` this will mean a matrix with three columns - for `'cmyk'` it means
#' four columns.
#' 
#' @param from,to The input and output colour space. Allowed values are: "cmy", 
#' "cmyk", "hsl", "hsb", "hsv", "lab", "hunterlab", "lch", "luv", "rgb", "xyz", 
#' "yxy"
#' 
#' @return A numeric matrix with the same number of rows as `colour` and either 
#' 3 or 4 columns depending on the value of `to`
#' 
#' @seealso [grDevices::convertColor()], [grDevices::col2rgb()]
#' 
#' @export
#' 
#' @examples
#' spectrum <- t(col2rgb(rainbow(10)))
#' convert_colour(spectrum, 'rgb', 'lab')
#' 
convert_colour <- function(colour, from, to) {
  from <- match.arg(tolower(from), colourspaces)
  to <- match.arg(tolower(to), colourspaces)
  convert_c(as.matrix(colour), from, to)
}