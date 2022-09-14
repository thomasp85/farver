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
  expect_equal(encode_colour(list(cols_cmy[,1], cols_cmy[,2], cols_cmy[,3]), from = 'cmy'), cols)
})


test_that("can provide a colour to be used when NA are found", {
  na_color <- "red"
  cols_dec_na <- cols_dec
  cols_dec_na[2,3] <- NA
  cols_na <- cols
  cols_na[2] <- NA
  
  expect_equal(encode_colour(cols_dec_na), cols_na)
  cols_na[2] <- na_color
  expect_equal(encode_colour(cols_dec_na, na_colour = na_color), cols_na)
  
  native_cols_na <- native_cols
  native_cols_na[2] <- NA
  expect_equal(encode_native(cols_dec_na), native_cols_na)
  native_cols_na[2] <- farver::encode_native(na_color)
  expect_equal(encode_native(cols_dec_na, na_colour = na_color), native_cols_na)
})


test_that("Encoding with alpha is consistent", {
  red_no_alpha <- "#FF0000"
  red_with_alphas <- c("#FF0000", "#FF0000FF", "#FF0000AA", "#FF000000")
  red_with_alphas_exp <- c("#FF0000", "#FF0000", "#FF0000AA", "#FF000000")
  red_with_alphas_dec <- decode_colour(red_with_alphas, alpha = TRUE)
  red_with_alphas_dec[1,4] <- NA
  expect_equal(encode_colour(red_with_alphas_dec, alpha = NULL), rep(red_no_alpha, nrow(red_with_alphas_dec)))
  expect_equal(encode_colour(red_with_alphas_dec, alpha = NA), rep(red_no_alpha, nrow(red_with_alphas_dec)))
  red_with_alphas_exp <- red_with_alphas
  red_with_alphas_exp[2] <- "#FF0000"
  expect_equal(encode_colour(red_with_alphas_dec, alpha = red_with_alphas_dec[,4]), red_with_alphas_exp)
})
