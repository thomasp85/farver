cols <- c("#404040", "#8FBC8F", "#FFFFE0", "#7AC5CD", "#66CDAA", "#1E90FF", 
  "#CDC0B0", "#CD0000", "#7A67EE", "#FFFACD")
cols_dec <- decode_colour(cols)
native_cols <- encode_native(cols)

test_that("colours can be encoded", {
  expect_equal(encode_colour(cols_dec), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'cmy'), from = 'cmy'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'cmyk'), from = 'cmyk'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'hsl'), from = 'hsl'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'hsb'), from = 'hsb'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'hsv'), from = 'hsv'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'lab'), from = 'lab'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'hunterlab'), from = 'hunterlab'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'lch'), from = 'lch'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'luv'), from = 'luv'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'xyz'), from = 'xyz'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'yxy'), from = 'yxy'), cols)
  expect_equal(encode_colour(convert_colour(cols_dec, 'rgb', 'hcl'), from = 'hcl'), cols)
})

test_that("alpha gets encoded correctly", {
  alpha_col <- encode_colour(cols_dec[1:6,], alpha = seq(0, 1, length.out = 6))
  expect_equal(substr(alpha_col, 8, 9), c("00", "33", "66", "99", "CC", ""))
})



test_that("colours can be encoded to native", {
  expect_equal(encode_native(cols_dec), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'cmy'), from = 'cmy'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'cmyk'), from = 'cmyk'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'hsl'), from = 'hsl'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'hsb'), from = 'hsb'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'hsv'), from = 'hsv'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'lab'), from = 'lab'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'hunterlab'), from = 'hunterlab'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'lch'), from = 'lch'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'luv'), from = 'luv'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'xyz'), from = 'xyz'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'yxy'), from = 'yxy'), native_cols)
  expect_equal(encode_native(convert_colour(cols_dec, 'rgb', 'hcl'), from = 'hcl'), native_cols)
})


test_that("colours can be encoded from a list of channels", {
  cols_cmy <- convert_colour(cols_dec, 'rgb', 'cmy')
  expect_equal(encode_colour(list(cols_dec[,1], cols_dec[,2], cols_dec[,3])), cols)
  expect_equal(encode_colour(data.frame(cols_dec[,1], cols_dec[,2], cols_dec[,3])), cols)
  expect_equal(encode_colour(list(cols_cmy[,1], cols_cmy[,2], cols_cmy[,3]), from = 'cmy'), cols)
  expect_equal(encode_colour(data.frame(cols_cmy[,1], cols_cmy[,2], cols_cmy[,3]), from = 'cmy'), cols)
})

