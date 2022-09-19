#' @rdname manip_channel
#' @export
set_channel_native <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_native_c(colour, channel, value, space, 1L, white, na_value)
}

#' @rdname manip_channel
#' @export
add_to_channel_native <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_native_c(colour, channel, value, space, 2L, white, na_value)
}

#' @rdname manip_channel
#' @export
multiply_channel_native <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_native_c(colour, channel, value, space, 3L, white, na_value)
}

#' @rdname manip_channel
#' @export
raise_channel_native <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_native_c(colour, channel, value, space, 4L, white, na_value)
}

#' @rdname manip_channel
#' @export
cap_channel_native <- function(colour, channel, value, space = 'rgb', white = 'D65', na_value = NA) {
  if (space != 'rgb') {
    white <- as_white_ref(white)
  }
  encode_channel_native_c(colour, channel, value, space, 5L, white, na_value)
}

encode_channel_native_c <- function(colour, channel, value, space, op, white, na_value) {
  if (length(colour) == 0) {
    return(colour)
  }
  if (length(value) == 0) {
    stop("`value` must not be empty", call. = FALSE)
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

  if (!is.integer(colour)) {
    stop('colour must be an integer vector (nativeRaster format)', call. = FALSE)
  }
  
  .Call(`farver_encode_channel_native_c`, colour, as.integer(channel), value, as.integer(space), as.integer(op), white, as.character(na_value))
}
