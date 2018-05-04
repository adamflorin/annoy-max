# [annoy]

`[annoy]` is a Max wrapper around [`annoy`](https://github.com/spotify/annoy),
an Approximate Nearest Neighbors search.

## Developing

### Installing dependencies

[Clone annoy](https://github.com/spotify/annoy) and make sure that `annoy/src` is linked to in Xcode "Header Search Paths".

### Building

For Mac and PC:

- Download the [Max SDK](https://github.com/Cycling74/max-sdk)
- Clone this repository into `source`
- Open Visual Studio or Xcode project file in `cue/source` and build
  (2x on Windows)
- Find build in SDK `externals` directory and copy to `externals` directory here
