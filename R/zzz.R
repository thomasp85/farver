def_palette <- NULL
def_palette_values <- NULL
old_palette <- c("black", "red", "green3", "blue", "cyan", "magenta", "yellow", 
                 "gray")
old_palette_values <- matrix(
  c(0L, 0L, 0L, 1L, 
    255L, 0L, 0L, 1L, 
    0L, 205L, 0L, 1L, 
    0L, 0L, 255L, 1L, 
    0L, 255L, 255L, 1L, 
    255L, 0L, 255L, 1L, 
    255L, 255L, 0L, 1L, 
    190L, 190L, 190L, 1L), 
  nrow = 4
)
modern_palette <- c("black", "#DF536B", "#61D04F", "#2297E6", "#28E2E5", 
                    "#CD0BBC", "#F5C710", "gray62")
modern_palette_values <- matrix(
  c(0L, 0L, 0L, 1L, 
    223L, 83L, 107L, 1L, 
    97L, 208L, 79L, 1L, 
    34L, 151L, 230L, 1L, 
    40L, 226L, 229L, 1L, 
    205L, 11L, 188L, 1L, 
    245L, 199L, 16L, 1L, 
    158L, 158L, 158L, 1L),
  nrow = 4
)
.onLoad <- function(...) {
  if (getRversion() < "4.0.0") {
    def_palette <<- c("transparent", old_palette)
    def_palette_values <<- cbind(c(255L, 255L, 255L, 0L), old_palette_values)
  } else {
    def_palette <<- c("transparent", modern_palette)
    def_palette_values <<- cbind(c(255L, 255L, 255L, 0L), modern_palette_values)
  }
  load_colour_names()
}
