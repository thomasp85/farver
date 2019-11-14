codes <- c("#404040", "#8FBC8F", "#FFFFE0", "#7AC5CD", "#66CDAA", "#1E90FF", 
          "#CDC0B0", "#CD0000", "#7A67EE", "#FFFACD")

codes_alpha <- paste0(codes, c('', '1f', '44', 'fe', '', '9e', 'aa', 'b2', '0c', '21'))

cols <- decode_colour(codes)
alpha <- decode_colour(codes_alpha, TRUE)[, 'alpha']
cols_mod <- cols

cols_lch <- convert_colour(cols, 'rgb' ,'lch')
cols_lch_mod <- cols_lch

test_that("setting channel works", {
  cols_mod[, 'g'] <- 1:10
  expect_equal(set_channel(codes, 'g', 1:10), encode_colour(cols_mod))
  
  cols_lch_mod[, 'l'] <- 1:10
  expect_equal(set_channel(codes, 'l', 1:10, 'lch'), encode_colour(cols_lch_mod, from = 'lch'))
  
  expect_equal(set_channel(codes_alpha, 'alpha', (1:10)/10), encode_colour(cols, alpha = (1:10)/10))
})

test_that("adding channel works", {
  cols_mod[, 'r'] <- cols_mod[, 'r'] + 1:10
  expect_equal(add_to_channel(codes, 'r', 1:10), encode_colour(cols_mod))
  
  cols_lch_mod[, 'c'] <- cols_lch_mod[, 'c'] + 1:10
  expect_equal(add_to_channel(codes, 'c', 1:10, 'lch'), encode_colour(cols_lch_mod, from = 'lch'))
  
  skip_on_os('linux') # Rounding difference on someones aarch64/ppc64le processor
  expect_equal(add_to_channel(codes_alpha, 'alpha', (1:10)/10), encode_colour(cols, alpha = alpha + (1:10)/10))
})

test_that("multiply channel works", {
  cols_mod[, 'b'] <- cols_mod[, 'b'] * 1:10
  expect_equal(multiply_channel(codes, 'b', 1:10), encode_colour(cols_mod))
  
  cols_lch_mod[, 'h'] <- cols_lch_mod[, 'h'] * 1:10
  expect_equal(multiply_channel(codes, 'h', 1:10, 'lch'), encode_colour(cols_lch_mod, from = 'lch'))
  
  expect_equal(multiply_channel(codes_alpha, 'alpha', 1:10), encode_colour(cols, alpha = alpha * 1:10))
})

test_that("raising channel works", {
  cols_mod[, 'g'] <- ifelse(cols_mod[, 'g'] < 200, 200, cols_mod[, 'g'])
  expect_equal(raise_channel(codes, 'g', 200), encode_colour(cols_mod))
  
  cols_lch_mod[, 'l'] <- ifelse(cols_lch_mod[, 'l'] < 50, 50, cols_lch_mod[, 'l'])
  expect_equal(raise_channel(codes, 'l', 50, 'lch'), encode_colour(cols_lch_mod, from = 'lch'))
  
  expect_equal(raise_channel(codes_alpha, 'alpha', 0.5), encode_colour(cols, alpha = ifelse(alpha < 0.5, 0.5, alpha)))
})

test_that("capping channel works", {
  cols_mod[, 'g'] <- ifelse(cols_mod[, 'g'] > 200, 200, cols_mod[, 'g'])
  expect_equal(cap_channel(codes, 'g', 200), encode_colour(cols_mod))
  
  cols_lch_mod[, 'l'] <- ifelse(cols_lch_mod[, 'l'] > 50, 50, cols_lch_mod[, 'l'])
  expect_equal(cap_channel(codes, 'l', 50, 'lch'), encode_colour(cols_lch_mod, from = 'lch'))
  
  expect_equal(cap_channel(codes_alpha, 'alpha', 0.5), encode_colour(cols, alpha = ifelse(alpha > 0.5, 0.5, alpha)))
})
