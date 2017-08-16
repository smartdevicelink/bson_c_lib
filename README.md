# README #
Library for converting to and from BSON

## Build library ##
```bash
./configure
make
```

## Install library ##
```bash
sudo make install
```

### Apple Platforms ###
There is a CocoaPod for iOS, MacOS, tvOS, and watchOS. Add to your podfile:

```ruby
pod 'BiSON'
```

## Build and run sample program ##
```bash
cd examples
gcc -o sample sample.c -lbson
./sample
```
