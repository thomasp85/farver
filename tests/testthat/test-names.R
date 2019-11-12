names <- c("#404040", "#8FBC8F", "#FFFFE0", "#7AC5CD", "#66CDAA", "#1E90FF", 
           "#CDC0B0", "#CD0000", "#7A67EE", "#FFFACD")

cols <- decode_colour(names)
cols_named <- cols
rownames(cols_named) <- names
codes_named <- names
names(codes_named) <- names

test_that("names gets transfered", {
  expect_equal(names(encode_colour(cols_named)), names)
  expect_null(names(encode_colour(cols)))
  
  expect_equal(rownames(decode_colour(codes_named)), names)
  expect_null(rownames(decode_colour(names)))
  
  expect_equal(rownames(convert_colour(cols_named, 'rgb', 'lab')), names)
  expect_null(rownames(convert_colour(cols, 'rgb', 'lab')))
  
  col_dist <- compare_colour(cols, cols_named, 'rgb')
  expect_equal(dimnames(col_dist), list(NULL, names))
})
