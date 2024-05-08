#' Modify colour space channels in hex-encoded colour strings
#' 
#' This set of functions allows you to modify colours as given by strings, 
#' without first decoding them. For large vectors of colour values this should
#' provide a considerable speedup. The corresponding `_native` functions provide
#' the same feature on [native-encoding] colours.
#' 
#' @param colour A character string giving colours, either as hexadecimal 
#' strings or accepted colour names. The `_native` functions expect an integer
#' vector, describing colours in [native-encoding] format.
#' @param channel The channel to modify or extract as a single letter, or 
#' `'alpha'` for the alpha channel.
#' @param value The value to modify with
#' @param space The colour space the channel pertains to. Allowed values are: 
#' `"cmy"`, `"cmyk"`, `"hsl"`, `"hsb"`, `"hsv"`, `"lab"` (CIE L*ab), `"hunterlab"` 
#' (Hunter Lab), `"oklab"` , `"lch"` (CIE Lch(ab) / polarLAB), `"luv"`, 
#' `"rgb"` (sRGB), `"xyz"`, `"yxy"` (CIE xyY), `"hcl"` (CIE Lch(uv) / polarLuv),
#' or `"oklch"` (Polar form of oklab)
#' @param white The white reference of the channel colour space. Will only have 
#' an effect for relative colour spaces such as Lab and luv. Any value accepted 
#' by [as_white_ref()] allowed.
#' @param na_value A valid colour string or `NA` to use when `colour` contains
#' `NA` elements. The general approach in farver is to carry `NA` values over,
#' but if you want to mimick [col2rgb()] you should set 
#' `na_value = 'transparent'`, i.e. treat `NA` as transparent white.
#' @param skip_na_values By default (`skip_na_values = FALSE`), if `value` is `NA` the colour
#' will be set to `NA`. If `skip_na_values` is `TRUE` and the `value` to modify with
#' is `NA`, the colour won't be modified.
#' 
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
#' # This returns NA, because the replacement value is NA
#' set_channel("#FF0000", "r", NA)
#' # This returns "red", because we skip modifications on NA values:
#' set_channel("red", "r", NA, skip_na_values = TRUE)
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
set_channel <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA, skip_na_values = FALSE) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_c(colour, channel, value, space, 1L, white, na_value, skip_na_values)
}

#' @rdname manip_channel
#' @export
add_to_channel <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA, skip_na_values = FALSE) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_c(colour, channel, value, space, 2L, white, na_value, skip_na_values)
}

#' @rdname manip_channel
#' @export
multiply_channel <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA, skip_na_values = FALSE) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_c(colour, channel, value, space, 3L, white, na_value, skip_na_values)
}

#' @rdname manip_channel
#' @export
raise_channel <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA, skip_na_values = FALSE) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_c(colour, channel, value, space, 4L, white, na_value, skip_na_values)
}

#' @rdname manip_channel
#' @export
cap_channel <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA, skip_na_values = FALSE) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_c(colour, channel, value, space, 5L, white, na_value, skip_na_values)
}

#' @rdname manip_channel
#' @export
get_channel <- function(colour, channel, space = 'rgb', white = 'D65', na_value = NA) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  decode_channel_c(colour, channel, space, white, na_value)
}

encode_channel_c <- function(colour, channel, value, space, op, white, na_value, skip_na_values) {
  if (length(colour) == 0) {
    return(colour)
  }
  if (length(value) == 0) {
    stop("`value` must not be empty", call. = FALSE)
  }
  if (is.logical(value)) {
    if (!all(is.na(value))) {
      stop("`value` must be a numeric", call. = FALSE)
    }
    value <- NA_integer_
  }
  if (length(value) != 1) value <- rep_len(value, length(colour))
  if (channel == 'alpha') {
    channel <- 0L
    space <- 0L
  } else {
    space <- colourspace_match(space)
    channel <- colour_channel_index[[space]][channel]
    if (is.na(channel)) stop('Invalid channel for this colourspace', call. = FALSE)
  }
  skip_na_values <- isTRUE(skip_na_values)
  .Call(`farver_encode_channel_c`, as_colour_code(colour), as.integer(channel), value, as.integer(space), as.integer(op), white, as.character(na_value), skip_na_values)
}

decode_channel_c <- function(colour, channel, space, white, na_value) {
  if (channel == 'alpha') {
    channel <- 0L
    space <- 0L
  } else {
    space <- colourspace_match(space)
    channel <- colour_channel_index[[space]][channel]
    if (is.na(channel)) stop('Invalid channel for this colourspace', call. = FALSE)
  }
  
  .Call(`farver_decode_channel_c`, as_colour_code(colour), as.integer(channel), as.integer(space), white, as.character(na_value))
}
