#page_correlation

Compares images based on layout anaylisis, pixel correlation and image dimensions using leptonica library. Provides utility to generate html page displaying compared images side by side.

```
Usage: page_correlation [options] [--] <image_file1> <image_file2>

--size-threshold=,-s<[0,1]> (default: 0.6)
        Images will be qual only if their sizes ratio is lower then size-threshold.
        1 means their sizes must be equal
        0 their sizes does not matter

--correlation-threshold=,c<[0,1]> (default: 0.6)
        If layout analysis fails pixel correlation is computed.
        1 means they are equal
        0 means they are different

--threshold-method=,-m<otsu|threshold|dither> (default: otsu)
        Otsu is adaptive thresholding method which try select best value for an image.
        Threshold will make thresholding using --threshold argument
        Dither

--threshold=,-t<n> (default: 128)
        Binarization parameter. Everything below is black and above white.

--min-lines=,-l<n> (default: 3)
        Minimum n textual lines have to be equal.

--min-words=,-v<n> (default: 3)
        Minimum n words have to be equal to consider lines equal.

--blank-threshold=,-b<pixels> (default: 3000)
        Number of black pixels allowed in image to consider it as a blank page.

--width=,-w<n> (default: 800)
--height=,-h<n> (default: 1100)
        To speed up page analysis images are scaled down.
        0 for one argument means keep aspect ratio.

--disable-text-correlation
        If layout analysis thinks two images does not match then pixel correlation matters.
        This disable pixel correlation for text documents.
--help,-?

--
        Switch off option parsing. Useful if image file name starts with '-'.

Returns 0 if images seems to be equal and 1 otherwise.
Returns 100 on argument error.
Returns 101 resp 102 if image 1 resp image 2 does not exist.
```

#page_correlation2html

generates thumbnails and htmlpages based on page_correlation outputs

```
page_correlation2html <log file|-> <out_dir>
```
