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