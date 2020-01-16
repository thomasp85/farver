This release addresses the problems of the last release that had to be 
withdrawn. farver should now recognise all of the same colour formats as 
col2rgb(), namely colour names + "transparent"", numerics indexing into 
palette(), and numerals given as strings behaving like numerics. It further 
recognises the "NA" string and treats it as NA_character_

I have done extensive reverse dependency checks which include the reverse 
dependencies of scales and ggplot2 without detecting any problems related to 
colour conversion, so I'm hopeful that this upgrade will be smooth.

## Test environments
* local R installation, R 3.6.0
* ubuntu 16.04 (on travis-ci), R 3.6.0
* win-builder (devel)

## R CMD check results

0 errors | 0 warnings | 0 note

## revdepcheck results

We checked 3158 reverse dependencies, comparing R CMD check results across CRAN 
dev versions of this package.

 * We saw 0 new problems
 * We failed to check 89 packages
