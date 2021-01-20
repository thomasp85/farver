colourspaces <- c(
  "cmy",       # 1
  "cmyk",      # 2
  "hsl",       # 3
  "hsb",       # 4
  "hsv",       # 5
  "lab",       # 6
  "hunterlab", # 7
  "lch",       # 8
  "luv",       # 9
  "rgb",       # 10
  "xyz",       # 11
  "yxy",       # 12
  "hcl",       # 13
  "oklab",     # 14
  "oklch"      # 15
)
colour_dims <- list(
  cmy = c('c', 'm', 'y'),
  cmyk = c('c', 'm', 'y', 'k'),
  hsl = c('h', 's', 'l'),
  hsb = c('h', 's', 'b'),
  hsv = c('h', 's', 'v'),
  lab = c('l', 'a', 'b'),
  hunterlab = c('l', 'a', 'b'),
  lch = c('l', 'c', 'h'),
  luv = c('l', 'u', 'v'),
  rgb = c('r', 'g', 'b'),
  xyz = c('x', 'y', 'z'),
  yxy = c('y1', 'x', 'y2'),
  hcl = c('h', 'c', 'l'),
  oklab = c('l', 'a', 'b'),
  oklch = c('l', 'c', 'h')
)
colour_channel_index <- list(
  cmy = c('c' = 1L, 'm' = 2L, 'y' = 3L),
  cmyk = c('c' = 1L, 'm' = 2L, 'y' = 3L, 'k' = 4L),
  hsl = c('h' = 1L, 's' = 2L, 'l' = 3L),
  hsb = c('h' = 1L, 's' = 2L, 'b' = 3L),
  hsv = c('h' = 1L, 's' = 2L, 'v' = 3L),
  lab = c('l' = 1L, 'a' = 2L, 'b' = 3L),
  hunterlab = c('l' = 1L, 'a' = 2L, 'b' = 3L),
  lch = c('l' = 1L, 'c' = 2L, 'h' = 3L),
  luv = c('l' = 1L, 'u' = 2L, 'v' = 3L),
  rgb = c('r' = 1L, 'g' = 2L, 'b' = 3L),
  xyz = c('x' = 1L, 'y' = 2L, 'z' = 3L),
  yxy = c('y1' = 1L, 'x' = 2L, 'y2' = 3L),
  hcl = c('h' = 1L, 'c' = 2L, 'l' = 3L),
  oklab = c('l' = 1L, 'a' = 2L, 'b' = 3L),
  oklch = c('l' = 1L, 'c' = 2L, 'h' = 3L)
)
distances <- c(
  "euclidean",
  "cie1976",
  "cie94",
  "cie2000",
  "cmc"
)
operations <- c(
  "set",
  "add",
  "multiply",
  "least",
  "most"
)

colourspace_match <- function(colour) {
  m <- pmatch(tolower(colour), colourspaces)
  if (is.na(m)) stop("Unknown colour space", call. = FALSE)
  m
}

distance_match <- function(dist) {
  m <- pmatch(match.arg(tolower(dist), distances), distances)
  if (is.na(m)) stop("Unknown distance measure", call. = FALSE)
  m
}
operation_match <- function(op) {
  m <- pmatch(match.arg(tolower(op), operations), operations)
  if (is.na(m)) stop("Unknown operation", call. = FALSE)
  m
}

