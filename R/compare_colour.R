#' Calculate the distance between colours
#' 
#' There are many ways to measure the distance between colours. `farver` 
#' provides 5 different algorithms, ranging from simple euclidean distance in 
#' RGB space, to different perceptual measures such as CIE2000.
#' 
#' @inheritSection  convert_colour Handling of non-finite and out of bounds values
#' 
#' @param from,to Numeric matrices with colours to compare - the format is the 
#' same as that for [convert_colour()]. If `to` is not set `from` will be 
#' compared with itself and only the upper triangle will get calculated
#' 
#' @param from_space,to_space The colour space of `from` and `to` respectively. 
#' `to_space` defaults to be the same as `from_space`.
#' 
#' @param method The method to use for comparison. Either `'euclidean'`, 
#' `'cie1976'`, `'cie94'`, `'cie2000'`, or `'cmc'`
#' 
#' @param white_from,white_to The white reference of the from and to colour 
#' space. Will only have an effect for relative colour spaces such as Lab and 
#' luv. Any value accepted by [as_white_ref()] allowed.
#' 
#' @return A numeric matrix with the same number of rows as colours in `from`
#' and the same number of columns as colours in `to`. If `to` is not given, only
#' the upper triangle will be returned.
#' 
#' @export
#' 
#' @examples 
#' r <- decode_colour(rainbow(10))
#' h <- decode_colour(heat.colors(15))
#' 
#' # Compare two sets of colours
#' compare_colour(r, h, 'rgb', method = 'cie2000')
#' 
#' # Compare a set of colours with itself
#' compare_colour(r, from_space = 'rgb', method = 'cmc')
#' 
#' # Compare colours from different colour spaces
#' h_luv <- convert_colour(h, 'rgb', 'luv')
#' compare_colour(r, h_luv, 'rgb', 'luv')
#' 
compare_colour <- function(from, to = NULL, from_space, to_space = from_space, method = 'euclidean', white_from = 'D65', white_to = white_from) {
  sym <- FALSE
  if (is.null(to)) {
    to <- from;
    sym <- TRUE
  }
  compare_c(from, to, colourspace_match(from_space), colourspace_match(to_space), distance_match(method), sym, as_white_ref(white_from), as_white_ref(white_to))
}

compare_c <- function(from, to, from_space, to_space, method, symmetric, white_from, white_to) {
  .Call('compare_c', as.matrix(from), as.matrix(to), as.integer(from_space), 
        as.integer(to_space), as.integer(method), as.logical(symmetric), 
        as.numeric(white_from), as.numeric(white_to), PACKAGE = 'farver')
}
