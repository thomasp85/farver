context("comparison")

spectrum <- unname(t(col2rgb(rainbow(10))))
spectrum2 <- unname(t(col2rgb(heat.colors(5))))
reconvert <- function(data, space) {
  round(convert_colour(convert_colour(data, 'rgb', space), space, 'rgb'))
}
test_that("basic io works", {
  expect_error(compare_colour(spectrum, from_space = 'test', to_space = 'lab', method = 'cmc'))
  expect_error(compare_colour(spectrum, from_space = 'rgb', to_space = 'test', method = 'cmc'))
  expect_error(compare_colour(spectrum, from_space = 'rgb', to_space = 'lab', method = 'test'))
  expect_equal(rep(nrow(spectrum), 2), dim(compare_colour(spectrum, from_space = 'rgb')))
  expect_equal(c(nrow(spectrum), nrow(spectrum2)), dim(compare_colour(spectrum, spectrum2, from_space = 'rgb')))
})

test_that("blue is included in Euclidean comparison (#20)", {
  expect_equal(
    compare_colour(decode_colour("#000000"), decode_colour("#0000FF"), "rgb")[1, 1],
    255
  )
})
