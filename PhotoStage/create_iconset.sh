convert appicon.png -resize 16x16\! appicon.iconset/icon_16x16.png
convert appicon.png -resize 32x32\! appicon.iconset/icon_32x32.png
cp appicon.iconset/icon_32x32.png appicon.iconset/icon_16x16\@2x.png
convert appicon.png -resize 64x64\! appicon.iconset/icon_32x32\@2x.png
convert appicon.png -resize 128x128\! appicon.iconset/icon_128x128.png
convert appicon.png -resize 256x256\! appicon.iconset/icon_256x256.png
cp appicon.iconset/icon_256x256.png appicon.iconset/icon_128x128\@2x.png
convert appicon.png -resize 512x512\! appicon.iconset/icon_512x512.png
cp appicon.iconset/icon_512x512.png appicon.iconset/icon_256x256\@2x.png
convert appicon.png -resize 1024x1024\! appicon.iconset/icon_512x512\@2x.png
iconutil -c icns appicon.iconset
