colourspaces <- c(
  "cmy",       # 0
  "cmyk",      # 1
  "hsl",       # 2
  "hsb",       # 3
  "hsv",       # 4
  "lab",       # 5
  "hunterlab", # 6
  "lch",       # 7
  "luv",       # 8
  "rgb",       # 9
  "xyz",       # 10
  "yxy"        # 11
)
distances <- c(
  "euclidean",
  "cie1976",
  "cie94",
  "cie2000",
  "cmc"
)

colourspace_match <- function(colour){
  m <- pmatch( tolower(colour), colourspaces )
  if( is.na(m) ) stop("Unknown colour space")
  m
}

distance_match <- function(dist) {
  m <- pmatch( match.arg(tolower(dist), distances), distances)
  if( is.na(m) ) stop("Unknown distance measure")
  m
}
