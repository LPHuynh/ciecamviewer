# CIECAM02 Viewer
CIECAM02 Viewer is CIECAM02-based colour space browser, colour schemer, and image mapping software.

## Authors

- Long Huynh  — main developer (longp.huynh@gmail.com)

## Source Papers/Credits

- Fairchild, Mark D. Color Appearance Models Second Edition. 2005. John Wiley & Sons Ltd.
- Hu, Guosheng et al. An Interactive Method for Generating Harmonious Color Schemes. 2012. COLOR research and application. DOI 10.1002/col.21762.
- Hu, Guosheng et al. A User-Oriented Method for Preferential Color Scheme Generation, 2014. COLOR research and application.  DOI 10.1002/col.21860.
- Yamazaki, Hideki and Kondo, Kunio. A Method of Changing a Color Scheme with Kansei Scales. 1999. Journal for Geometry and Graphics Volume 3, No. 1, 77–84.

## Downloads

- Windows(x64): https://github.com/LPHuynh/ciecamviewer/releases

## Compiling

- Requires SFML (https://www.sfml-dev.org/) and imgui-sfml (https://github.com/eliasdaler/imgui-sfml) as dependencies.
- JSON for Modern C++ (https://github.com/nlohmann/json) is included with the project.

## Usage

- Double-clicking the titlebar of any sub windows will roll/unroll them.
- Clicking any slider while holding down the Ctrl key will allow you to enter a value
- images to be analysed should be placed in the same folder as the executable. 
- Jpeg, PNG, BMP, TGA, and Gif file format are supported by the image analyser. File extensions are automatically detected.

## Colour Schemer

- The colour schemer included in this program is loosely based on the work on Hu et al.
- It has been updated to utilize the newer CIECAM02 colour space.

## Kansei Scale Rating

- The ratings given to images and colour schemes are very loosely based on the data from Yamazaki & Kondo
- It has been updated to utilize the newer CIECAM02 colour space.
- In the original paper, "warm vs cool" is based solely on hue angle, closest to the orange or cyan hue angle. However, this would imply that brunt umber and sepia are warmer than pyrrole red. I disagree with this, and have made it so that the warmth vs coolness of a colour is based on its proportional colour differences from an imaginary orange (JCh: -,120,40) and cyan (JCh: -,120,220). The degree of impression does not have to be reevaluated since there are only one evaluation element affecting this Kansei scale.
- "Gay vs quiet" is likely a mistranslation in the original paper, and has been renamed to "active vs passive" for better clarity (Original studies took place in Japan).
- In the original paper, only the relative placement of colours on each Kansei scale to each other mattered. To better fit the goals of this program (colour schemes are rated independently from one another), I have set an arbitrary neutral point for each of the five scales and scaled each extremities accordingly.
- Due to the numerous changes in methodology from the original paper, this program IS NOT representation of the original paper's work.

## Known Issues

- Standard Illuminant E is known to give  erratic results. This is likely due to a div/0 error. However, given the multi-step complexity of the CIE XYZ -> CIECAM02 conversion process, the author of this program has not yet determined whether this div/0 is due to premature simplification of the mathematical formulas involved or if Standard Illuminant E is simply incompatible with the CIECAM02 colour space.
