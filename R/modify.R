#' Modify colour space channels in hex-encoded colour strings
#' 
#' This set of functions allows you to modify colours as given by strings, 
#' whithout first decoding them. For large vectors of colour values this should
#' provide a considerable speedup.
#' 
#' @param colour A character string giving colours, either as hexadecimal 
#' strings or accepted colour names. 
#' @param channel The channel to modify or extract as a single letter, or 
#' `'alpha'` for the alpha channel.
#' @param value The value to modify with
#' @param space The colour space the channel pertains to. Allowed values are: 
#' `"cmy"`, `"cmyk"`, `"hsl"`, `"hsb"`, `"hsv"`, `"lab"` (CIE L*ab), `"hunterlab"` 
#' (Hunter Lab), `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, `"rgb"` (sRGB), 
#' `"xyz"`, `"yxy"` (CIE xyY), or `"hcl"` (CIE Lch(uv) / polarLuv)
#' @param white The white reference of the channel colour space. Will only have 
#' an effect for relative colour spaces such as Lab and luv. Any value accepted 
#' by [as_white_ref()] allowed.
#' 
#' @return A character vector of the same length as `colour` (or a numeric 
#' vector in the case of `get_channel()`)
#' 
#' @family encoding and decoding functions
#' 
#' @rdname manip_channel
#' @name manip_channel
#' 
#' @examples 
#' spectrum <- rainbow(10)
#' 
#' # set a specific channel
#' set_channel(spectrum, 'r', c(10, 50))
#' set_channel(spectrum, 'l', 50, space = 'lab')
#' set_channel(spectrum, 'alpha', c(0.5, 1))
#' 
#' # Add value to channel
#' add_to_channel(spectrum, 'r', c(10, 50))
#' add_to_channel(spectrum, 'l', 50, space = 'lab')
#' 
#' # Multiply a channel
#' multiply_channel(spectrum, 'r', c(10, 50))
#' multiply_channel(spectrum, 'l', 50, space = 'lab')
#' 
#' # set a lower bound on a channel
#' raise_channel(spectrum, 'r', c(10, 50))
#' raise_channel(spectrum, 'l', 20, space = 'lab')
#' 
#' # set an upper bound on a channel
#' cap_channel(spectrum, 'r', c(100, 50))
#' cap_channel(spectrum, 'l', 20, space = 'lab')
#' 
NULL

#' @rdname manip_channel
#' @export
set_channel <- function(colour, channel, value, space = 'rgb', white = 'D65') {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  if (length(value) != 1) value <- rep_len(value, length(colour))
  encode_channel_c(colour, channel, value, space, 1L, white)
}

#' @rdname manip_channel
#' @export
add_to_channel <- function(colour, channel, value, space = 'rgb', white = 'D65') {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  if (length(value) != 1) value <- rep_len(value, length(colour))
  encode_channel_c(colour, channel, value, space, 2L, white)
}

#' @rdname manip_channel
#' @export
multiply_channel <- function(colour, channel, value, space = 'rgb', white = 'D65') {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  if (length(value) != 1) value <- rep_len(value, length(colour))
  encode_channel_c(colour, channel, value, space, 3L, white)
}

#' @rdname manip_channel
#' @export
raise_channel <- function(colour, channel, value, space = 'rgb', white = 'D65') {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  if (length(value) != 1) value <- rep_len(value, length(colour))
  encode_channel_c(colour, channel, value, space, 4L, white)
}

#' @rdname manip_channel
#' @export
cap_channel <- function(colour, channel, value, space = 'rgb', white = 'D65') {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  if (length(value) != 1) value <- rep_len(value, length(colour))
  encode_channel_c(colour, channel, value, space, 5L, white)
}

#' @rdname manip_channel
#' @export
get_channel <- function(colour, channel, space = 'rgb', white = 'D65') {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  decode_channel_c(colour, channel, space, white)
}

encode_channel_c <- function(colour, channel, value, space, op, white) {
  if (channel == 'alpha') {
    channel <- 0L
    space <- 0L
  } else {
    space <- colourspace_match(space)
    channel <- colour_channel_index[[space]][channel]
    if (is.na(channel)) stop('Invalid channel for this colourspace', call. = FALSE)
  }
  
  .Call('encode_channel_c', as_character(colour), as.integer(channel), value, as.integer(space), as.integer(op), white, PACKAGE = 'farver')
}

decode_channel_c <- function(colour, channel, space, white) {
  if (channel == 'alpha') {
    channel <- 0L
    space <- 0L
  } else {
    space <- colourspace_match(space)
    channel <- colour_channel_index[[space]][channel]
    if (is.na(channel)) stop('Invalid channel for this colourspace', call. = FALSE)
  }
  
  .Call('decode_channel_c', as_character(colour), as.integer(channel), as.integer(space), white, PACKAGE = 'farver')
}
