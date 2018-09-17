context("conversion")

spectrum <- unname(t(col2rgb(rainbow(10))))
reconvert <- function(data, space) {
  unname(round(convert_colour(convert_colour(data, 'rgb', space), space, 'rgb')))
}
test_that("basic io works", {
  expect_error(convert_colour(spectrum, 'test', 'lab'))
  expect_error(convert_colour(spectrum, 'rgb', 'test'))
  expect_equal(nrow(spectrum), nrow(convert_colour(spectrum, 'rgb', 'lab')))
})

test_that("cmy works", {
  expect_equal(spectrum, reconvert(spectrum, 'cmy'))
})
test_that("cmyk works", {
  expect_equal(spectrum, reconvert(spectrum, 'cmyk'))
})
test_that("hsl works", {
  expect_equal(spectrum, reconvert(spectrum, 'hsl'))
})
test_that("hsb works", {
  expect_equal(spectrum, reconvert(spectrum, 'hsb'))
})
test_that("hsv works", {
  expect_equal(spectrum, reconvert(spectrum, 'hsv'))
})
test_that("lab works", {
  expect_equal(spectrum, reconvert(spectrum, 'lab'))
})
test_that("hunterlab works", {
  expect_equal(spectrum, reconvert(spectrum, 'hunterlab'))
})
test_that("lch works", {
  expect_equal(spectrum, reconvert(spectrum, 'lch'))
})
test_that("luv works", {
  expect_equal(spectrum, reconvert(spectrum, 'luv'))
})
test_that("rgb works", {
  expect_equal(spectrum, reconvert(spectrum, 'rgb'))
})
test_that("xyz works", {
  expect_equal(spectrum, reconvert(spectrum, 'xyz'))
})
test_that("yxy works", {
  expect_equal(spectrum, reconvert(spectrum, 'yxy'))
})
