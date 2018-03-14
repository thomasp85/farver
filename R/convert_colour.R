#' @export
convert_colour <- function(colour, from, to) {
  convert_c(as.matrix(colour), tolower(from), tolower(to))
}