white_references <- list(
  "2" = list(
    A = c(0.44757, 0.40745),
    B = c(0.34842, 0.35161),
    C = c(0.31006, 0.31616),
    D50 = c(0.34567, 0.35850),
    D55 = c(0.33242, 0.34743),
    D65 = c(0.31271, 0.32902),
    D75 = c(0.29902, 0.31485),
    E = c(1/3, 1/3),
    F1 = c(0.31310, 0.33727),
    F2 = c(0.37208, 0.37529),
    F3 = c(0.40910, 0.39430),
    F4 = c(0.44018, 0.40329),
    F5 = c(0.31379, 0.34531),
    F6 = c(0.37790, 0.38835),
    F7 = c(0.31292, 0.32933),
    F8 = c(0.34588, 0.35875),
    F9 = c(0.37417, 0.37281),
    F10 = c(0.34609, 0.35986),
    F11 = c(0.38052, 0.37713),
    F12 = c(0.43695, 0.40441)
  ),
  "10" = list(
    A = c(0.45117, 0.40594),
    B = c(0.34980, 0.35270),
    C = c(0.31039, 0.31905),
    D50 = c(0.34773, 0.35952),
    D55 = c(0.33411, 0.34877),
    D65 = c(0.31382, 0.33100),
    D75 = c(0.29968, 0.31740),
    E = c(1/3, 1/3),
    F1 = c(0.31811, 0.33559),
    F2 = c(0.37925, 0.36733),
    F3 = c(0.41761, 0.38324),
    F4 = c(0.44920, 0.39074),
    F5 = c(0.31975, 0.34246),
    F6 = c(0.38660, 0.37847),
    F7 = c(0.31569, 0.32960),
    F8 = c(0.34902, 0.35939),
    F9 = c(0.37829, 0.37045),
    F10 = c(0.35090, 0.35444),
    F11 = c(0.38541, 0.37123),
    F12 = c(0.44256, 0.39717)
  )
)
#' Convert value to a tristimulus values normalised to Y=100
#' 
#' This function can take either the name of a standardised illuminants, x
#' and y chromaticity coordinates or X, Y, and Z tristimulus values and converts
#' it to tristimulus values normalised to Y=100. All Illuminant series A-F are
#' supported and can be queried both on the CIE 1931 2° and CIE 1964 10° 
#' chromaticity coordinates.
#' 
#' @param x A string giving the name of the standardized illuminant or a 
#' 2 (chromaticity) or 3 (trsitimulus) length numeric vector.
#' 
#' @param fow The field-of-view for the illuminant - either `2` or `10`
#' 
#' @return A 3-length vector with tristimulus values
#' 
#' @keywords internal
#' 
#' @export
#' 
#' @examples 
#' # Using names
#' as_white_ref('D65')
#' 
#' # Using chromaticity values
#' as_white_ref(c(0.3, 0.4))
as_white_ref <- function(x, fow = 2) {
  if (is.character(x)) {
    x <- white_references[[as.character(fow)]][[toupper(x)]]
    if (is.null(x)) stop('Unknown white reference', call. = FALSE)
  }
  if (is.integer(x)) x <- as.numeric(x)
  if (!is.numeric(x)) stop('White reference must be a numeric vector', call. = FALSE)
  if (length(x) == 2) {
    tmp <- 100/x[2]
    x <- c(
      tmp * x[1],
      100,
      tmp * (1 - sum(x))
    )
  } else if (length(x) == 3) {
    if (x[2] != 100) {
      x <- x * 100 / x[2]
    }
  } else {
    stop('White reference must be of length 2 (chromaticity) or 3 (tristimulus)', call. = FALSE)
  }
  structure(x, names = c('X', 'Y', 'Z'))
}

load_colour_names <- function() {
  .Call('load_colour_names_c', 
        c(all_colours, as.character(seq_along(def_palette) - 1L)), 
        cbind(all_values, def_palette_values), 
        PACKAGE = 'farver')
  invisible()
}
as_colour_code <- function(x) {
  if (is.character(x)) return(x)
  n <- names(x)
  if (is.numeric(x)) {
    if (any(x <= 0, na.rm = TRUE)) {
      stop("colours encodes as numbers must be positive", call. = FALSE)
    }
    x <- as.integer(x)
    x <- ifelse(x == 0, 1, ((x - 1) %% (length(def_palette) - 1)) + 2)
    x <- def_palette[x]
  } else {
    x <- as.character(x)
  }
  names(x) <- n
  x
}
