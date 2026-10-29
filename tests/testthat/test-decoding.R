cols <- c("#404040", "#8FBC8F", "#FFFFE0", "#7AC5CD", "#66CDAA", "#1E90FF", 
          "#CDC0B0", "#CD0000", "#7A67EE", "#FFFACD")
truth <- matrix(c(64L, 143L, 255L, 122L, 102L, 30L, 205L, 205L, 122L, 
                  255L, 64L, 188L, 255L, 197L, 205L, 144L, 192L, 0L, 103L, 250L, 
                  64L, 143L, 224L, 205L, 170L, 255L, 176L, 0L, 238L, 205L), 
                ncol = 3, dimnames = list(NULL, c("r", "g", "b")))
test_that("colours can be decoded", {
  expect_equal(decode_colour(cols), truth)
  expect_equal(decode_colour(cols, to = 'cmy'), convert_colour(truth, 'rgb', 'cmy'))
  expect_equal(decode_colour(cols, to = 'cmyk'), convert_colour(truth, 'rgb', 'cmyk'))
  expect_equal(decode_colour(cols, to = 'hsl'), convert_colour(truth, 'rgb', 'hsl'))
  expect_equal(decode_colour(cols, to = 'hsb'), convert_colour(truth, 'rgb', 'hsb'))
  expect_equal(decode_colour(cols, to = 'hsv'), convert_colour(truth, 'rgb', 'hsv'))
  expect_equal(decode_colour(cols, to = 'lab'), convert_colour(truth, 'rgb', 'lab'))
  expect_equal(decode_colour(cols, to = 'hunterlab'), convert_colour(truth, 'rgb', 'hunterlab'))
  expect_equal(decode_colour(cols, to = 'lch'), convert_colour(truth, 'rgb', 'lch'))
  expect_equal(decode_colour(cols, to = 'luv'), convert_colour(truth, 'rgb', 'luv'))
  expect_equal(decode_colour(cols, to = 'xyz'), convert_colour(truth, 'rgb', 'xyz'))
  expect_equal(decode_colour(cols, to = 'yxy'), convert_colour(truth, 'rgb', 'yxy'))
  expect_equal(decode_colour(cols, to = 'hcl'), convert_colour(truth, 'rgb', 'hcl'))
})

test_that("alpha gets decoded correctly", {
  alpha <- decode_colour(
    paste0(cols[1:6], c("00", "33", "66", "99", "CC", "")), 
    alpha = TRUE
  )[, 'alpha']
  expect_equal(alpha, seq(0, 1, length.out = 6))
})
test_that("all-NA numeric colours decode to one row per input (#57)", {
  expect_equal(nrow(decode_colour(NA_real_)), 1)
  expect_equal(nrow(decode_colour(NA_integer_)), 1)
  expect_equal(nrow(decode_colour(NaN)), 1)
  expect_equal(nrow(decode_colour(rep(NA_integer_, 3))), 3)
  expect_true(all(is.na(decode_colour(rep(NA_integer_, 3)))))
  pal <- farver:::def_palette[-1]
  n <- length(pal)
  expect_equal(decode_colour(c(seq_len(n + 2), NA)), decode_colour(c(pal, pal[1:2], NA)))
  expect_equal(decode_colour(c(0.5, NA)), decode_colour(c("transparent", NA)))
  expect_equal(decode_colour(0.5), decode_colour("transparent"))
  expect_length(get_channel(rep(NA_integer_, 3), "r"), 3)
  expect_length(get_channel(rep(NA_real_, 3), "alpha"), 3)
  expect_length(set_channel(rep(NA_integer_, 3), "r", 100), 3)
})
