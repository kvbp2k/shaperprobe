make -f Makefile.osx
lipo -detailed_info prober

rm -rf ShaperProbe ShaperProbe.dmg
mkdir ShaperProbe
mv prober ShaperProbe/ShaperProbe

./create-dmg --window-size 500 300 --volname "ShaperProbe" ShaperProbe.dmg ./ShaperProbe/

