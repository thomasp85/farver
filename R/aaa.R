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

colourspace_match <- function(colour){
  m <- pmatch( tolower(colour), colourspaces )
  if( is.na(m) ) stop("Unknown colour space")
  m
}
