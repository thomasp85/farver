bad_codes <- c('#345', '#e51f87', 'steelblue', 'test', NA)
bad_colours <- cbind(
  c(34, NaN, Inf, NA, -Inf, 67),
  c(34, 0, 0, 0, 0, 67),
  c(34, 0, 0, 0, 0, 67)
)

test_that("non-finite values are treated correctly", {
  expect_error(decode_colour(bad_codes), 'Malformed colour string `#345`')
  expect_error(decode_colour(bad_codes[-1]), 'Unknown colour name: test')
  expect_true(all(is.na(decode_colour(bad_codes[-c(1, 4)]))[3, ]))
  expect_false(any(is.na(decode_colour(bad_codes[-c(1, 4)]))[1:2, ]))
  
  expect_true(all(is.na(encode_colour(bad_colours))[2:5]))
  expect_false(any(is.na(encode_colour(bad_colours))[c(1,6)]))
  
  expect_true(all(is.na(convert_colour(bad_colours, 'rgb', 'lab')[2:5, ])))
  expect_false(any(is.na(convert_colour(bad_colours, 'rgb', 'lab')[c(1,6), ])))
  
  col_compare <- compare_colour(bad_colours, from_space = 'rgb')
  col_compare <- col_compare[upper.tri(col_compare)]
  
  expect_true(all(is.na(col_compare[-11])))
  expect_false(any(is.na(col_compare[11])))
})
